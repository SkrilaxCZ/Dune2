/** @file src/scenario.c %Scenario handling routines. */

#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <cassert>
#include <cctype>
#include <cstring>
#include "types.h"
#include "os/math.h"

#include "scenario.h"
#include "file.h"
#include "house.h"
#include "ini.h"
#include "map.h"
#include "opendune.h"
#include "pool/housepool.h"
#include "pool/pool.h"
#include "pool/structurepool.h"
#include "pool/unitpool.h"
#include "sprites.h"
#include "structure.h"
#include "team.h"
#include "timer/timer.h"
#include "tools/coord.h"
#include "unit.h"
#include "gui/gui.h"

ScenarioType g_scenario_type;
Scenario g_scenario;
Skirmish g_skirmish;

static void* s_scenarioBuffer = NULL;

/*--------------------------------------------------------------*/

static uint16 ObjectInfo_FlagsToUint16(const ObjectInfo* oi)
{
	uint16 flags = 0;

	/* Note: Read/write flags as they appear in the original executable. */
	if (oi->flags.hasShadow)
		flags |= 0x0001;
	if (oi->flags.factory)
		flags |= 0x0002;
	/* 0x0004 unused. */
	if (oi->flags.notOnConcrete)
		flags |= 0x0008;
	if (oi->flags.busyStateIsIncoming)
		flags |= 0x0010;
	if (oi->flags.blurTile)
		flags |= 0x0020;
	if (oi->flags.hasTurret)
		flags |= 0x0040;
	if (oi->flags.conquerable)
		flags |= 0x0080;
	if (oi->flags.canBePickedUp)
		flags |= 0x0100;
	if (oi->flags.noMessageOnDeath)
		flags |= 0x0200;
	if (oi->flags.tabSelectable)
		flags |= 0x0400;
	if (oi->flags.scriptNoSlowdown)
		flags |= 0x0800;
	if (oi->flags.targetAir)
		flags |= 0x1000;
	if (oi->flags.priority)
		flags |= 0x2000;

	return flags;
}

static uint16 UnitInfo_FlagsToUint16(const UnitInfo* ui)
{
	uint16 flags = 0;

	/* Note: Read/write flags as they appear in the original executable. */
	/* 0x0001 unused. */
	if (ui->flags.isBullet)
		flags |= 0x0002;
	if (ui->flags.explodeOnDeath)
		flags |= 0x0004;
	if (ui->flags.sonicProtection)
		flags |= 0x0008;
	if (ui->flags.canWobble)
		flags |= 0x0010;
	if (ui->flags.isTracked)
		flags |= 0x0020;
	if (ui->flags.isGroundUnit)
		flags |= 0x0040;
	if (ui->flags.mustStayInMap)
		flags |= 0x0080;
	/* 0x0100 unused. */
	/* 0x0200 unused. */
	if (ui->flags.firesTwice)
		flags |= 0x0400;
	if (ui->flags.impactOnSand)
		flags |= 0x0800;
	if (ui->flags.isNotDeviatable)
		flags |= 0x1000;
	if (ui->flags.hasAnimationSet)
		flags |= 0x2000;
	if (ui->flags.notAccurate)
		flags |= 0x4000;
	if (ui->flags.isNormalUnit)
		flags |= 0x8000;

	return flags;
}

/*--------------------------------------------------------------*/

