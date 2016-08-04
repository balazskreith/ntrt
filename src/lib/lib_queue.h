#ifndef INCGUARD_NTRT_LIBRARY_QUEUE_H_
#define INCGUARD_NTRT_LIBRARY_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "lib_funcs.h"
#include "lib_makers.h"
#include "lib_descs.h"
#include "lib_puffers.h"

typedef struct _queue {
  dlist_t* head;
  dlist_t* tail;
  void (*disposer)(void *);
  int32_t counter;
} queue_t;

queue_t* make_queue();
void queue_test(void);
void queue_dtor(queue_t* target);
void print_queue_status(queue_t* queue);
void queue_push_head(queue_t* this, ptr_t data);
ptr_t queue_pop_head(queue_t* this);
ptr_t queue_peek_head(queue_t* this);
void queue_push_tail(queue_t* this, ptr_t data);
ptr_t queue_pop_tail(queue_t* this);
ptr_t queue_peek_tail(queue_t* this);
bool_t queue_is_empty(queue_t* this);

#endif /* INCGUARD_NTRT_LIBRARY_VECTOR_H_ */
