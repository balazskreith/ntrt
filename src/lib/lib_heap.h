/*
 * lib_heap.h
 *
 *  Created on: Jul 15, 2014
 *      Author: balazs
 */

#ifndef INCGUARD_LIB_HEAP_H_
#define INCGUARD_LIB_HEAP_H_
#include "lib_descs.h"

// WARNING: Requires C99 compatible compiler

//typedef int heap_item_t;
typedef packet_t* HeapItem;

struct Heap
{
	unsigned int size; // Size of the allocated memory (in number of items)
	unsigned int count; // Count of the elements in the heap
	HeapItem *data; // Array with the elements
};

void _heap_init(struct Heap *restrict h);
void _heap_push(struct Heap *restrict h, HeapItem value);
void _heap_pop(struct Heap *restrict h);

// Returns the biggest element in the heap
//Returns the smallest sequence element in the heap
#define heap_front(h) (*(h)->data)

// Frees the allocated memory
#define heap_term(h) (free((h)->data))

void _heapify(HeapItem data[restrict], unsigned int count);

#endif /* INCGUARD_LIB_HEAP_H_ */