static void Scenario_Load_General()
{
	g_scenario.winFlags = Ini_GetInteger("BASIC", "WinFlags", 0, (char*)s_scenarioBuffer);
	g_scenario.loseFlags = Ini_GetInteger("BASIC", "LoseFlags", 0, (char*)s_scenarioBuffer);
	g_scenario.mapSeed = Ini_GetInteger("MAP", "Seed", 0, (char*)s_scenarioBuffer);
	g_scenario.timeOut = Ini_GetInteger("BASIC", "TimeOut", 0, (char*)s_scenarioBuffer);
	g_viewportPosition = Ini_GetInteger("BASIC", "TacticalPos", g_viewportPosition, (char*)s_scenarioBuffer);
	g_selectionRectanglePosition = Ini_GetInteger("BASIC", "CursorPos", g_selectionRectanglePosition, (char*)s_scenarioBuffer);
	g_scenario.mapScale = Ini_GetInteger("BASIC", "MapScale", 0, (char*)s_scenarioBuffer);
	g_techLevel = Ini_GetInteger("BASIC", "TechLevel", 0, (char*)s_scenarioBuffer);

	Ini_GetString("BASIC", "BriefPicture", "HARVEST.WSA", g_scenario.pictureBriefing, 14, (char*)s_scenarioBuffer);
	Ini_GetString("BASIC", "WinPicture", "WIN1.WSA", g_scenario.pictureWin, 14, (char*)s_scenarioBuffer);
	Ini_GetString("BASIC", "LosePicture", "LOSTBILD.WSA", g_scenario.pictureLose, 14, (char*)s_scenarioBuffer);

	g_selectionPosition = g_selectionRectanglePosition;
	Map_MoveDirection(0, 0);
}

static void Scenario_Load_House(uint8 houseID)
{
	const char* houseName = g_table_houseInfo[houseID].name;
	const char* houseType;
	char buf[128];
	char* b;

	/* Get the type of the House (CPU / Human) */
	Ini_GetString(houseName, "Brain", "NONE", buf, 127, (char*)s_scenarioBuffer);
	for (b = buf; *b != '\0'; b++)
		if (*b >= 'a' && *b <= 'z')
			*b += 'A' - 'a';

	Brain brain = BRAIN_NONE;

	if (!strcmp(buf, "HUMAN"))
		brain = BRAIN_HUMAN;
	else if (!strcmp(buf, "CPU_ALLY"))
		brain = BRAIN_CPU_ALLY;
	else if (!strcmp(buf, "CPU_ENEMY"))
		brain = BRAIN_CPU_ENEMY;

	if (brain == BRAIN_NONE)
		return;

	uint16 credits;
	uint16 creditsQuota;
	uint16 unitCountMax;

	credits = Ini_GetInteger(houseName, "Credits", 0, (char*)s_scenarioBuffer);
	creditsQuota = Ini_GetInteger(houseName, "Quota", 0, (char*)s_scenarioBuffer);
	unitCountMax = Ini_GetInteger(houseName, "MaxUnit", 39, (char*)s_scenarioBuffer);
	Scenario_Create_House((HouseType)houseID, brain, credits, creditsQuota, unitCountMax);
}

House* Scenario_Create_House(HouseType houseID, Brain brain, uint16 credits, uint16 creditsQuota, uint16 unitCountMax)
{
	House* h;

	/* Create the house */
	h = House_Allocate(houseID);

	h->credits = credits;
	h->creditsQuota = creditsQuota;
	h->unitCountMax = unitCountMax;
	h->brain = brain;

	/* For 'Brain = Human' we have to set a few additional things */
	if (brain != BRAIN_HUMAN)
		return h;

	h->flags.human = true;
	g_playerHouseID = houseID;
	g_playerHouse = h;
	g_playerCreditsNoSilo = h->credits;
	return h;
}

static void Scenario_Load_Houses()
{
	House* h;
	uint8 houseID;

	for (houseID = 0; houseID < HOUSE_MAX; houseID++)
		Scenario_Load_House(houseID);

	h = g_playerHouse;
	/* In case there was no unitCountMax in the scenario, calculate
	 *  it based on values used for the AI controlled houses. */
	if (h->unitCountMax == 0)
	{
		PoolFindStruct find;
		uint8 max;
		House* h2;

		find.houseID = HOUSE_INVALID;
		find.index = 0xFFFF;
		find.type = 0xFFFF;

		max = 80;
		while ((h2 = House_Find(&find)) != NULL)
		{
			/* Skip the human controlled house */
			if (h2->flags.human)
				continue;
			max -= h2->unitCountMax;
		}

		h->unitCountMax = max;
	}
#if DEBUG
	h->unitCountMax = 0xFFFF;
#endif
}

