#include "lib_funcs.h"
#include "inc_predefs.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "lib_tors.h"
#include "dmap.h"
#include "dmap_sel.h"
#include "lib_makers.h"
#include "fsm.h"


void execute_cback(callback_t *cback)
{
	if(cback == NULL){
		return;
	}
	if(cback->simple != NULL){
		cback->simple();
		return;
	}

	if(cback->parameterized != NULL){
		cback->parameterized(cback->parameter);
	}
}

void* execute_rcback(rcallback_t *rcback)
{
	if(rcback == NULL){
		return NULL;
	}

	if(rcback->simple != NULL){
		return rcback->simple();
	}

	if(rcback->parameterized != NULL){
		return rcback->parameterized(rcback->parameter);
	}
	return NULL;
}

datchain_t* datchain_prepend(datchain_t *chain, ptr_t item)
{
  datchain_t *head;
  head = make_datchain(item);
  if(!chain){
    return head;
  }
  head->next = chain;
  chain->prev = head;
  return head;
}

datchain_t* datchain_append(datchain_t *chain, ptr_t item)
{
  datchain_t *tail,*head;
  tail = make_datchain(item);
  if(!chain){
    return tail;
  }
  for(tail = head = chain; tail->next; tail = tail->next);
  tail->next = item;
  return head;
}

void datchain_foreach(datchain_t* chain, void (*process)(datchain_t*, ptr_t), ptr_t data)
{
  for(; chain; chain = chain->next){
    process(chain, data);
  }
}

slist_t* slist_append(slist_t *slist, ptr_t item)
{
  slist_t* tail;
  if(!slist){
    return make_slist(item);
  }
  tail = slist;
  while(tail->next){
    tail = tail->next;
  }
  tail->next = make_slist(item);
  return slist;
}

slist_t* slist_prepend(slist_t* slist, ptr_t item)
{
  slist_t* head;
  if(!slist){
    return make_slist(item);
  }
  head = make_slist(item);
  head->next = slist;
  return head;
}

void slist_foreach(slist_t* slist, void (*process)(slist_t*, ptr_t), ptr_t data)
{
  for(; slist; slist = slist->next){
    process(slist, data);
  }
}

slist_t* slist_cpy(slist_t* slist)
{
  slist_t* head, *actual;
  if(!slist){
    return NULL;
  }
  actual = slist;
again:
  head = slist_append(head, actual->data);
  actual = actual->next;
  if(actual){
    goto again;
  }
  return head;
}

void *eventer(eventer_arg_t *eventer_arg)
{
	int32_t event;
	void *arg;
	if(eventer_arg == NULL){
		return NULL;
	}
	event = eventer_arg->event;
	arg = eventer_arg->arg;

	get_fsm()->fire(event, arg);
	return NULL;
}

void ptrmov(void **dst, void **src)
{
	*dst = *src;
	*src = NULL;
}
