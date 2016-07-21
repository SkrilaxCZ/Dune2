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
 *
 * LGPL License
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef SOUND_ADLIB_H
#define SOUND_ADLIB_H

#include "types.h"

#include <allegro5/allegro.h>
#include <cstdint>
#include <vector>

class AdLibDriver;

/**
 * AdLib implementation of the sound output device.
 *
 * It uses a special sound file format special to
 * Dune II, Kyrandia 1 and 2. While Dune II and
 * Kyrandia 1 are using exact the same format, the
 * one of Kyrandia 2 slightly differs.
 *
 * See AdLibDriver for more information.
 * @see AdLibDriver
 */
class SoundAdLibPC
{
public:

	SoundAdLibPC(ALLEGRO_FILE* rwop, int freq, bool bMAME = true);
	~SoundAdLibPC();

	static void callback(void*, uint8*, int);

	std::vector<int> getSubsongs();

	bool init();
	void process();

	void playTrack(uint8 track);
	void haltTrack();

	bool isPlaying() const;

	void playSoundEffect(uint8 track);

	void beginFadeOut();

private:
	void internalLoadFile(ALLEGRO_FILE* rwop);

	void play(uint8 track);

	AdLibDriver* _driver;

	uint8 _trackEntries[500];
	uint8* _soundDataPtr;
	int _sfxPlayingSound;

	uint8 _sfxPriority;
	uint8 _sfxFourthByteOfSong;

	int _numSoundTriggers;
	const int* _soundTriggers;

	unsigned char getsampsize();

	int m_channels;
	int m_freq;
	uint16 m_format;

	bool bJustStartedPlaying;
};

#endif