static void Scenario_Load_Unit(const char* key, char* settings)
{
	uint8 houseType, unitType, actionType;
	int8 orientation;
	uint16 hitpoints;
	tile32 position;
	char* split;

	UNUSED(key);

	/* The value should have 6 values separated by a ',' */
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* First value is the House type */
	houseType = House_StringToType(settings);
	if (houseType == HOUSE_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Second value is the Unit type */
	unitType = Unit_StringToType(settings);
	if (unitType == UNIT_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Third value is the Hitpoints in percent (in base 256) */
	hitpoints = atoi(settings);

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Fourth value is the position on the map */
	position = Tile_UnpackTile(atoi(settings));

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Fifth value is orientation */
	orientation = (int8)((uint8)atoi(settings));

	/* Sixth value is the current state of the unit */
	settings = split + 1;
	actionType = Unit_ActionStringToType(settings);
	if (actionType == ACTION_INVALID)
		return;

	Scenario_Create_Unit((HouseType)houseType, (UnitType)unitType, hitpoints, position, orientation, (UnitActionType)actionType);
}

void Scenario_Create_Unit(HouseType houseType, UnitType unitType, uint16 hitpoints, tile32 position, int8 orientation, UnitActionType actionType)
{
	Unit* u;

	u = Unit_Allocate(UNIT_INDEX_INVALID, unitType, houseType);
	if (u == NULL)
		return;
	u->o.flags.s.byScenario = true;

	u->o.hitpoints = hitpoints * g_table_unitInfo[unitType].o.hitpoints / 256;
	u->o.position = position;
	u->orientation[0].current = orientation;
	u->actionID = actionType;
	u->nextActionID = ACTION_INVALID;

	/* In case the above function failed, don't add the unit */
	if (!Map_IsValidPosition(Tile_PackTile(u->o.position)))
	{
		Unit_Free(u);
		return;
	}

	/* XXX -- There is no way this is ever possible, as the beingBuilt flag is unset by Unit_Allocate() */
	if (!u->o.flags.s.isNotOnMap)
		Unit_SetAction(u, (ActionType)u->actionID);

	u->o.seenByHouses = 0x00;

	Unit_HouseUnitCount_Add(u, u->o.houseID);

	Unit_SetOrientation(u, u->orientation[0].current, true, 0);
	Unit_SetOrientation(u, u->orientation[0].current, true, 1);
	Unit_SetSpeed(u, 0);
}

static void Scenario_Load_Structure(const char* key, char* settings)
{
	uint8 index, houseType, structureType;
	uint16 hitpoints, position;
	char* split;

	/* 'GEN' marked keys are Slabs and Walls, where the number following indicates the position on the map */
	if (strnicmp(key, "GEN", 3) == 0)
	{
		/* Position on the map is in the key */
		position = atoi(key + 3);

		/* The value should have two values separated by a ',' */
		split = strchr(settings, ',');
		if (split == NULL)
			return;
		*split = '\0';
		/* First value is the House type */
		houseType = House_StringToType(settings);
		if (houseType == HOUSE_INVALID)
			return;

		/* Second value is the Structure type */
		settings = split + 1;
		structureType = Structure_StringToType(settings);
		if (structureType == STRUCTURE_INVALID)
			return;

		Structure_Create(STRUCTURE_INDEX_INVALID, structureType, houseType, position);
		return;
	}

	/* The key should start with 'ID', followed by the index */
	index = atoi(key + 2);

	/* The value should have four values separated by a ',' */
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* First value is the House type */
	houseType = House_StringToType(settings);
	if (houseType == HOUSE_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Second value is the Structure type */
	structureType = Structure_StringToType(settings);
	if (structureType == STRUCTURE_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	hitpoints = 256;

	/* Fourth value is the position of the structure */
	settings = split + 1;
	position = atoi(settings);

	/* Ensure nothing is already on the tile */
	/* XXX -- DUNE2 BUG? -- This only checks the top-left corner? Not really a safety, is it? */
	if (Structure_Get_ByPackedTile(position) != NULL)
		return;

	{
		Structure* s = Structure_Create(index, structureType, houseType, position);
		if (s == NULL)
			return;

		s->o.hitpoints = hitpoints * g_table_structureInfo[s->o.type].o.hitpoints / 256;
		s->o.flags.s.degrades = false;
		s->state = STRUCTURE_STATE_IDLE;
	}
}

static void Scenario_Load_Map(const char* key, char* settings)
{
	Tile* t;
	uint16 packed;
	uint16 value;
	char* s;
	char posY[3];

	if (*key != 'C')
		return;

	memcpy(posY, key + 4, 2);
	posY[2] = '\0';

	packed = Tile_PackXY(atoi(posY), atoi(key + 6)) & 0xFFF;
	t = &g_map[packed];

	s = strtok(settings, ",\r\n");
	value = atoi(s);
	t->houseID = value & 0x07;
	t->isUnveiled = (value & 0x08) != 0 ? true : false;
	t->hasUnit = (value & 0x10) != 0 ? true : false;
	t->hasStructure = (value & 0x20) != 0 ? true : false;
	t->hasAnimation = (value & 0x40) != 0 ? true : false;
	t->hasExplosion = (value & 0x80) != 0 ? true : false;

	s = strtok(NULL, ",\r\n");
	t->groundSpriteID = atoi(s) & 0x01FF;
	if (g_mapSpriteID[packed] != t->groundSpriteID)
		g_mapSpriteID[packed] |= 0x8000;

	if (!t->isUnveiled)
		t->overlaySpriteID = g_veiledSpriteID;
}

void Scenario_Load_Map_Bloom(uint16 packed, Tile* t)
{
	t->groundSpriteID = g_bloomSpriteID;
	g_mapSpriteID[packed] |= 0x8000;
}

void Scenario_Load_Map_Field(uint16 packed, Tile* t)
{
	Map_Bloom_ExplodeSpice(packed, HOUSE_INVALID);

	/* Show where a field started in the preview mode by making it an odd looking sprite */
	if (g_debugScenario)
	{
		t->groundSpriteID = 0x01FF;
	}
}

void Scenario_Load_Map_Special(uint16 packed, Tile* t)
{
	t->groundSpriteID = g_bloomSpriteID + 1;
	g_mapSpriteID[packed] |= 0x8000;
}

static void Scenario_Load_Reinforcement(const char* key, char* settings)
{
	uint8 index, houseType, unitType, locationID;
	uint16 timeBetween;
	bool repeat;
	char* split;

	/* There is a bug here: keys beginning with ';' are not ignored
	 * but read as index 0.  This means that up to 10 units are
	 * created, and the final unit is delivered.  The other indices
	 * are unusable.  This only occurs in SCENA002,SCENA003,SCENA004.
	 */
	index = atoi(key);

	/* The value should have 4 values separated by a ',' */
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* First value is the House type */
	houseType = House_StringToType(settings);
	if (houseType == HOUSE_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Second value is the Unit type */
	unitType = Unit_StringToType(settings);
	if (unitType == UNIT_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Third value is the location of the reinforcement */
	if (stricmp(settings, "NORTH") == 0)
		locationID = 0;
	else if (stricmp(settings, "EAST") == 0)
		locationID = 1;
	else if (stricmp(settings, "SOUTH") == 0)
		locationID = 2;
	else if (stricmp(settings, "WEST") == 0)
		locationID = 3;
	else if (stricmp(settings, "AIR") == 0)
		locationID = 4;
	else if (stricmp(settings, "VISIBLE") == 0)
		locationID = 5;
	else if (stricmp(settings, "ENEMYBASE") == 0)
		locationID = 6;
	else if (stricmp(settings, "HOMEBASE") == 0)
		locationID = 7;
	else
		return;

	/* Fourth value is the time between reinforcement */
	settings = split + 1;
	timeBetween = atoi(settings) * 6 + 1;
	repeat = (settings[strlen(settings) - 1] == '+') ? true : false;

	Scenario_Create_Reinforcement(index, (HouseType)houseType, (UnitType)unitType, locationID, timeBetween, repeat);
}

void
Scenario_Create_Reinforcement(uint8 index, HouseType houseType,
                              UnitType unitType, uint8 locationID, uint16 timeBetween, bool repeat)
{
	tile32 position;
	Unit* u;

	position.x = 0xFFFF;
	position.y = 0xFFFF;
	u = Unit_Create(UNIT_INDEX_INVALID, unitType, houseType, position, 0);
	if (u == NULL)
		return;

	g_scenario.reinforcement[index].unitID = u->o.index;
	g_scenario.reinforcement[index].locationID = locationID;
	g_scenario.reinforcement[index].timeLeft = timeBetween;
	g_scenario.reinforcement[index].timeBetween = timeBetween;
	g_scenario.reinforcement[index].repeat = repeat ? 1 : 0;
}

static void Scenario_Load_Team(const char* key, char* settings)
{
	uint8 houseType, teamActionType, movementType;
	uint16 minMembers, maxMembers;
	char* split;

	UNUSED(key);

	/* The value should have 5 values separated by a ',' */
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* First value is the House type */
	houseType = House_StringToType(settings);
	if (houseType == HOUSE_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Second value is the teamAction type */
	teamActionType = Team_ActionStringToType(settings);
	if (teamActionType == TEAM_ACTION_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Third value is the movement type */
	movementType = Unit_MovementStringToType(settings);
	if (movementType == MOVEMENT_INVALID)
		return;

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Fourth value is minimum amount of members in team */
	minMembers = atoi(settings);

	/* Find the next value in the ',' separated list */
	settings = split + 1;
	split = strchr(settings, ',');
	if (split == NULL)
		return;
	*split = '\0';

	/* Fifth value is maximum amount of members in team */
	maxMembers = atoi(settings);

	Team_Create(houseType, teamActionType, movementType, minMembers, maxMembers);
}

/**
 * Initialize a unit count of the starport.
 * @param key Unit type to set.
 * @param settings Count to set.
 */
static void Scenario_Load_Choam(const char* key, char* settings)
{
	uint8 unitType;

	unitType = Unit_StringToType(key);
	if (unitType == UNIT_INVALID)
		return;

	g_starportAvailable[unitType] = atoi(settings);
}

static void Scenario_Load_MapParts(const char* key, void (*ptr)(uint16 packed, Tile* t))
{
	char* s;
	char buf[128];

	Ini_GetString("MAP", key, '\0', buf, 127, (char*)s_scenarioBuffer);

	s = strtok(buf, ",\r\n");
	while (s != NULL)
	{
		uint16 packed;
		Tile* t;

		packed = atoi(s);
		t = &g_map[packed];

		(*ptr)(packed, t);

		s = strtok(NULL, ",\r\n");
	}
}

static void Scenario_Load_Chunk(const char* category, void (*ptr)(const char* key, char* settings))
{
	char* buffer = (char*)g_readBuffer;

	Ini_GetString(category, NULL, NULL, (char*)g_readBuffer, g_readBufferSize, (char*)s_scenarioBuffer);
	while (true)
	{
		char buf[127];

		if (*buffer == '\0')
			break;

		Ini_GetString(category, buffer, NULL, buf, 127, (char*)s_scenarioBuffer);

		(*ptr)(buffer, buf);
		buffer += strlen(buffer) + 1;
	}
}

void Scenario_InitTables()
{
	memcpy_s(g_table_houseInfo, sizeof(g_table_houseInfo), g_table_base_houseInfo, sizeof(g_table_base_houseInfo));
	memcpy_s(g_table_structureInfo, sizeof(g_table_structureInfo), g_table_base_structureInfo, sizeof(g_table_base_structureInfo));
	memcpy_s(g_table_unitInfo, sizeof(g_table_unitInfo), g_table_base_unitInfo, sizeof(g_table_base_unitInfo));

/*#ifdef DEBUG
	for (int i = 0; i < STRUCTURE_MAX; i++)
	{
		g_table_structureInfo[i].o.buildTime = 1;
		g_table_structureInfo[i].o.buildCredits = 1;
	}

	for (int i = 0; i < UNIT_MAX; i++)
	{
		g_table_unitInfo[i].o.buildTime = 1;
		g_table_unitInfo[i].o.buildCredits = 1;
	}
#endif*/
}

void Scenario_CentreViewport(uint8 houseID)
{
	PoolFindStruct find;

	find.houseID = houseID;
	find.type = STRUCTURE_CONSTRUCTION_YARD;
	find.index = 0xFFFF;

	Structure* s = Structure_Find(&find);
	if (s != NULL)
	{
		Map_CentreViewport((s->o.position.x >> 4) + TILE_SIZE, (s->o.position.y >> 4) + TILE_SIZE);
		return;
	}

	/* ENHANCEMENT -- centre on MCV. */
	find.houseID = houseID;
	find.type = UNIT_MCV;
	find.index = 0xFFFF;

	Unit* u = Unit_Find(&find);
	if (u != NULL)
		Map_CentreViewport(u->o.position.x >> 4, u->o.position.y >> 4);
}

bool Scenario_Load(uint16 scenarioID, uint8 houseID)
{
	char filename[14];

	if (houseID >= HOUSE_MAX)
		return false;

	/* Load scenario file */
	snprintf(filename, sizeof(filename), "SCEN%c%03d.INI", g_table_houseInfo[houseID].name[0], scenarioID);
	return Scenario_Load(filename, SEARCHDIR_CAMPAIGN_DIR);
}

bool Scenario_Load(const char* filename, SearchDirectory directory)
{
	if (!File_Exists_Ex(directory, filename))
		return false;

	s_scenarioBuffer = File_ReadWholeFile_Ex(directory, filename);

	memset(&g_scenario, 0, sizeof(Scenario));

	Scenario_Load_General();
	Sprites_LoadTiles();
	Map_CreateLandscape(g_scenario.mapSeed);

	for (int i = 0; i < 16; i++)
		g_scenario.reinforcement[i].unitID = UNIT_INDEX_INVALID;

	Scenario_Load_Houses();

	Scenario_Load_Chunk("UNITS", &Scenario_Load_Unit);
	Scenario_Load_Chunk("STRUCTURES", &Scenario_Load_Structure);
	Scenario_Load_Chunk("MAP", &Scenario_Load_Map);
	Scenario_Load_Chunk("REINFORCEMENTS", &Scenario_Load_Reinforcement);
	Scenario_Load_Chunk("TEAMS", &Scenario_Load_Team);
	Scenario_Load_Chunk("CHOAM", &Scenario_Load_Choam);

	Scenario_Load_MapParts("Bloom", Scenario_Load_Map_Bloom);
	Scenario_Load_MapParts("Field", Scenario_Load_Map_Field);
	Scenario_Load_MapParts("Special", Scenario_Load_Map_Special);

	Scenario_CentreViewport(g_playerHouseID);
	g_tickScenarioStart = g_timerGame;

	free(s_scenarioBuffer);
	s_scenarioBuffer = NULL;
	return true;
}
