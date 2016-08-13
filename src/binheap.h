#ifndef BINHEAP_H
#define BINHEAP_H

#include <inttypes.h>

struct BinHeapElem
{
	int64_t key;
};

struct BinHeap
{
	int num_elem;
	int max_elem;
	size_t elem_size;

	void* elem;
};

void BinHeap_Init(BinHeap* heap, size_t elem_size);
void BinHeap_Free(BinHeap* heap);
BinHeapElem* BinHeap_GetElem(BinHeap* heap, int i);

void* BinHeap_Push(BinHeap* heap, int64_t key);
void BinHeap_Pop(BinHeap* heap);
void* BinHeap_GetMin(BinHeap* heap);
void BinHeap_UpdateMin(BinHeap* heap);

#endif
