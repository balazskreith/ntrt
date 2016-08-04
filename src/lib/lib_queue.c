#include "lib_queue.h"
#include "lib_puffers.h"
#include "inc_mtime.h"

void queue_test(void)
{
  queue_t* queue;
  int32_t values[] = {1,2,3,4,5,6,7,8,9,10};
  int32_t i;

  queue = make_queue(free);

  //FIFO test
  for(i = 0; i < 10; ++i){
    queue_push_tail(queue, &values[i]);
  }
  print_queue_status(queue);
  for(i = 0; i < 10; ++i){
    int32_t* data;
    data = queue_pop_head(queue);
    sysio->print_stdout("%d,", *data);
  }
  sysio->print_stdout("\n");
  print_queue_status(queue);

  for(i = 0; i < 10; ++i){
    queue_push_head(queue, &values[i]);
  }
  print_queue_status(queue);
  for(i = 0; i < 10; ++i){
    int32_t* data;
    data = queue_pop_tail(queue);
    sysio->print_stdout("%d,", *data);
  }
  sysio->print_stdout("\n");
  print_queue_status(queue);

  //LIFO TEST
  for(i = 0; i < 10; ++i){
    queue_push_head(queue, &values[i]);
  }
  print_queue_status(queue);
  for(i = 0; i < 10; ++i){
    int32_t* data;
    data = queue_pop_head(queue);
    sysio->print_stdout("%d,", *data);
  }
  sysio->print_stdout("\n");
  print_queue_status(queue);

  for(i = 0; i < 10; ++i){
    queue_push_tail(queue, &values[i]);
  }
  print_queue_status(queue);
  for(i = 0; i < 10; ++i){
    int32_t* data;
    data = queue_pop_tail(queue);
    sysio->print_stdout("%d,", *data);
  }
  sysio->print_stdout("\n");
  print_queue_status(queue);

  queue_dtor(queue);
  //queue->counter = 1;//segfault
}

queue_t* make_queue(void (*disposer)(ptr_t))
{
  queue_t* result;
  result = malloc(sizeof(queue_t));
  memset(result, 0, sizeof(queue_t));
  result->tail = result->head = NULL;
  result->disposer = disposer;
  return result;
}

void queue_dtor(queue_t* target)
{
  queue_t* this;
  if(!target){
    return;
  }
  this = target;
  while(!queue_is_empty(this)){
    ptr_t data;
    data = queue_pop_head(this);
    if(!this->disposer){
      DEBUGPRINT("No disposer for data %p in queue", data);
      continue;
    }
    this->disposer(data);
  }
  free(this);
}

void print_queue_status(queue_t* queue)
{
  sysio->print_stdout("queue %p properties\n"
                      "counter: %d, head: %p, tail: %p\n"
      , queue, queue->counter,queue->head, queue->tail);
}

void queue_push_head(queue_t* this, ptr_t data)
{
  dlist_t* item;
  ++this->counter;
  if(!this->head){
    this->head = this->tail = make_dlist(data);
    return;
  }
  item = make_dlist(data);
  item->next = this->head;
  this->head->prev = item;
  this->head = item;
}

ptr_t queue_pop_head(queue_t* this)
{
  dlist_t *next;
  ptr_t result;

  if(!this->head){
    return NULL;
  }
  --this->counter;
  next = this->head->next;
  result = this->head->data;
  free(this->head);
  this->head = next;
  if(next){
    next->prev = NULL;
  }else{
    this->tail = NULL;
  }
  return result;
}

ptr_t queue_peek_head(queue_t* this)
{
  if(!this->head){
    return NULL;
  }
  return this->head->data;
}

void queue_push_tail(queue_t* this, ptr_t data)
{
  dlist_t* item;
  ++this->counter;
  if(!this->tail){
    this->head = this->tail = make_dlist(data);
    return;
  }
  item = make_dlist(data);
  item->prev = this->tail;
  this->tail->next = item;
  this->tail = item;
}


ptr_t queue_pop_tail(queue_t* this)
{
  dlist_t *prev;
  ptr_t result;

  if(!this->tail){
    return NULL;
  }
  --this->counter;
  prev = this->tail->prev;
  result = this->tail->data;
  free(this->tail);
  this->tail = prev;
  if(prev){
    prev->next = NULL;
  }else{
    this->tail = NULL;
  }
  return result;
}

ptr_t queue_peek_tail(queue_t* this)
{
  if(!this->tail){
    return NULL;
  }
  return this->tail->data;
}

bool_t queue_is_empty(queue_t* this)
{
  return 0 < this->counter ? BOOL_TRUE : BOOL_FALSE;
}


