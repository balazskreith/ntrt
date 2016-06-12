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
  for(head = chain; chain; chain = chain->next){
    tail->prev = chain;
    chain = chain->next;
  }
  tail->prev->next = tail;
  return head;
}

void datchain_foreach(datchain_t* chain, void (*process)(datchain_t*, ptr_t), ptr_t data)
{
  for(; chain; chain = chain->next){
    process(chain, data);
  }
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
