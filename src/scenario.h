/** @file src/scenario.h %Scenario handling definitions. */

#ifndef SCENARIO_H
#define SCENARIO_H

#include "enumeration.h"
#include "types.h"

enum ScenarioType
{
	SCENARIO_CAMPAIGN = 0,
	SCENARIO_SKIRMISH = 1
};

/**
 * Information about reinforcements in the scenario.
 */
struct Reinforcement
{
	uint16 unitID; /*!< The Unit which is already created and ready to join the game. */
	uint16 locationID; /*!< The location where the Unit will appear. */
	uint16 timeLeft; /*!< In how many ticks the Unit will appear. */
	uint16 timeBetween; /*!< In how many ticks the Unit will appear again if repeat is set. */
	uint16 repeat; /*!< If non-zero, the Unit will appear every timeBetween ticks. */
};

/**
 * Information about the current loaded scenario.
 */
struct Scenario
{
	uint16 score; /*!< Base score. */
	uint16 winFlags; /*!< BASIC/WinFlags. */
	uint16 loseFlags; /*!< BASIC/LoseFlags. */
	uint32 mapSeed; /*!< MAP/Seed. */
	uint16 mapScale; /*!< BASIC/MapScale. 0 is 62x62, 1 is 32x32, 2 is 21x21. */
	uint16 timeOut; /*!< BASIC/TimeOut. */
	char pictureBriefing[14]; /*!< BASIC/BriefPicture. */
	char pictureWin[14]; /*!< BASIC/WinPicture. */
	char pictureLose[14]; /*!< BASIC/LosePicture. */
	uint16 killedAllied; /*!< Number of units lost by "You". */
	uint16 killedEnemy; /*!< Number of units lost by "Enemy". */
	uint16 destroyedAllied; /*!< Number of structures lost by "You". */
	uint16 destroyedEnemy; /*!< Number of structures lost by "Enemy". */
	uint16 harvestedAllied; /*!< Total amount of spice harvested by "You". */
	uint16 harvestedEnemy; /*!< Total amount of spice harvested by "Enemy". */
	Reinforcement reinforcement[16]; /*!< Reinforcement information. */
};

 struct Skirmish
{
	uint32 seed;
	Brain brain[HOUSE_MAX];
};

struct House;
struct Tile;

extern ScenarioType g_scenario_type;
extern Scenario g_scenario;
extern Skirmish g_skirmish;

void Scenario_InitTables();
void Scenario_SetCampaignAlliacnes();
bool Scenario_Load(uint16 scenarioID, uint8 houseID);
void Scenario_CentreViewport(uint8 houseID);

void Scenario_Load_Map_Bloom(uint16 packed, struct Tile* t);
void Scenario_Load_Map_Field(uint16 packed, struct Tile* t);
void Scenario_Load_Map_Special(uint16 packed, struct Tile* t);
struct House* Scenario_Create_House(HouseType houseID, Brain brain, uint16 credits, uint16 creditsQuota, uint16 unitCountMax);
void Scenario_Create_Unit(HouseType houseType, UnitType unitType, uint16 hitpoints, tile32 position, int8 orientation, UnitActionType actionType);
void Scenario_Create_Reinforcement(uint8 index, HouseType houseType, UnitType unitType, uint8 locationID, uint16 timeBetween, bool repeat);

#endif /* SCENARIO_H */
