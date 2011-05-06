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
#ifndef ESERVERTOCLIENTPACKET_H
#define ESERVERTOCLIENTPACKET_H

#include "..\util\Enum.h"

enum EServerToClientPacket_raw
{
	eServerToClientPacket_Login_response            = 0x01,
	eServerToClientPacket_Time_update               = 0x04,
	eServerToClientPacket_Spawn_position            = 0x06,
	eServerToClientPacket_Update_health             = 0x08,
	eServerToClientPacket_Add_to_inventory          = 0x11,
	eServerToClientPacket_Named_entity_spawn        = 0x14,
	eServerToClientPacket_Pickup_spawn              = 0x15,
	eServerToClientPacket_Collect_item              = 0x16,
	eServerToClientPacket_Add_object                = 0x17,
	eServerToClientPacket_Mob_spawn                 = 0x18,
	eServerToClientPacket_Destroy_entity            = 0x1d,
	eServerToClientPacket_Entity                    = 0x1e,
	eServerToClientPacket_Entity_relative_move      = 0x1f,
	eServerToClientPacket_Entity_look               = 0x20,
	eServerToClientPacket_Entity_look_relative_move = 0x21,
	eServerToClientPacket_Entity_teleport           = 0x22,
	eServerToClientPacket_Death_animation           = 0x26,
	eServerToClientPacket_Pre_chunk                 = 0x32,
	eServerToClientPacket_Map_chunk                 = 0x33,
	eServerToClientPacket_Multi_block_change        = 0x34,
	eServerToClientPacket_Block_change              = 0x35,
	eServerToClientPacket_Play_note                 = 0x36,
	eServerToClientPacket_Open_window               = 0x64,
	eServerToClientPacket_Progress_bar              = 0x69,
	eServerToClientPacket_Transaction               = 0x6a,
  //eServerToClientPacket_Complex_entities          = 0x3b,
	eServerToClientPacket_Kick                      = 0xff,
	eServerToClientPacket_Entity_velocity 			= 0x1c,
	eServerToClientPacket_Attach_entity   			= 0x27,
	eServerToClientPacket_Weather = 0x47,
	eServerToClientPacket_Increment_statistics = 0xC8,
};

DeclareEnum(EServerToClientPacket_raw,EServerToClientPacket);

#endif //ESERVERTOCLIENTPACKET_H