/** @file src/pool/unit.h %Unit pool definitions. */

#ifndef POOL_UNIT_H
#define POOL_UNIT_H

#include "types.h"

enum
{
	UNIT_INDEX_MAX = 102, /*!< The highest possible index for any Unit. */

	UNIT_INDEX_INVALID = 0xFFFF
};

struct PoolFindStruct;

extern struct Unit* g_unitFindArray[UNIT_INDEX_MAX];
extern uint16 g_unitFindCount;

extern struct Unit* Unit_Get_ByIndex(uint16 index);
extern struct Unit* Unit_Find(struct PoolFindStruct* find);

void Unit_Init();
void Unit_Recount();
extern struct Unit* Unit_Allocate(uint16 index, uint8 type, uint8 houseID);
void Unit_Free(struct Unit* u);

#endif /* POOL_UNIT_H */
