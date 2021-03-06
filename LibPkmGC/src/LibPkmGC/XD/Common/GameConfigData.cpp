/*
* Copyright (C) TuxSH 2015
* This file is part of LibPkmGC.
*
* LibPkmGC is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* LibPkmGC is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with LibPkmGC.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <LibPkmGC/XD/Common/GameConfigData.h>


namespace LibPkmGC {
namespace XD {

GameConfigData::GameConfigData(void) : GC::GameConfigData(0x88) {
	initWithEmptyData();
}

GameConfigData::GameConfigData(const u8* inData) : GC::GameConfigData(0x88, inData) {
	load();
}

GameConfigData::GameConfigData(GameConfigData const& other) : GC::GameConfigData(other) {
}


GameConfigData::~GameConfigData(void) {
}

void GameConfigData::loadFields(void) {
	GC::GameConfigData::loadFields();
	LD_FIELD(s32, headerChecksum, 0x38);
}

void GameConfigData::save(void) {
	GC::GameConfigData::save();
	std::fill(data + 4, data + 8, 0);
	SV_FIELD(s32, headerChecksum, 0x38);
}

GameConfigData* GameConfigData::clone(void) const {
	return new GameConfigData(*this);
}

GameConfigData* GameConfigData::create(void) const {
	return new GameConfigData;
}

}
}