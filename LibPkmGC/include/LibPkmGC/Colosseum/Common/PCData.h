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

#ifndef _LIBPKMGC_COLOSSEUM_PC_DATA_H
#define _LIBPKMGC_COLOSSEUM_PC_DATA_H

#include <LibPkmGC/GC/Common/PCData.h>
#include <LibPkmGC/Colosseum/Common/PokemonBox.h>

namespace LibPkmGC {
namespace Colosseum {

/*
	0x00: PokemonBox boxes[8]
	0x6dec -- end(0x7198): items[235]
*/

class LIBPKMGC_DECL PCData :
	public GC::PCData
{
public:
	static const size_t size = 0x7198;
	PCData(void);
	PCData(PCData const& other);
	PCData(const u8* inData);
	~PCData(void);

	PCData* clone(void) const;
	PCData* create(void) const;

	void save(void);
protected:
	void loadFields(void);
private:
	PCData(XD::PCData const&);
};

}
}

#endif
