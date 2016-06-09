/**
 * Copyright: Hamid Alipour
 */

#include "lib_vector.h"

void vector_init(vector_t *v, size_t elem_size, size_t init_size, void (*free_func)(void *))
{
	v->item_size = elem_size;
	v->items_alloc_num = (int) init_size > 0 ? init_size : VECTOR_INIT_SIZE;
	v->items_num = 0;
	v->items = malloc(elem_size * v->items_alloc_num);
	assert(v->items != NULL);
	v->disposer = free_func != NULL ? free_func : NULL;
}

void vector_dispose(vector_t *v)
{
	size_t i;

	if (v->disposer != NULL) {
		for (i = 0; i < v->items_num; i++) {
			v->disposer(VECTOR_INDEX(i));
		}
	}

	free(v->items);
}


void vector_copy(vector_t *v1, vector_t *v2)
{
	v2->items_num = v1->items_num;
	v2->items_alloc_num = v1->items_alloc_num;
	v2->item_size = v1->item_size;

	v2->items = realloc(v2->items, v2->items_alloc_num * v2->item_size);
	assert(v2->items != NULL);

	memcpy(v2->items, v1->items, v2->items_num * v2->item_size);
}

void vector_insert(vector_t *v, void *elem, size_t index)
{
	void *target;

	if ((int) index > -1) {
		if (!VECTOR_INBOUNDS(index))
			return;
		target = VECTOR_INDEX(index);
	} else {
		if (!VECTOR_HASSPACE(v))
			vector_grow(v, 0);
		target = VECTOR_INDEX(v->items_num);
		v->items_num++; /* Only grow when adding a new item not when inserting in a spec indx */
	}

	memcpy(target, elem, v->item_size);
}

void vector_insert_at(vector_t *v, void *elem, size_t index)
{
	if ((int) index < 0)
		return;

	if (!VECTOR_HASSPACE(v))
		vector_grow(v, 0);

	if (index < v->items_num)
		memmove(VECTOR_INDEX(index + 1), VECTOR_INDEX(index), (v->items_num - index) * v->item_size);

	/* 1: we are passing index so insert won't increment this 2: insert checks INBONDS... */
	v->items_num++;

	vector_insert(v, elem, index);
}

void vector_push(vector_t *v, void *elem)
{
	vector_insert(v, elem, -1);
}

void vector_pop(vector_t *v, void *elem)
{
	memcpy(elem, VECTOR_INDEX(v->items_num - 1), v->item_size);
	v->items_num--;
}

void vector_shift(vector_t *v, void *elem)
{
	memcpy(elem, v->items, v->item_size);
	memmove(VECTOR_INDEX(0), VECTOR_INDEX(1), v->items_num * v->item_size);

	v->items_num--;
}

void vector_unshift(vector_t *v, void *elem)
{
	if (!VECTOR_HASSPACE(v))
		vector_grow(v, v->items_num + 1);

	memmove(VECTOR_INDEX(1), v->items, v->items_num * v->item_size);
	memcpy(v->items, elem, v->item_size);

	v->items_num++;
}

void vector_transpose(vector_t *v, size_t index1, size_t index2)
{
	vector_swap(VECTOR_INDEX(index1), VECTOR_INDEX(index2), v->item_size);
}

void vector_grow(vector_t *v, size_t size)
{
	if (size > v->items_alloc_num)
		v->items_alloc_num = size;
	else
		v->items_alloc_num *= 2;

	v->items = realloc(v->items, v->item_size * v->items_alloc_num);
	assert(v->items != NULL);
}

void vector_get(vector_t *v, size_t index, void *elem)
{
	assert((int) index >= 0);

	if (!VECTOR_INBOUNDS(index)) {
		elem = NULL;
		return;
	}

	memcpy(elem, VECTOR_INDEX(index), v->item_size);
}

void vector_get_all(vector_t *v, void *elems)
{
	memcpy(elems, v->items, v->items_num * v->item_size);
}

void vector_remove(vector_t *v, size_t index)
{
	assert((int) index > 0);

	if (!VECTOR_INBOUNDS(index))
		return;

	memmove(VECTOR_INDEX(index), VECTOR_INDEX(index + 1), v->item_size);
	v->items_num--;
}

void vector_remove_all(vector_t *v)
{
	v->items_num = 0;
	v->items = realloc(v->items, v->items_alloc_num);
	assert(v->items != NULL);
}

size_t vector_length(vector_t *v)
{
	return v->items_num;
}

size_t vector_size(vector_t *v)
{
	return v->items_num * v->item_size;
}

void vector_cmp_all(vector_t *v, void *elem, int (*cmp_func)(const void *, const void *))
{
	size_t i;
	void *best_match = VECTOR_INDEX(0);

	for (i = 1; i < v->items_num; i++)
		if (cmp_func(VECTOR_INDEX(i), best_match) > 0)
			best_match = VECTOR_INDEX(i);

	memcpy(elem, best_match, v->item_size);
}

void vector_qsort(vector_t *v, int (*cmp_func)(const void *, const void *))
{
	qsort(v->items, v->items_num, v->item_size, cmp_func);
}

void vector_swap(void *elemp1, void *elemp2, size_t elem_size)
{
	void *tmp = malloc(elem_size);

	memcpy(tmp, elemp1, elem_size);
	memcpy(elemp1, elemp2, elem_size);
	memcpy(elemp2, tmp, elem_size);

             free(tmp); /* Thanks to gromit */
}
