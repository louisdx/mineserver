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
#include <vector>
#include <string>
#include <stdint.h>
#include "user.h"
#include "constants.h"
#include "./packets/packets.h"
#include "mineserver.h"



uint32_t generateEID();

class Mob
{
public:
  // Singular
  uint32_t UID;
  int8_t type;
  double x, y, z;
  size_t map;
  int8_t yaw, pitch;
  int8_t meta;
  bool spawned;
  bool respawnable;
  int health;

  Mob(); // Dont create your own Mob, use Mobs.createMob()
  ~Mob();
  void sethealth(int health);

  // Specifically packet-based commands
  void spawnToAll();
  void deSpawnToAll();
  void relativeMoveToAll();
  void teleportToAll();
  void animateMob(const char* userIn, int animID);
  void animateDamage(const char* userIn, int animID);
  void moveAnimal(const char* userIn);

  void moveTo(double to_x, double to_y, double to_z, int to_map = -1);
  void look(int16_t yaw, int16_t pitch);
private:
  static void sendMobSpawn(NonNull<Mob> mob, NonNull<User> user);
  static void sendTeleportToAll(NonNull<Mob> mob, NonNull<User> user);
};

class Mobs
{
public:
  Mob* getMobByID(int id);
  size_t getMobByTarget(uint32_t mobID);
  int mobNametoType(std::string name);
  inline size_t getMobCount()
  {
    return m_moblist.size();
  }
  inline std::vector<Mob*> & getAll()
  {
    return m_moblist;
  };
  inline const std::vector<Mob*> & getAll() const
  {
    return m_moblist;
  };
  inline void addMob(Mob* mob)
  {
    m_moblist.push_back(mob);
  }
  inline Mob* createMob()
  {
    Mob* mob = new Mob();
    addMob(mob);
    return mob;
  }

private:
  std::vector<Mob*> m_moblist;
};
