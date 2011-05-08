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

#ifndef _PACKETS_H
#define _PACKETS_H

#include <string.h>
#include <stdint.h>

#include ".\notchian\EClientToServerPacket.h"
#include ".\notchian\EServerToClientPacket.h"
#include "packet.h"

#define PACKET_NEED_MORE_DATA -3
#define PACKET_DOES_NOT_EXIST -2
#define PACKET_VARIABLE_LEN   -1
#define PACKET_OK             0

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

class PacketHandler;
class User;

struct Packets
{
  int len;
  int (PacketHandler::*function)(User*);

  //int (PacketHandler::*varLenFunction)(User *);

  Packets(int newlen = PACKET_DOES_NOT_EXIST)
  : len(newlen)
  {
  }

  /*  Packets(int newlen, void (PacketHandler::*newfunction)(uint *, User *))
    {
      len      = newlen;
      function = newfunction;
    }

    Packets(int newlen, void (PacketHandler::*newfunction)(User *))
    {
      len            = newlen;
      function = newfunction;
    }
  */

  Packets(int newlen, int (PacketHandler::*newfunction)(User*))
  : len(newlen), function(newfunction)
  {
  }

  Packets(const Packets & other)
  : len(other.len), function(other.function)
  {
  }
};

struct packet_login_request
{
  int version;
  std::wstring Username;
  std::wstring Password;
  int64_t map_seed;
  uint8_t dimension;
};

struct packet_player_position
{
  double x;
  double y;
  double stance;
  double z;
  char onground;
};

struct packet_player_look
{
  float yaw;
  float pitch;
  char onground;
};

struct packet_player_position_and_look
{
  double x;
  double y;
  double stance;
  double z;
  float yaw;
  float pitch;
  char onground;
};

class PacketHandler
{
public:
  void init();

  // Information of all the packets
  // around 2kB of memory
  Packets packets[256];

  // The packet functions
  int keep_alive(User* user);
  int login_request(User* user);
  int handshake(User* user);
  int chat_message(User* user);
  int player(User* user);
  int player_position(User* user);
  int player_look(User* user);
  int player_position_and_look(User* user);
  int player_digging(User* user);
  int player_block_placement(User* user);
  int holding_change(User* user);
  int arm_animation(User* user);
  int pickup_spawn(User* user);
  int disconnect(User* user);
  int use_entity(User* user);
  int respawn(User* user);
  int change_sign(User* user);
  int inventory_transaction(User* user);

  int inventory_change(User* user);
  int inventory_close(User* user);
  int destroy_entity(User* user);

  int entity_crouch(User* user);

  int unhadledPacket(User* user);
};

#endif
