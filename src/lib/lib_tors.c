/**
 * @file lib_ctors.c
 * @brief constructors for structures defined in the library
 * @author Balazs, Kreith; Debrecen, Hungary
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @date 2014.02.11
*/
#include "lib_tors.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib_defs.h"
#include "lib_descs.h"
#include "lib_threading.h"
#include "time.h"
#include "inc_texts.h"

//-------------------------------------------------------------------------------------
//----------------------- Constructors--------------------------------------------------
//-------------------------------------------------------------------------------------


callback_t* cback_ctor()
{
	callback_t *result;
	result = (callback_t*) malloc(sizeof(callback_t));
	memset(result, 0, sizeof(callback_t));
	result->simple = NULL;
	result->parameter = NULL;
	result->parameterized = NULL;
	return result;
}//#cback_ctor end 

rcallback_t* rcback_ctor()
{
	rcallback_t *result;
	result = (rcallback_t*) malloc(sizeof(rcallback_t));
	memset(result, 0, sizeof(rcallback_t));
	result->simple = NULL;
	result->parameter = NULL;
	result->parameterized = NULL;
	result->disposable = BOOL_TRUE;
	return result;
}//#cback_ctor end 

string_t* string_ctor()
{
	string_t *result;
	result = (string_t*) malloc(sizeof(string_t));
	memset(result, 0, sizeof(string_t));
	return result;
}//#string_ctor end  

lstring_t* lstring_ctor()
{
	lstring_t *result;
	result = (lstring_t*) malloc(sizeof(lstring_t));
	memset(result, 0, sizeof(lstring_t));
	return result;
}//#lstring_ctor end  

datarray_t* datarray_ctor(int32_t length)
{
	int32_t     index;
	datarray_t  *result;
	result = (datarray_t*) malloc(sizeof(datarray_t));
	memset(result, 0, sizeof(datarray_t));
	result->length = length;
	for(index = 0; index < length; index++){
		result->items[index] = NULL;
	}
	return result;
}//#datarray_ctor end 

datchain_t* datchain_ctor()
{
	datchain_t *result;
	result = (datchain_t*) malloc(sizeof(datchain_t));
	memset(result, 0, sizeof(datchain_t));
	result->next = NULL;
	result->prev = NULL;
	result->item = NULL;
	return result;
}//#datchain_ctor end 


//*
byte_t* bytes_ctor(int32_t size)
{
	byte_t* result;
	result = (byte_t*) malloc(sizeof(byte_t)*size);
	memset(result, 0, sizeof(byte_t)*size);
	return result;
}//#bytes_ctor end
/**/


eventer_arg_t* eventer_arg_ctor()
{
	eventer_arg_t* result;
	result = (eventer_arg_t*) malloc(sizeof(eventer_arg_t));
	memset(result, 0, sizeof(eventer_arg_t));
	return result;
}

array_t* array_ctor(int32_t length)
{
	array_t* result;
	result = (array_t*) malloc(sizeof(array_t));
	memset(result, 0, sizeof(array_t));
	return result;
}

feature_t* feature_ctor()
{
  feature_t* result;
  result = (feature_t*) malloc(sizeof(feature_t));
  memset(result, 0, sizeof(feature_t));
  return result;
}

pcap_listener_t* pcap_listener_ctor()
{
  pcap_listener_t* result;
  result = (pcap_listener_t*) malloc(sizeof(pcap_listener_t));
  memset(result, 0, sizeof(pcap_listener_t));
  return result;
}

//-------------------------------------------------------------------------------------
//----------------------- Destructors--------------------------------------------------
//-------------------------------------------------------------------------------------




void cback_dtor(void* callback)
{
	callback_t* target;
	if(callback == NULL){
		return;
	}
	target = (callback_t*) callback;
	target->parameter = NULL;
	target->parameterized = NULL;
	target->simple = NULL;
	free(target);
}//# cback_dtor end

void rcback_dtor(void* rcallback)
{
	rcallback_t* target;
	if(rcallback == NULL){
		return;
	}
	target = (rcallback_t*) rcallback;
	if(target->disposable == BOOL_FALSE){
		return;
	}
	target->parameter = NULL;
	target->parameterized = NULL;
	target->simple = NULL;
	free(target);
}//# cback_dtor end

void string_dtor(void* string)
{
	string_t* target;
	if(string == NULL){
		return;
	}
	target = (string_t*) string;
	free(target); target = NULL;
}//# string_dtor end

void lstring_dtor(void* lstring)
{
	lstring_t* target;
	if(lstring == NULL){
		return;
	}
	target = (lstring_t*) lstring;
	free(target);
}//# lstring_dtor end

void datarray_dtor(void* datarray)
{
	datarray_t* target;
	int32_t     index;
	void*       item;
	if(datarray == NULL){
		return;
	}
	target = (datarray_t*) datarray;
	for(index = 0; index < target->length; index++){
		item = target->items[index];
		if(item == NULL){
			continue;
		}
		item = NULL;
	}
	target->items = NULL;
	target->length = 0;
	free(target);
}//# datarray_dtor end


void datchain_dtor(void* datchain)
{
        datchain_t *target,*next;
        if(datchain == NULL){
                return;
        }
        target = (datchain_t*) datchain;
        do{
                next = target->next;
                free(target);
                target = next;
        } while(target != NULL);
}//# datchain_dtor end
//*

/**/
//*
void bytes_dtor(void* target)
{
	if(target == NULL){
		return;
	}
	free(target);
}//# bytes_dtor end
/**/


void eventer_arg_dtor(eventer_arg_t *target)
{
	free(target);
}

void clock_event_dtor(void *target)
{
	free(target);
}

void array_dtor(void *target)
{
	free(target);
}

void feature_dtor(ptr_t target)
{
  free(target);
}

void pcap_listener_dtor(ptr_t target)
{
  pcap_listener_t *listener;
  listener = target;
  datchain_dtor(listener->features);
  free(listener);
}
