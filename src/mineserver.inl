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

inline std::vector<User*>& Mineserver::users()
{
	return m_users;
}

inline size_t Mineserver::mapCount()
{
	return mWorlds.size();
}

inline NonNull<Mobs> Mineserver::mobs() 
{
	return m_mobs.get();
}

inline NonNull<Plugin> Mineserver::plugin() 
{
	return m_plugin.get();
}

inline NonNull<Screen> Mineserver::screen() 
{
	return m_screen.get();
}

inline NonNull<Physics> Mineserver::physics(size_t n) 
{
	assert(n<mWorlds.size());
	return mWorlds[n]->m_physics.get();
}

inline NonNull<Config> Mineserver::config() 
{
	return m_config.get();
}

inline NonNull<FurnaceManager> Mineserver::furnaceManager() 
{
	return m_furnaceManager.get();
}

inline NonNull<PacketHandler> Mineserver::packetHandler() 
{
	return m_packetHandler.get();
}

inline NonNull<MapGen> Mineserver::mapGen(size_t n) 
{
	assert(n<mWorlds.size());
	return mWorlds[n]->m_mapGen.get();
}

inline NonNull<Inventory> Mineserver::inventory() 
{
	return m_inventory.get();
}

inline Ptr<Map> Mineserver::map(size_t n)
{
	if (n < mWorlds.size())
	{
		return mWorlds[n]->m_map.get();
	}
	
	return NULL;
}