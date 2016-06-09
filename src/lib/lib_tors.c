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


//This is my comment for packet_ctor function
packet_t* packet_ctor()
{
	packet_t *result;
	result = (packet_t*) malloc(sizeof(packet_t));
	memset(result, 0, sizeof(packet_t));
	result->bytes = NULL;
	result->length = -1;
	result->connection = NULL;
	result->path_out = 0;
	result->local = BOOL_FALSE;
	return result;
} //#packet_ctor end  

command_t* cmd_ctor()
{
	command_t* result;
	result = (command_t*) malloc(sizeof(command_t));
	memset(result, 0, sizeof(command_t));
	result->action = NULL;
	result->callback = NULL;
	result->next_command = NULL;
	result->signal = NULL;
	result->request = NULL;
	result->state = COMMAND_STATE_CREATED;
	strcpy(result->name, "Unknown command");
	time(&result->created);
	return result;
} //#cmd_ctor end  

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

connection_t* conn_ctor()
{
	//int32_t       index;
	connection_t  *result;
	result = (connection_t*) malloc(sizeof(connection_t));
	memset(result, 0, sizeof(connection_t));
	return result;
}//#conn_ctor end  

interface_t *interface_ctor()
{
	interface_t *result;
	result = (interface_t*) malloc(sizeof(interface_t));
	memset(result, 0, sizeof(interface_t));
	return result;
}//#interface_ctor end  

tunnel_t *tunnel_ctor()
{
	tunnel_t *result;
	result = (tunnel_t*) malloc(sizeof(tunnel_t));
	memset(result, 0, sizeof(tunnel_t));
	result->fd = -1;
	return result;
}//#tunnel_ctor end  


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

path_t*	path_ctor()
{
	path_t *result;
	result = (path_t*) malloc(sizeof(path_t));
	memset(result, 0, sizeof(path_t));
	return result;
}//#path_ctor end  

network_t* network_ctor()
{
	network_t *result;
	result = (network_t*) malloc(sizeof(network_t));
	memset(result, 0, sizeof(network_t));
	return result;
}//#network_ctor end  

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
cmdres_t* cmdres_ctor()
{
	cmdres_t* result;
	result = (cmdres_t*) malloc(sizeof(cmdres_t));
	memset(result, 0, sizeof(cmdres_t));
	result->command = NULL;
	result->request = NULL;
	return result;
}//#cmdres_ctor end


//*
byte_t* bytes_ctor(int32_t size)
{
	byte_t* result;
	result = (byte_t*) malloc(sizeof(byte_t)*size);
	memset(result, 0, sizeof(byte_t)*size);
	return result;
}//#bytes_ctor end
/**/

request_t* request_ctor()
{
	request_t* result;
	result = (request_t*) malloc(sizeof(request_t));
	memset(result, 0, sizeof(request_t));
	return result;
}

pathring_t* pathring_ctor()
{
	pathring_t* result;
	result = (pathring_t*) malloc(sizeof(pathring_t));
	memset(result, 0, sizeof(pathring_t));
	return result;
}


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

//-------------------------------------------------------------------------------------
//----------------------- Destructors--------------------------------------------------
//-------------------------------------------------------------------------------------



void packet_dtor(packet_t* target)
{
	byte_t* bytes;
	if(target == NULL){
		return;
	}
	bytes = target->bytes;
	bytes_dtor(bytes);
	free(target);
}//# packet_dtor end

void cmd_dtor(command_t* target)
{
	if(target == NULL){
		return;
	}
	if(target->action != NULL){
		rcback_dtor((void*)target->action);
	}
	target->action = NULL;

	if(target->callback != NULL){
		cback_dtor((void*)target->callback);
	}
	target->callback = NULL;

	if(target->next_command != NULL && target->next_command != target){
		cmd_dtor(target->next_command);
	}
	target->next_command = NULL;
	if(target->signal != NULL){
		signal_dtor(target->signal);
	}

	if(target->request != NULL){
		request_dtor(target->request);
	}
	target->request = NULL;
	free(target);
}//# cmd_dtor end


void tunnel_dtor(void* tunnel)
{
	tunnel_t* target;
	if(tunnel == NULL){
		return;
	}
	target = (tunnel_t*) tunnel;
	free(target); target = NULL;
}//# tunnel_dtor end


void conn_dtor(void* connection)
{
	connection_t* target;
	if(connection == NULL){
		return;
	}
	target = (connection_t*) connection;
	free(target);
}//# conn_dtor end

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

void interface_dtor(void* interface_)
{
	interface_t* target;
	if(interface_ == NULL){
		return;
	}
	target = (interface_t*) interface_;
	free(target);
}//# interface_dtor end

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

void path_dtor(void* path)
{
	path_t* target;
	if(path == NULL){
		return;
	}
	target = (path_t*) path;
	free(target); target = NULL;
}//# path_dtor end

void network_dtor(void* network)
{
	network_t* target;
	if(network == NULL){
		return;
	}
	target = (network_t*) network;
	free(target);
}//# network_dtor end

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
void cmdres_dtor(void* target)
{
	cmdres_t* cmdres;
	if(target == NULL){
		return;
	}
	cmdres = (cmdres_t*) target;
	if(cmdres->command != NULL){
		cmd_dtor(cmdres->command);
	}
	if(cmdres->request != NULL){
		request_dtor(cmdres->request);
	}
	free(cmdres);
}//# cmdres_dtor end

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

void request_dtor(request_t *target)
{
	if(target == NULL){
		return;
	}
	free(target);
}

void pathring_dtor(pathring_t *target)
{
	pathring_t *curent, *next;
	if(target == NULL){
		return;
	}
	curent = target;
	do{
		curent->actual = NULL;
		next = curent->next;
		free(curent);
		curent = next;
	}while(curent != NULL && curent != target);
}


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
