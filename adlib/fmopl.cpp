/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "fmopl.h"

#include "opl_dosbox.h"
#include "opl_mame.h"

namespace OPL
{
	bool OPL::_hasInstance = false;
} // End of namespace OPL

void OPLDestroy(OPL::OPL* OPL)
{
	delete OPL;
}

void OPLResetChip(OPL::OPL* OPL)
{
	OPL->reset();
}

void OPLWrite(OPL::OPL* OPL, int a, int v)
{
	OPL->write(a, v);
}

unsigned char OPLRead(OPL::OPL* OPL, int a)
{
	return OPL->read(a);
}

void OPLWriteReg(OPL::OPL* OPL, int r, int v)
{
	OPL->writeReg(r, v);
}

void YM3812UpdateOne(OPL::OPL* OPL, int16_t* buffer, int length)
{
	OPL->readBuffer(buffer, length);
}

OPL::OPL* makeAdLibOPL(int rate, bool bMAME)
{
	OPL::OPL* opl;
	if (bMAME)
		opl = new OPL::MAME::OPL();
	else
		opl = new OPL::DOSBox::OPL(OPL::Config::kOpl2);

	if (opl)
	{
		if (!opl->init(rate))
		{
			delete opl;
			opl = 0;
		}
	}

	return opl;
}
