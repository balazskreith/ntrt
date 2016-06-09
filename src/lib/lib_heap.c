#include <unistd.h>
#include <stdlib.h>

#include "lib_heap.h"

//#define CMP(a, b) ((a) >= (b))
#define HEAP_CMP(a, b) ((a)->seq_num <= (b)->seq_num)

static const unsigned int base_size = 4;

// Prepares the heap for use
void _heap_init(struct Heap *restrict h)
{
	*h = (struct Heap){
		.size = base_size,
		.count = 0,
		.data = malloc(sizeof(HeapItem) * base_size)
	};
	if (!h->data) _exit(1); // Exit if the memory allocation fails
}

// Inserts element to the heap
void _heap_push(struct Heap *restrict h, HeapItem value)
{
	unsigned int index, parent;

	// Resize the heap if it is too small to hold all the data
	if (h->count == h->size)
	{
		h->size <<= 1;
		h->data = realloc(h->data, sizeof(HeapItem) * h->size);
		if (!h->data) _exit(1); // Exit if the memory allocation fails
	}

	// Find out where to put the element and put it
	for(index = h->count++; index; index = parent)
	{
		parent = (index - 1) >> 1;
		if HEAP_CMP(h->data[parent], value) break;
		h->data[index] = h->data[parent];
	}
	h->data[index] = value;
}

// Removes the biggest element from the heap
void _heap_pop(struct Heap *restrict h)
{
	unsigned int index, swap, other;

	// Remove the biggest element
	HeapItem temp = h->data[--h->count];

	// Resize the heap if it's consuming too much memory
	if ((h->count <= (h->size >> 2)) && (h->size > base_size))
	{
		h->size >>= 1;
		h->data = realloc(h->data, sizeof(HeapItem) * h->size);
		if (!h->data) _exit(1); // Exit if the memory allocation fails
	}

	// Reorder the elements
	for(index = 0; 1; index = swap)
	{
		// Find the child to swap with
		swap = (index << 1) + 1;
		if (swap >= h->count) break; // If there are no children, the heap is reordered
		other = swap + 1;
		if ((other < h->count) && HEAP_CMP(h->data[other], h->data[swap])) swap = other;
		if HEAP_CMP(temp, h->data[swap]) break; // If the bigger child is less than or equal to its parent, the heap is reordered

		h->data[index] = h->data[swap];
	}
	h->data[index] = temp;
}

// Heapifies a non-empty array
void _heapify(HeapItem data[restrict], unsigned int count)
{
	unsigned int item, index, swap, other;
	HeapItem temp;

	// Move every non-leaf element to the right position in its subtree
	item = (count >> 1) - 1;
	while (1)
	{
		// Find the position of the current element in its subtree
		temp = data[item];
		for(index = item; 1; index = swap)
		{
			// Find the child to swap with
			swap = (index << 1) + 1;
			if (swap >= count) break; // If there are no children, the current element is positioned
			other = swap + 1;
			if ((other < count) && HEAP_CMP(data[other], data[swap])) swap = other;
			if HEAP_CMP(temp, data[swap]) break; // If the bigger child is smaller than or equal to the parent, the heap is reordered

			data[index] = data[swap];
		}
		if (index != item) data[index] = temp;

		if (!item) return;
		--item;
	}
}
