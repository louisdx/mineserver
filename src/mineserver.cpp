/*
   Copyright (c) 2011, The Mineserver Project
   All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the The Mineserver Project nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>

#include <sys/stat.h>
#ifdef WIN32
#include <conio.h>
#include <winsock2.h>
#include <process.h>
#include <direct.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <sys/times.h>
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <cassert>
#include <deque>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <event.h>
#include <ctime>
#include <vector>
#include <zlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <boost/bind.hpp>

#include "constants.h"
#include "mineserver.h"
#include "logger.h"
#include "sockets.h"
#include "tools.h"
#include "map.h"
#include "user.h"
#include "worldgen/mapgen.h"
#include "worldgen/nethergen.h"
#include "worldgen/heavengen.h"
#include "worldgen/biomegen.h"
#include "worldgen/eximgen.h"
#include "config.h"
#include "config/node.h"
#include "nbt.h"
#include "./packets/packets.h"
#include "physics.h"
#include "plugin.h"
#include "furnaceManager.h"
#include "cliScreen.h"
#include "hook.h"
#include "mob.h"
//#include "minecart.h"
#ifdef WIN32
static bool quit = false;
#endif

int setnonblock(int fd)
{
#ifdef WIN32
  u_long iMode = 1;
  ioctlsocket(fd, FIONBIO, &iMode);
#else
  int flags;

  flags  = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
#endif

  return 1;
}

// Handle signals
void sighandler(int sig_num)
{
  Mineserver::get()->stop();
}

#ifndef WIN32
void pipehandler(int sig_num)
{
  //Do nothing
}
#endif

std::string removeChar(std::string str, const char* c)
{
  const size_t loc = str.find(c);
  if (loc != std::string::npos)
  {
    return str.replace(loc, 1, "");
  }
  return str;
}

// What is the purpose of "code"? -- louisdx
int printHelp(int code)
{
  std::cout
      << "Mineserver " << VERSION << "\n"
      << "Usage: mineserver [CONFIG_FILE] [OVERRIDE]...\n"
      << "   or: mineserver -h|--help\n"
      << "\n"
      << "Syntax for overrides is: +VARIABLE=VALUE\n"
      << "\n"
      << "Examples:\n"
      << "  mineserver /etc/mineserver/config.cfg +system.path.home=\"/var/lib/mineserver\" +net.port=25565\n";
  return code;
}


int main(int argc, char* argv[])
{
  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);

#ifndef WIN32
  signal(SIGPIPE, pipehandler);
#else
  signal(SIGBREAK, sighandler);
#endif

  std::srand((uint32_t)std::time(NULL));

  std::string cfg;
  std::vector<std::string> overrides;

  for (int i = 1; i < argc; i++)
  {
    const std::string arg(argv[i]);

    switch (arg[0])
    {
    case '-':   // option
      // we have only '-h' and '--help' now, so just return with help
      return printHelp(EXIT_SUCCESS);

    case '+':   // override
      overrides.push_back(arg.substr(1));
      break;

    default:    // otherwise, it is config file
      if (!cfg.empty())
      {
        LOG2(ERROR, "Only single CONFIG_FILE argument is allowed!");
        return EXIT_FAILURE;
      }
      cfg = arg;
      break;
    }
  }

  const std::string DEFAULT_HOME = pathExpandUser(std::string("~/.mineserver"));

  // create home and copy files if necessary
  // TODO: winex: this one needs cfg and overrides to be taken into account!
  Mineserver::get()->homePrepare(DEFAULT_HOME);

#ifdef DEBUG
  // using CONFIG_FILE in current directory is only for development purposes
  const std::string cfgcwd = std::string("./") + CONFIG_FILE;
  struct stat st;
  if ((stat(cfgcwd.c_str(), &st) == 0)) // && S_ISREG(st.st_mode) )
  {
    cfg = cfgcwd;
  }
#endif

  if (cfg.empty())
  {
    cfg = DEFAULT_HOME + '/' + CONFIG_FILE;
  }

  // load config
  if (!Mineserver::get()->config()->load(cfg))
  {
    return EXIT_FAILURE;
  }

  LOG2(INFO, "Using config: " + cfg);

  if (overrides.size())
  {
    std::stringstream override_config;
    for (size_t i = 0; i < overrides.size(); i++)
    {
      LOG2(INFO, "Overriden: " + overrides[i]);
      override_config << overrides[i] << ';' << std::endl;
    }
    // override config
    if (!Mineserver::get()->config()->load(override_config))
    {
      LOG2(ERROR, "Error when parsing overrides: maybe you forgot to doublequote string values?");
      return EXIT_FAILURE;
    }
  }

  bool ret = Mineserver::get()->init();

  if (!ret)
  {
    LOG2(ERROR, "Failed to start Mineserver!");
  }
  else
  {
    ret = Mineserver::get()->run();
  }

  Mineserver::get()->free();

  return ret ? EXIT_SUCCESS : EXIT_FAILURE;
}


Mineserver::Mineserver()
  :  m_socketlisten  (0),
     m_saveInterval  (0),
     m_lastSave      (std::time(NULL)),
     m_pvp_enabled   (false),
     m_damage_enabled(false),
     m_only_helmets  (false),
     m_running       (false),
     m_eventBase     (NULL),

     // core modules
     m_config        (new Config()),
     m_screen        (new CliScreen()),

     m_plugin        (NULL),
     m_furnaceManager(NULL),
     m_packetHandler (NULL),
     m_inventory     (NULL),
     m_mobs          (NULL),
     mServerUser(new User(-1, SERVER_CONSOLE_UID))
{
  memset(&m_listenEvent, 0, sizeof(event));
  initConstants();
}

Mineserver::~Mineserver()
{
}


bool Mineserver::init()
{
  // expand '~', '~user' in next vars
  bool error = false;
  const char* vars[] =
  {
    "system.path.data",
    "system.path.plugins",
    "system.path.home",
    "system.pid_file",
  };
  for (size_t i = 0; i < sizeof(vars) / sizeof(vars[0]); i++)
  {
    boost::shared_ptr<ConfigNode> node = config()->mData(vars[i]);
    if (!node)
    {
      LOG2(ERROR, std::string("Variable is missing: ") + vars[i]);
      error = true;
      continue;
    }
    if (node->type() != CONFIG_NODE_STRING)
    {
      LOG2(ERROR, std::string("Variable is not string: ") + vars[i]);
      error = true;
      continue;
    }

    const std::string newvalue = pathExpandUser(node->sData());
    node->setData(newvalue);
    LOG2(INFO, std::string(vars[i]) + " = \"" + newvalue + "\"");
  }
  if (error)
  {
    return false;
  }

  const std::string str = config()->sData("system.path.home");
#ifdef WIN32
  if (_chdir(str.c_str()) != 0)
#else
  if (chdir(str.c_str()) != 0)
#endif
  {
    LOG2(ERROR, "Failed to change working directory to: " + str);
    return false;
  }

  // Write PID to file
  std::ofstream pid_out((config()->sData("system.pid_file")).c_str());
  if (!pid_out.fail())
  {
#ifdef WIN32
    pid_out << _getpid();
#else
    pid_out << getpid();
#endif
  }
  pid_out.close();


  // screen::init() needs m_plugin
  m_plugin.reset(new Plugin);

  init_plugin_api();

  if (config()->bData("system.interface.use_cli"))
  {
    // Init our Screen
    screen()->init(VERSION);
  }


  LOG2(INFO, "Welcome to Mineserver v" + VERSION);

  m_saveInterval = m_config->iData("map.save_interval");

  m_only_helmets = m_config->bData("system.armour.helmet_strict");
  m_pvp_enabled = m_config->bData("system.pvp.enabled");
  m_damage_enabled = m_config->bData("system.damage.enabled");

  const char* key = "map.storage.nbt.directories"; // Prefix for worlds config
  if (m_config->has(key) && (m_config->type(key) == CONFIG_NODE_LIST))
  {
    std::auto_ptr< std::list<std::string> > tmp(m_config->mData(key)->keys());

    int n = 0;
    for (std::list<std::string>::const_iterator it = tmp->begin(); it != tmp->end(); ++it)
    {
      boost::shared_ptr<World> newWorld(new World());
      newWorld->m_map.reset(new Map());
      newWorld->m_physics.reset(new Physics);
      newWorld->m_physics->map = n;

      int k = m_config->iData((std::string(key) + ".") + (*it));
      // WARNING: if k is too big this will be an access error! -- louisdx
      switch(k)
      {
      case 0:
        {
          newWorld->m_mapGen.reset(new MapGen());
        }
        break;
      case 1:
        {
          newWorld->m_mapGen.reset(new NetherGen());
        }
        break;
      case 2:
        {
          newWorld->m_mapGen.reset(new HeavenGen());
        }
        break;
      case 3:
        {
          newWorld->m_mapGen.reset(new BiomeGen());
        }
        break;
      case 4:
        {
          newWorld->m_mapGen.reset(new EximGen());
        }
        break;
      default:
        {
          // use biomeGen as default
          newWorld->m_mapGen.reset(new BiomeGen());
          std::ostringstream s;
          s << "Error! Mapgen number " << k << " in config unkown.";
          LOG2(INFO, s.str());
        }
        break;
      }

      mWorlds.push_back(newWorld);
      n++;

    }
  }
  else
  {
    LOG2(WARNING, "Cannot find map.storage.nbt.directories.*");
  }

  if (mWorlds.size() == 0)
  {
    LOG2(ERROR, "No worlds in Config!");
    return false;
  }

  m_furnaceManager.reset(   new FurnaceManager());
  m_packetHandler.reset(    new PacketHandler());
  m_inventory.reset(        new Inventory(m_config->sData("system.path.data") + '/' + "recipes", ".recipe", "ENABLED_RECIPES.cfg"));
  m_mobs.reset(             new Mobs());

  return true;
}

bool Mineserver::free()
{
  // Let the user know we're shutting the server down cleanly
  LOG2(INFO, "Shutting down...");

  // Close the cli session if its in use
  if (config()->bData("system.interface.use_cli"))
  {
    screen()->end();
  }

  // Remove the PID file
#ifdef WIN32
  _unlink((config()->sData("system.pid_file")).c_str());
#else
  unlink((config()->sData("system.pid_file")).c_str());
#endif

  return true;
}


event_base* Mineserver::getEventBase()
{
  return m_eventBase;
}

void saveAllPlayers(NonNull<User> user)
{
  if (user->logged)
  {
    user->saveData();
  }
}

void Mineserver::saveAll()
{
  for (int i = 0; i < mWorlds.size(); i++)
  {
    mWorlds[i]->m_map->saveWholeMap();
  }
  forEachUser(&saveAllPlayers);
}

bool shouldRemoveUser(time_t timeNow, NonNull<User const> const user)
{
  if(user->mQueuedForDelete)
  {
    return true;
  }
  // No data received in 30s, timeout
  if (user->logged && (timeNow - user->lastData) > 30)
  {
    LOG2(INFO, "Player " + user->nick + " timed out");

    return true;
  }
  else if (!user->logged && (timeNow - user->lastData) > 100)
  {
    return true;
  }
  return false;
}

void updateUser(bool damageEnabled, NonNull<User> user)
{
  if (damageEnabled)
  {
    user->checkEnvironmentDamage();
  }
  user->pushMap();
  user->popMap();

  // Underwater check / drowning
  // ToDo: this could be done a bit differently? - Fador
  // -- User::all() == users() - louisdx
  user->isUnderwater();
  if (user->pos.y < 0)
  {
    user->sethealth(user->health - 5);
  }
}

void sendServerTimeToUser(int64_t mapTime, NonNull<User> user)
{
    // Send server time
    Packet pkt;
    pkt << (int8_t)eServerToClientPacket_Time_update << (int64_t)mapTime;
    user->sendAll((uint8_t*)pkt.getWrite(), pkt.getWriteLen());
}

bool Mineserver::run()
{
  uint32_t starttime = (uint32_t)time(0);
  uint32_t tick      = (uint32_t)time(0);


  // load plugins
  if (config()->has("system.plugins") && (config()->type("system.plugins") == CONFIG_NODE_LIST))
  {
    std::auto_ptr< std::list<std::string> > tmp(config()->mData("system.plugins")->keys());
    for (std::list<std::string>::const_iterator it = tmp->begin(); it != tmp->end(); ++it)
    {
      std::string path  = config()->sData("system.path.plugins");
      std::string name  = config()->sData("system.plugins." + (*it));
      std::string alias = *it;
      if (name[0] == '_')
      {
        path = "";
        alias = name;
        name = name.substr(1);
      }

      plugin()->loadPlugin(name, path, alias);
    }
  }

  // Initialize map
  for (int i = 0; i < (int)mWorlds.size(); i++)
  {
    physics(i)->enabled = (config()->bData("system.physics.enabled"));

    mWorlds[i]->m_map->init(i);
    if (config()->bData("map.generate_spawn.enabled"))
    {
      LOG2(INFO, "Generating spawn area...");
      int size = config()->iData("map.generate_spawn.size");
      bool show_progress = config()->bData("map.generate_spawn.show_progress");
#ifdef __FreeBSD__
      show_progress = false;
#endif

#ifdef WIN32
      DWORD t_begin = 0, t_end = 0;
#else
      clock_t t_begin = 0, t_end = 0;
#endif

      for (int x = -size; x <= size; x++)
      {
        if (show_progress)
        {
#ifdef WIN32
          t_begin = timeGetTime();
#else
          t_begin = clock();
#endif
        }
        for (int z = -size; z <= size; z++)
        {
          mWorlds[i]->m_map->loadMap(x, z);
        }

        if (show_progress)
        {
#ifdef WIN32
          t_end = timeGetTime();
          LOG2(INFO, dtos((x + size + 1) *(size * 2 + 1)) + "/" + dtos((size * 2 + 1) *(size * 2 + 1)) + " done. " + dtos((t_end - t_begin) / (size * 2 + 1)) + "ms per chunk");
#else
          t_end = clock();
          LOG2(INFO, dtos((x + size + 1) *(size * 2 + 1)) + "/" + dtos((size * 2 + 1) *(size * 2 + 1)) + " done. " + dtos(((t_end - t_begin) / (CLOCKS_PER_SEC / 1000)) / (size * 2 + 1)) + "ms per chunk");
#endif
        }
      }
    }
#ifdef DEBUG
    LOG(DEBUG, "Map", "Spawn area ready!");
#endif
  }

  // Initialize packethandler
  packetHandler()->init();

  // Load ip from config
  const std::string ip = config()->sData("net.ip");

  // Load port from config
  const int port = config()->iData("net.port");

#ifdef WIN32
  WSADATA wsaData;
  int iResult;
  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0)
  {
    LOG2(ERROR, std::string("WSAStartup failed with error: " + iResult));
    return false;
  }
#endif

  struct sockaddr_in addresslisten;
  int reuse = 1;

  m_eventBase = reinterpret_cast<event_base*>(event_init());
#ifdef WIN32
  m_socketlisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
  m_socketlisten = socket(AF_INET, SOCK_STREAM, 0);
#endif

  if (m_socketlisten < 0)
  {
    LOG2(ERROR, "Failed to create listen socket");
    return false;
  }

  memset(&addresslisten, 0, sizeof(addresslisten));

  addresslisten.sin_family      = AF_INET;
  addresslisten.sin_addr.s_addr = inet_addr(ip.c_str());
  addresslisten.sin_port        = htons(port);

  setsockopt(m_socketlisten, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

  // Bind to port
  if (bind(m_socketlisten, (struct sockaddr*)&addresslisten, sizeof(addresslisten)) < 0)
  {
    LOG2(ERROR, "Failed to bind to " + ip + ":" + dtos(port));
    return false;
  }

  if (listen(m_socketlisten, 5) < 0)
  {
    LOG2(ERROR, "Failed to listen to socket");
    return false;
  }

  setnonblock(m_socketlisten);
  event_set(&m_listenEvent, m_socketlisten, EV_WRITE | EV_READ | EV_PERSIST, accept_callback, NULL);
  event_add(&m_listenEvent, NULL);

  LOG2(INFO, "Listening on: ");
  if (ip == "0.0.0.0")
  {
    // Print all local IPs
    char name[255];
    gethostname(name, sizeof(name));
    struct hostent* hostinfo = gethostbyname(name);
    int ipIndex = 0;
    while (hostinfo && hostinfo->h_addr_list[ipIndex])
    {
      const std::string ip(inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[ipIndex++]));
      LOG2(INFO, ip + ":" + dtos(port));
    }
  }
  else
  {
    LOG2(INFO, ip + ":" + dtos(port));
  }

  timeval loopTime;
  loopTime.tv_sec  = 0;
  loopTime.tv_usec = 200000; // 200ms

  m_running = true;
  event_base_loopexit(m_eventBase, &loopTime);

  // Create our Server Console user so we can issue commands

  time_t timeNow = time(NULL);
  while (m_running && event_base_loop(m_eventBase, 0) == 0)
  {
    event_base_loopexit(m_eventBase, &loopTime);

    // Run 200ms timer hook
    static_cast<Hook0<bool>*>(plugin()->getHook("Timer200"))->doAll();
    // Alert any block types that care about timers
    BlockBasic* blockcb;
    for (size_t i = 0 ; i < plugin()->getBlockCB().size(); i++)
    {
      blockcb = plugin()->getBlockCB()[i];
      if (blockcb != NULL)
      {
        blockcb->timer200();
      }
    }

    //Update physics every 200ms
    for (std::vector<Map*>::size_type i = 0 ; i < mWorlds.size(); i++)
    {
      physics(i)->update();
    }

    //Every 10 seconds..
    timeNow = time(0);
    if (timeNow - starttime > 10)
    {
      starttime = (uint32_t)timeNow;

      //Map saving on configurable interval
      if (m_saveInterval != 0 && timeNow - m_lastSave >= m_saveInterval)
      {
        //Save
        for (std::vector<Map*>::size_type i = 0; i < mWorlds.size(); i++)
        {
          mWorlds[i]->m_map->saveWholeMap();
        }

        m_lastSave = timeNow;
      }

      Mineserver::get()->forEachUser(boost::bind(&sendServerTimeToUser,mWorlds[0]->m_map->mapTime,_1));

      //Check for tree generation from saplings
      for (std::vector<Map*>::size_type i = 0; i < mWorlds.size(); i++)
      {
        mWorlds[i]->m_map->checkGenTrees();
      }

      // TODO: Run garbage collection for chunk storage dealie?

      // Run 10s timer hook
      static_cast<Hook0<bool>*>(plugin()->getHook("Timer10000"))->doAll();
    }

    // Every second
    if (timeNow - tick > 0)
    {
      tick = (uint32_t)timeNow;
      // Loop users

      // removed disconnected users
      forEachUserRemoveIfTrue(boost::bind(&shouldRemoveUser,timeNow,_1));

      for (std::vector<Map*>::size_type i = 0 ; i < mWorlds.size(); i++)
      {
        mWorlds[i]->m_map->mapTime += 20;
        if (mWorlds[i]->m_map->mapTime >= 24000)
        {
          mWorlds[i]->m_map->mapTime = 0;
        }
      }

      // allowed to modify users, but not delete them!
      forEachUser(boost::bind(&updateUser,m_damage_enabled,_1));

      // Check for Furnace activity
      furnaceManager()->update();

      // Run 1s timer hook
      static_cast<Hook0<bool>*>(plugin()->getHook("Timer1000"))->doAll();
    }
  }

#ifdef WIN32
  closesocket(m_socketlisten);
#else
  close(m_socketlisten);
#endif

  saveAll();

  event_base_free(m_eventBase);

  return true;
}

bool Mineserver::stop()
{
  m_running = false;
  return true;
}

bool Mineserver::homePrepare(const std::string& path)
{
  struct stat st;
  if (stat(path.c_str(), &st) != 0)
  {
    LOG2(INFO, "Creating: " + path);
#ifdef WIN32
    if (_mkdir(path.c_str()) == -1)
#else
    if (mkdir(path.c_str(), 0755) == -1)
#endif
    {
      LOG2(ERROR, path + ": " + strerror(errno));
      return false;
    }
  }

  // copy example configs
  const std::string files[] =
  {
    "banned.txt",
    "commands.cfg",
    "config.cfg",
    "item_alias.cfg",
    "ENABLED_RECIPES.cfg",
    "motd.txt",
    "permissions.txt",
    "roles.txt",
    "rules.txt",
    "whitelist.txt",
  };
  for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); i++)
  {
    // TODO: winex: hardcoded path won't work on installation
    const std::string namein  = std::string("../files") + '/' + files[i];
    const std::string nameout = path + '/' + files[i];

    // don't overwrite existing files
    if ((stat(nameout.c_str(), &st) == 0)) // && S_ISREG(st.st_mode) )
    {
      continue;
    }

    std::ifstream fin(namein.c_str(), std::ios_base::binary | std::ios_base::in);
    if (fin.fail())
    {
      LOG2(ERROR, "Failed to open: " + namein);
      continue;
    }

    LOG2(INFO, "Copying: " + nameout);
    std::ofstream fout(nameout.c_str(), std::ios_base::binary);
    if (fout.fail())
    {
      LOG2(ERROR, "Failed to write to: " + nameout);
      continue;
    }

    fout << fin.rdbuf();
  }

  return true;
}

Mineserver* Mineserver::get()
{
  static Mineserver* m_instance = NULL;

  if (!m_instance)
  {
    m_instance = new Mineserver;
  }

  return m_instance;
}

uint32_t Mineserver::generateEID()
{
  static uint32_t m_EID = 0;
  return ++m_EID;
}

void voidSharedToNonNull(boost::function<void(NonNull<User>)> func, boost::shared_ptr<User> user)
{
  NonNull<User> nonNull(user.get());
  func(nonNull);
}

void Mineserver::forEachUser(boost::function<void(NonNull<User>)> func)
{
  std::for_each(m_users.begin(),m_users.end(),boost::bind(&voidSharedToNonNull,func,_1));
}

bool boolSharedToNonNull(boost::function<bool(NonNull<User const> const)> func, boost::shared_ptr<User> user)
{
  return func(NonNull<User const>(user.get()));
}

void Mineserver::forEachUserRemoveIfTrue(boost::function<bool(NonNull<User const> const)> func)
{
  std::vector< boost::shared_ptr<User> >::iterator new_end = std::remove_if(m_users.begin(),m_users.end(),boost::bind(&boolSharedToNonNull,func,_1));

  for(std::vector< boost::shared_ptr<User> >::iterator itr = new_end;
    itr != m_users.end();
    ++itr)
  {
    (*itr)->destructorActions();
  }
  m_users.erase(new_end, m_users.end());
}

Ptr<User> Mineserver::userFromName(std::string user, bool caseSensative)
{
  std::string nick = caseSensative ? user : strToLower(user);
  for (unsigned int i = 0; i < m_users.size(); i++)
  {
    if (m_users[i]->fd && m_users[i]->logged)
    {
      std::string currentNick = caseSensative ? m_users[i]->nick : strToLower(m_users[i]->nick);
      // Don't send to his user if he is DND and the message is a chat message
      if (nick == currentNick)
      {
        return Ptr<User>(m_users[i].get());
      }
    }
  }
  return Ptr<User>();
}

NonNull<User> Mineserver::createUser(int sock)
{
  boost::shared_ptr<User> newUser(new User(sock,Mineserver::generateEID()));
  m_users.push_back(newUser);

  return NonNull<User>(newUser.get());
}

Ptr<User> Mineserver::userFromEID(unsigned int EID)
{
  for (unsigned int i = 0; i < m_users.size(); i++)
  {
    if (m_users[i]->fd && m_users[i]->logged)
    {
      if (EID == m_users[i]->UID)
      {
        return Ptr<User>(m_users[i].get());
      }
    }
  }
  return Ptr<User>();
}

// returns the first user where condition returns true
Ptr<User> Mineserver::findUser(boost::function<bool(NonNull<User>)> condition)
{
  for (unsigned int i = 0; i < m_users.size(); i++)
  {
    NonNull<User> currentUser(m_users[i].get());
    if (condition(currentUser))
    {
      return currentUser.get();
    }
  }
  return Ptr<User>();
}