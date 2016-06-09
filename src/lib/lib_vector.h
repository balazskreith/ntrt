#ifndef INCGUARD_MPT_LIBRARY_VECTOR_H_
#define INCGUARD_MPT_LIBRARY_VECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define VECTOR_INIT_SIZE    4
#define VECTOR_HASSPACE(v)  (((v)->items_num + 1) <= (v)->items_alloc_num)
#define VECTOR_INBOUNDS(i)	(((int) i) >= 0 && (i) < (v)->items_num)
#define VECTOR_INDEX(i)		((char *) (v)->items + ((v)->item_size * (i)))

typedef struct _vector {
	void *items;
	size_t item_size;
	size_t items_num;
	size_t items_alloc_num;
    void (*disposer)(void *);
} vector_t;

void vector_init(vector_t *, size_t, size_t, void (*disposer)(void *));
void vector_dispose(vector_t *);
void vector_copy(vector_t *, vector_t *);
void vector_insert(vector_t *, void *, size_t index);
void vector_insert_at(vector_t *, void *, size_t index);
void vector_push(vector_t *, void *);
void vector_pop(vector_t *, void *);
void vector_shift(vector_t *, void *);
void vector_unshift(vector_t *, void *);
void vector_get(vector_t *, size_t, void *);
void vector_remove(vector_t *, size_t);
void vector_transpose(vector_t *, size_t, size_t);
size_t vector_length(vector_t *);
size_t vector_size(vector_t *);
void vector_get_all(vector_t *, void *);
void vector_cmp_all(vector_t *, void *, int (*cmp_func)(const void *, const void *));
void vector_qsort(vector_t *, int (*cmp_func)(const void *, const void *));
void vector_grow(vector_t *, size_t);
void vector_swap(void *, void *, size_t);


#endif /* INCGUARD_MPT_LIBRARY_VECTOR_H_ */
