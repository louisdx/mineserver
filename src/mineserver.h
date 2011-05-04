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

#ifndef _MINESERVER_H
#define _MINESERVER_H

#include <vector>
#include <string>

#ifdef WIN32
// This is needed for event to work on Windows.
#include <Winsock2.h>
#else
//Do not remove!! Required on Debian
#include <sys/types.h>
#endif
#include <event.h>

#include <boost/scoped_ptr.hpp>

#include "util/NonNull.h"

class User;
class Map;
class Chat;
class Plugin;
class Screen;
class Config;
class FurnaceManager;
class PacketHandler;
class Physics;
class MapGen;
class Logger;
class Inventory;
class Mobs;
class Mob;

#define MINESERVER
#include "plugin_api.h"
#undef MINESERVER


struct event_base;

class Mineserver
{
public:
	static Mineserver* get();

	static uint32_t generateEID();


	bool init();
	bool free();

	bool run();
	bool stop();

	event_base* getEventBase();

	inline std::vector<User*>& users();

	struct event m_listenEvent;
	int m_socketlisten;

	int    m_saveInterval;
	time_t m_lastSave;

	bool m_pvp_enabled;
	bool m_damage_enabled;
	bool m_only_helmets;

	Map* map(size_t n);
	inline size_t mapCount();
	inline Chat* chat();
	inline Mobs* mobs();
	inline Plugin* plugin();
	inline NonNull<Screen> screen();
	inline Physics* physics(size_t n);
	inline NonNull<Config> config();
	inline FurnaceManager* furnaceManager();
	inline PacketHandler* packetHandler();
	inline MapGen* mapGen(size_t n);
	inline NonNull<Logger> logger();
	inline Inventory* inventory();

	void saveAllPlayers();
	void saveAll();

	bool homePrepare(const std::string& path);

private:
	Mineserver();
	~Mineserver();

	bool m_running;

	event_base* m_eventBase;

	// holds all connected users
	std::vector<User*>    m_users;

	std::vector<Map*>     m_map;
	std::vector<Physics*> m_physics;
	std::vector<MapGen*>  m_mapGenNames;
	std::vector<MapGen*>  m_mapGen;

	// core modules
	boost::scoped_ptr<Config> m_config;
	boost::scoped_ptr<Screen> m_screen;
	boost::scoped_ptr<Logger> m_logger;

	Plugin*         m_plugin;
	Chat*           m_chat;
	FurnaceManager* m_furnaceManager;
	PacketHandler*  m_packetHandler;
	Inventory*      m_inventory;
	Mobs*           m_mobs;
};

#include "mineserver.inl"
#endif
