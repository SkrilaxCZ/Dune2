/** @file src/pool/structure.h %Structure pool definitions. */

#ifndef POOL_STRUCTURE_H
#define POOL_STRUCTURE_H

#include "types.h"

enum
{
	STRUCTURE_INDEX_MAX_SOFT = 79, /*!< The highest possible index for normal Structure. */
	STRUCTURE_INDEX_MAX_HARD = 82, /*!< The highest possible index for any Structure. */

	STRUCTURE_INDEX_WALL = 79, /*!< All walls are are put under index 79. */
	STRUCTURE_INDEX_SLAB_2x2 = 80, /*!< All 2x2 slabs are put under index 80. */
	STRUCTURE_INDEX_SLAB_1x1 = 81, /*!< All 1x1 slabs are put under index 81. */

	STRUCTURE_INDEX_INVALID = 0xFFFF
};

struct PoolFindStruct;

extern struct Structure* Structure_Get_ByIndex(uint16 index);
extern struct Structure* Structure_Find(struct PoolFindStruct* find);

void Structure_Init();
void Structure_Recount();
extern struct Structure* Structure_Allocate(uint16 index, uint8 type);
void Structure_Free(struct Structure* s);

#endif /* POOL_STRUCTURE_H */
