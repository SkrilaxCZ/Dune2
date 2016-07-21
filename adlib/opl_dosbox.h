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

/*
 * Based on OPL emulation code of DOSBox
 * Copyright (C) 2002-2009  The DOSBox Team
 * Licensed under GPLv2+
 * http://www.dosbox.com
 */

#ifndef SOUND_SOFTSYNTH_OPL_DOSBOX_H
#define SOUND_SOFTSYNTH_OPL_DOSBOX_H

#include <cstdint>
#include "fmopl.h"

namespace OPL
{
	namespace DOSBox
	{
		class Handler;

		struct Timer
		{
			double startTime;
			double delay;
			bool enabled, overflow, masked;
			uint8_t counter;

			Timer();

			//Call update before making any further changes
			void update(double time);

			//On a reset make sure the start is in sync with the next cycle
			void reset(double time);

			void stop();

			void start(double time, int scale);
		};

		struct Chip
		{
			//Last selected register
			Timer timer[2];
			//Check for it being a write to the timer
			bool write(uint32_t addr, uint8_t val);
			//Read the current timer state, will use current double
			uint8_t read();
		};

		class Handler
		{
		public:
			virtual ~Handler()
			{
			}

			// Write an address to a chip, returns the address the chip sets
			virtual uint32_t writeAddr(uint32_t port, uint8_t val) = 0;
			// Write to a specific register in the chip
			virtual void writeReg(uint32_t addr, uint8_t val) = 0;
			// Generate a certain amount of samples
			virtual void generate(int16_t* chan, unsigned int samples) = 0;
			// Initialize at a specific sample rate and mode
			virtual void init(unsigned int rate) = 0;
		};

		class OPL : public ::OPL::OPL
		{
		private:
			Config::OplType _type;
			unsigned int _rate;

			Handler* _handler;
			Chip _chip[2];

			union
			{
				uint16_t normal;
				uint8_t dual[2];
			} _reg;

			void free();
			void dualWrite(uint8_t index, uint8_t reg, uint8_t val);
		public:
			OPL(Config::OplType type);
			~OPL();

			bool init(int rate) override;
			void reset() override;

			void write(int a, int v) override;
			uint8_t read(int a) override;

			void writeReg(int r, int v) override;

			void readBuffer(int16_t* buffer, int length) override;

			bool isStereo() const override
			{
				return _type != Config::kOpl2;
			}
		};
	} // End of namespace DOSBox
} // End of namespace OPL

#endif
