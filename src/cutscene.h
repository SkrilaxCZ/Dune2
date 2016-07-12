/** @file src/cutscene.h Introduction movie and cutscenes definitions. */

#ifndef CUTSCENE_H
#define CUTSCENE_H

#include "enumeration.h"

enum HouseAnimationType
{
	HOUSEANIMATION_INTRO = 0,
	HOUSEANIMATION_LEVEL4_HARKONNEN = 1,
	HOUSEANIMATION_LEVEL4_ATREIDES = 2,
	HOUSEANIMATION_LEVEL4_ORDOS = 3,
	HOUSEANIMATION_LEVEL8_HARKONNEN = 4,
	HOUSEANIMATION_LEVEL8_ATREIDES = 5,
	HOUSEANIMATION_LEVEL8_ORDOS = 6,
	HOUSEANIMATION_LEVEL9_HARKONNEN = 7,
	HOUSEANIMATION_LEVEL9_ATREIDES = 8,
	HOUSEANIMATION_LEVEL9_ORDOS = 9,
	HOUSEANIMATION_CREDITS = 10,

	HOUSEANIMATION_MAX = 11,
	HOUSEANIMATION_INVALID = -1
};

void Cutscene_PlayAnimation(HouseAnimationType anim);
void GameLoop_LevelEndAnimation();
void GameLoop_GameCredits(HouseType houseID);
void GameLoop_GameEndAnimation();
void GameLoop_GameIntroAnimation();

#endif
