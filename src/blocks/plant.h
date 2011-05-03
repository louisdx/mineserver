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

#pragma once
#include "basic.h"
#include "../util/NonNull.h"

// 10000 == 100%
enum { SEEDS_CHANCE = 1000 };

class User;

class PlantBlock
{
public:
	int x, y, z, map, count;
};

/** BlockPlant deals specifically with plant block functionality
@see BlockBasic
*/
class BlockPlant: public BlockBasic
{
public:
	BlockPlant();
	bool onBroken(User* user, int8_t status, int32_t x, int8_t y, int32_t z, int map, int8_t direction);
	bool onPlace(User* user, int16_t newblock, int32_t x, int8_t y, int32_t z, int map, int8_t direction);
	void onNeighbourPlace(User* user, int16_t newblock, int32_t x, int8_t y, int32_t z, int map, int8_t direction);
	void timer200();
private:
	int grass_timeout;
	int crop_timeout;
	int cactus_timeout;
	int reed_timeout;
	int cactus_max;
	int reed_max;
	bool affectedBlock(int block);
	void addBlocks(int x, int y, int z, int map);
	void addBlock(std::auto_ptr<PlantBlock> p2);
	void addBlock(int x, int y, int z, int map);
	void remBlock(NonNull<PlantBlock> p2);
	void remBlock(int x, int y, int z, int map);
	bool isPlant(int num);
};

