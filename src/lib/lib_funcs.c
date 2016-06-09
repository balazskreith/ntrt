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

void datchain_add_item(datchain_t *chain, void *item)
{
	datchain_t *end;
	datchain_t *new_;
	new_ = datchain_ctor();
	new_->item = item;
	if(chain == NULL){
		chain = new_;
		return;
	}
	end = chain;
	while(end->next != NULL);
	new_->prev = end;
	end->next = new_;
}

void datchain_rem_item(datchain_t *item)
{
	datchain_t *prev;
	datchain_t *next;
	if(item == NULL){
		return;
	}
	prev = item->prev;
	next = item->next;
	if(prev != NULL){
		prev->next = item->next;
	}
	item->prev = NULL;
	if(next != NULL){
		next->prev = item->prev;
	}
	item->next = NULL;
	datchain_dtor(item);
}

pathring_t* pathring_add(pathring_t** ring, path_t *path)
{
	pathring_t *actual;
	if(*ring == NULL){
		*ring = pathring_ctor();
		(*ring)->actual = path;
		(*ring)->next = *ring;
		return *ring;
	}
	actual = pathring_ctor();
	actual->actual = path;
	actual->next = (*ring)->next;
	(*ring)->next = actual;
	return actual;
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

command_t* cmdcat(command_t *first , command_t *last)
{
	command_t *cmd;
	if(first == NULL){
		return last;
	}
	for(cmd = first; cmd->next_command != NULL; cmd = cmd->next_command);
	cmd->next_command = last;
	return first;
}

void ptrmov(void **dst, void **src)
{
	*dst = *src;
	*src = NULL;
}
