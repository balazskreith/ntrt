/**
 * @file lib_makers.c
 * @brief base functions for structures defined in the library
 * @author Bal�zs, Kreith; Debrecen, Hungary
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @date 2014.02.11
*/
#include "lib_makers.h"
#include "lib_tors.h"
#include "lib_threading.h"
#include "dmap.h"

callback_t* make_callback(void (*action)(), 
						  void (*parameterized_action)(void*), 
						  void *action_parameter)
{
	callback_t* result = cback_ctor();
	result->simple = action;
	result->parameterized = parameterized_action;
	result->parameter = action_parameter;
	return result;
}

//make a callback with a simple action
callback_t* make_callback_sa(void (*action)())
{
	return make_callback(action, NULL, NULL);
}

//make a callback with a parameterized action
callback_t* make_callback_pa(void (*action)(void *), void *action_parameter)
{
	return make_callback(NULL, action, action_parameter);
}

rcallback_t* make_rcallback(void* (*action)(), 
						  void* (*parameterized_action)(void*), 
						  void *action_parameter)
{
	rcallback_t* result = rcback_ctor();
	result->simple = action;
	result->parameterized = parameterized_action;
	result->parameter = action_parameter;
	return result;
}

//make a callback with a simple action
rcallback_t* make_rcallback_sa(void* (*action)())
{
	return make_rcallback(action, NULL, NULL);
}

//make a callback with a parameterized action
rcallback_t* make_rcallback_pa(void* (*action)(void *), void *action_parameter)
{
	return make_rcallback(NULL, action, action_parameter);
}


string_t* make_string(char_t* characters)
{
	int32_t index = 0;
	string_t *result = string_ctor();
	if(characters == NULL){
		return result;
	}
	for(; index < 255 && characters[index] != '\0'; index++){
		result->text[index] = characters[index];
	}
	result->length = index;
	return result;
}

thread_t* make_thread(void *(*process)(void*), void *arg)
{
	thread_t* result;
	result = thread_ctor();
	setup_thread(result, process, arg);
	dmap_wrlock_table_thr();
	dmap_add_thr(result);
	dmap_wrunlock_table_thr();
	return result;
}

barrier_t *make_barrier(int32_t gatenum)
{
	barrier_t* result;
	result = barrier_ctor();
	result->gatenum = gatenum;
	return result;
}

eventer_arg_t *make_eventer_arg(int32_t event, void *arg)
{
	eventer_arg_t *result;
	result = eventer_arg_ctor();
	result->event = event;
	result->arg = arg;
	return result;
}

feature_t* make_feature(const char_t* name,
                        const char_t* identifier,
                        uint32_t (*evaluator)(sniff_t*,ptr_t),
                        ptr_t evaluator_data)
{
   feature_t *result;
   result = feature_ctor();
   strcpy(result->name, name);
   strcpy(result->identifier, identifier);
   result->evaluator = evaluator;
   result->evaluator_data = evaluator_data;
   return result;
}

datchain_t* make_datchain(ptr_t item)
{
  datchain_t *result;
  result = datchain_ctor();
  result->item = item;
  result->next = NULL;
  result->prev = NULL;
}


