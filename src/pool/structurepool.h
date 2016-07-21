/** @file src/pool/structure.h %Structure pool definitions. */

#ifndef POOL_STRUCTURE_H
#define POOL_STRUCTURE_H

#include "types.h"

const int STRUCTURE_INDEX_MAX_SOFT = 1021; /*!< The highest possible index for normal Structure. */
const int STRUCTURE_INDEX_MAX_HARD = 1024; /*!< The highest possible index for any Structure. */

const int STRUCTURE_INDEX_WALL     = 1021; /*!< All walls are are put under index 1021. */
const int STRUCTURE_INDEX_SLAB_2x2 = 1022; /*!< All 2x2 slabs are put under index 1022. */
const int STRUCTURE_INDEX_SLAB_1x1 = 1023; /*!< All 1x1 slabs are put under index 1023. */
const int STRUCTURE_INDEX_INVALID  = 0xFFFF;

struct PoolFindStruct;

extern struct Structure* Structure_Get_ByIndex(uint16 index);
extern struct Structure* Structure_Find(struct PoolFindStruct* find);

void Structure_Init();
void Structure_Recount();
extern struct Structure* Structure_Allocate(uint16 index, uint8 type);
void Structure_Free(struct Structure* s);

#endif /* POOL_STRUCTURE_H */
