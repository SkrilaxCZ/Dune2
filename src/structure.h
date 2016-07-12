/** @file src/structure.h %Structure handling definitions. */

#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "enumeration.h"
#include "buildqueue.h"
#include "object.h"

/** Available structure layouts. */
enum StructureLayout
{
	STRUCTURE_LAYOUT_1x1 = 0,
	STRUCTURE_LAYOUT_2x1 = 1,
	STRUCTURE_LAYOUT_1x2 = 2,
	STRUCTURE_LAYOUT_2x2 = 3,
	STRUCTURE_LAYOUT_2x3 = 4,
	STRUCTURE_LAYOUT_3x2 = 5,
	STRUCTURE_LAYOUT_3x3 = 6,

	STRUCTURE_LAYOUT_MAX = 7
};

ENUM_MATH_OPERATORS(StructureLayout)

/** States a structure can be in */
enum StructureState
{
	STRUCTURE_STATE_DETECT = -2, /*!< Used when setting state, meaning to detect which state it has by looking at other properties. */
	STRUCTURE_STATE_JUSTBUILT = -1, /*!< This shows you the building animation etc. */
	STRUCTURE_STATE_IDLE = 0, /*!< Structure is doing nothing. */
	STRUCTURE_STATE_BUSY = 1, /*!< Structure is busy (harvester in refinery, unit in repair, .. */
	STRUCTURE_STATE_READY = 2 /*!< Structure is ready and unit will be deployed soon. */
};

ENUM_MATH_OPERATORS(StructureState)

/**
 * A Structure as stored in the memory.
 */
struct Structure
{
	Object o; /*!< Common to Unit and Structures. */
	uint16 creatorHouseID; /*!< The Index of the House who created this Structure. Required in case of take-overs. */
	uint16 rotationSpriteDiff; /*!< Which sprite to show for the current rotation of Turrets etc. */
	uint16 objectType; /*!< Type of Unit/Structure we are building. */
	uint8 upgradeLevel; /*!< The current level of upgrade of the Structure. */
	uint8 upgradeTimeLeft; /*!< Time left before upgrade is complete, or 0 if no upgrade available. */
	uint16 countDown; /*!< General countdown for various of functions. */
	uint16 buildCostRemainder; /*!< The remainder of the buildCost for next tick. */
	int16 state; /*!< The state of the structure. @see StructureState. */
	uint16 hitpointsMax; /*!< Max amount of hitpoints. */

	SquadID squadID;
	BuildQueue queue;
	uint16 rallyPoint;
	int factoryOffsetY;
};

/**
 * Static information per Structure type.
 */
struct StructureInfo
{
	ObjectInfo o; /*!< Common to UnitInfo and StructureInfo. */
	uint32 enterFilter; /*!< Bitfield determining which unit is allowed to enter the structure. If bit n is set, then units of type n may enter */
	uint16 creditsStorage; /*!< How many credits this Structure can store. */
	int16 powerUsage; /*!< How much power this Structure uses (positive value) or produces (negative value). */
	uint16 layout; /*!< Layout type of Structure. */
	uint16 iconGroup; /*!< In which IconGroup the sprites of the Structure belongs. */
	uint8 animationIndex[3]; /*!< The index inside g_table_animation_structure for the Animation of the Structure. */
	uint8 buildableUnits[8]; /*!< Which units this structure can produce. */
	uint16 upgradeCampaign[3][HOUSE_MAX]; /*!< Minimum campaign for upgrades. */
};

/** X/Y pair defining a 2D size. */
struct XYSize
{
	uint16 width; /*!< Horizontal length. */
	uint16 height; /*!< Vertical length. */
};

struct House;
struct Widget;

extern const StructureInfo g_table_base_structureInfo[STRUCTURE_MAX];
extern StructureInfo g_table_structureInfo[STRUCTURE_MAX];
extern const uint16 g_table_structure_layoutTiles[STRUCTURE_LAYOUT_MAX][9];
extern const uint16 g_table_structure_layoutEdgeTiles[STRUCTURE_LAYOUT_MAX][8];
extern const uint16 g_table_structure_layoutTileCount[STRUCTURE_LAYOUT_MAX];
extern const tile32 g_table_structure_layoutTileDiff[STRUCTURE_LAYOUT_MAX];
extern const XYSize g_table_structure_layoutSize[STRUCTURE_LAYOUT_MAX];
extern const int16 g_table_structure_layoutTilesAround[STRUCTURE_LAYOUT_MAX][16];

extern Structure* g_structureActive;
extern uint16 g_structureActivePosition;
extern uint16 g_structureActiveType;

extern uint16 g_structureIndex;

void GameLoop_Structure();
uint8 Structure_StringToType(const char* name);
Structure* Structure_Create(uint16 index, uint8 typeID, uint8 houseID, uint16 position);
bool Structure_Place(Structure* s, uint16 position, HouseType houseID);
void Structure_CalculateHitpointsMax(struct House* h);
void Structure_SetState(Structure* s, int16 animation);
bool Structure_SupportsRallyPoints(StructureType s);
void Structure_SetRallyPoint(Structure* s, uint16 packed);
Structure* Structure_Get_ByPackedTile(uint16 packed);
uint32 Structure_GetStructuresBuilt(struct House* h);
int16 Structure_IsValidBuildLandscape(uint16 position, StructureType type);
int16 Structure_IsValidBuildLocation(uint16 position, StructureType type);
void Structure_ActivateSpecial(Structure* s);
void Structure_RemoveFog(Structure* s);
bool Structure_Damage(Structure* s, uint16 damage, uint16 range);
bool Structure_IsUpgradable(Structure* s);
bool Structure_ConnectWall(uint16 position, bool recurse);
struct Unit* Structure_GetLinkedUnit(Structure* s);
void Structure_UntargetMe(Structure* s);
uint16 Structure_FindFreePosition(Structure* s, bool checkForSpice);
void Structure_Remove(Structure* s);
void Structure_CancelBuild(Structure* s);
bool Structure_BuildObject(Structure* s, uint16 objectType);
bool Structure_SetUpgradingState(Structure* s, int8 value, struct Widget* w);
bool Structure_SetRepairingState(Structure* s, int8 value, struct Widget* w);
void Structure_UpdateMap(Structure* s);
int Structure_GetAvailable(const Structure* s, int i);
void Structure_InitFactoryItems(const Structure* s);
void Structure_Starport_Restock(UnitType type);
void Structure_HouseUnderAttack(uint8 houseID);

#endif /* STRUCTURE_H */
