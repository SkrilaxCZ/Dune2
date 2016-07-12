#ifndef BUILDQUEUE_H
#define BUILDQUEUE_H

#include "types.h"

typedef struct BuildQueueItem
{
	struct BuildQueueItem* next;
	struct BuildQueueItem* prev;

	uint16 objectType;

	/* Credits spent.  Used for refunds, in particular starports. */
	int credits;
} BuildQueueItem;

typedef struct BuildQueue
{
	BuildQueueItem* first;
	BuildQueueItem* last;
} BuildQueue;

void BuildQueue_Init(BuildQueue* queue);
void BuildQueue_Free(BuildQueue* queue);

void BuildQueue_Add(BuildQueue* queue, uint16 objectType, int credits);
extern uint16 BuildQueue_RemoveHead(BuildQueue* queue);
extern bool BuildQueue_RemoveTail(BuildQueue* queue, uint16 objectType, int* credits);
extern bool BuildQueue_IsEmpty(const BuildQueue* queue);
extern int BuildQueue_Count(const BuildQueue* queue, uint16 objectType);

#endif
