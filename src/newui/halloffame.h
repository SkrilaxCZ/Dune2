#ifndef NEWUI_HALLOFFAME_H
#define NEWUI_HALLOFFAME_H

#include <inttypes.h>
#include "../house.h"

enum
{
	MAX_RANKS = 12
};

enum HallOfFameState
{
	HALLOFFAME_PAUSE_START,
	HALLOFFAME_SHOW_RANK,
	HALLOFFAME_PAUSE_RANK,
	HALLOFFAME_SHOW_METER,
	HALLOFFAME_PAUSE_METER,
	HALLOFFAME_WAIT_FOR_INPUT,
};

ENUM_MATH_OPERATORS(HallOfFameState)

struct FadeInAux;

typedef struct HallOfFameData
{
	HallOfFameState state;

	int64_t pause_timer;

	int score;
	int64_t time;
	const char* rank;
	int rank_half_width;
	struct FadeInAux* rank_aux;

	int curr_meter_idx;
	int curr_meter_val;

	/* Meters are:
	 * spice harvested by you (g_scenario.harvestedAllied)
	 * spice harvested by enemy (g_scenario.harvestedEnemy)
	 * units destroyed by you (g_scenario.killedEnemy)
	 * units destroyed by enemy (g_scenario.killedAllied)
	 * structures destroyed by you (g_scenario.destroyedEnemy)
	 * structures destroyed by enemy (g_scenario.destroyedAllied)
	 */
	struct
	{
		int max;
		int inc;
		int width;
	} meter[6];

	bool meter_colour_dir;
	int64_t meter_colour_timer;
} HallOfFameData;

extern HallOfFameData g_hall_of_fame_state;

extern int HallOfFame_GetRank(int score);
extern const char* HallOfFame_GetRankString(int rank);
void HallOfFame_InitRank(int score, HallOfFameData* fame);
void HallOfFame_DrawBackground(HouseType houseID, bool halloffame);
void HallOfFame_DrawScoreTime(int score, int64_t ticks_played);
void HallOfFame_DrawRank(const HallOfFameData* fame);
void HallOfFame_DrawSpiceHarvested(HouseType houseID, const HallOfFameData* fame);
void HallOfFame_DrawUnitsDestroyed(HouseType houseID, const HallOfFameData* fame);
void HallOfFame_DrawBuildingsDestroyed(HouseType houseID, int scenarioID, const HallOfFameData* fame);

#endif
