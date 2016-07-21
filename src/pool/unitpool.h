/** @file src/pool/unit.h %Unit pool definitions. */

#ifndef POOL_UNIT_H
#define POOL_UNIT_H

#include "types.h"

const int UNIT_INDEX_AIR_START        = 0;
const int UNIT_INDEX_AIR_END          = 39;
const int UNIT_INDEX_FRIGATE_START    = 40;
const int UNIT_INDEX_FRIGATE_END      = 47;
const int UNIT_INDEX_PROJECTILE_START = 48;
const int UNIT_INDEX_PROJECTILE_END   = 79;
const int UNIT_INDEX_SANDWORM_START   = 80;
const int UNIT_INDEX_SANDWORM_END     = 90;
const int UNIT_INDEX_SABOTEUR_START   = 91;
const int UNIT_INDEX_SABOTEUR_END     = 98;
const int UNIT_INDEX_NORMAL_START     = 99;
const int UNIT_INDEX_NORMAL_END       = 1023;
const int UNIT_INDEX_MAX              = 1024; /*!< The highest possible index for any Unit. */
const int UNIT_INDEX_INVALID          = 0xFFFF;

struct PoolFindStruct;

extern struct Unit* g_unitFindArray[UNIT_INDEX_MAX];
extern uint16 g_unitFindCount;

extern struct Unit* Unit_Get_ByIndex(uint16 index);
extern struct Unit* Unit_Find(struct PoolFindStruct* find);

void Unit_Init();
void Unit_Recount();
struct Unit* Unit_Allocate(uint16 index, uint8 type, uint8 houseID);
void Unit_Free(struct Unit* u);

#endif /* POOL_UNIT_H */
