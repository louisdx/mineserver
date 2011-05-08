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

#include "..\util\Enum.h"

enum EClientToServerPacket_raw
{
	eClientToServerPacket_Keep_alive                = 0x00,
	eClientToServerPacket_Login_request             = 0x01,
	eClientToServerPacket_Handshake                 = 0x02,
	eClientToServerPacket_Chat_message              = 0x03,
	eClientToServerPacket_Entity_equipment          = 0x05,
	eClientToServerPacket_Respawn                   = 0x09,
	eClientToServerPacket_Player                    = 0x0a,
	eClientToServerPacket_Player_position           = 0x0b,
	eClientToServerPacket_Player_look               = 0x0c,
	eClientToServerPacket_Player_position_and_look  = 0x0d,
	eClientToServerPacket_Player_digging            = 0x0e,
	eClientToServerPacket_Player_block_placement    = 0x0f,
	eClientToServerPacket_Holding_change            = 0x10,
	eClientToServerPacket_Arm_animation             = 0x12,
	eClientToServerPacket_Entity_crouch             = 0x13,
	eClientToServerPacket_Inventory_close           = 0x65,
	eClientToServerPacket_Inventory_change          = 0x66,
	eClientToServerPacket_Set_slot                  = 0x67,
	eClientToServerPacket_Inventory                 = 0x68,
	eClientToServerPacket_Sign                      = 0x82,
	eClientToServerPacket_Disconnect                = 0xff,
	eClientToServerPacket_Use_entity				= 0x07,
};

DeclareEnum(EClientToServerPacket_raw,EClientToServerPacket);
