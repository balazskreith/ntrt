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

//make a command
command_t* make_command(char_t *desc, rcallback_t* action, callback_t* callback)
{
	command_t* result = cmd_ctor();
	result->action = action;
	result->callback = callback;
	strcpy(result->name, desc);
	return result;
}


//make a command with a simple action
command_t* make_command_sa(char_t *desc, void*(*action)())
{
	return make_command(
		desc,
		make_rcallback_sa(action),
		NULL );
}

//make a command with a parametirezed action
command_t* make_command_pa(char_t *desc, void*(*action)(void*), void *action_parameter)
{
	return make_command(
		desc,
		make_rcallback_pa(action, action_parameter),
		NULL );
}

//make a command with a simple action and a simple callback
command_t* make_command_sa_sc(char_t *desc, void*(*action)(), void (*callback)())
{
	return make_command(
		desc,
		make_rcallback_sa(action),
		make_callback_sa(callback) );
}

//make a command with parameterized action and simple callback
command_t* make_command_pa_sc(char_t *desc, void*(*action)(void*), void* action_parameter, void (*callback)())
{
	return make_command(
		desc,
		make_rcallback_pa(action, action_parameter),
		make_callback_sa(callback) );
}

//make a command with simple action and parametrezied callback
command_t* make_command_sa_pc(char_t *desc, void*(*action)(), void (*callback)(void*), void *callback_parameter)
{
	return make_command(
		desc,
		make_rcallback_sa(action),
		make_callback_pa(callback, callback_parameter) );
}

//make a command with a parameterized action and a parameterized callback
command_t* make_command_pa_pc(char_t *desc, void*(*action)(void*), void *action_parameter, void (*callback)(void*), void *callback_parameter)
{
	return make_command(
		desc,
		make_rcallback_pa(action, action_parameter),
		make_callback_pa(callback, callback_parameter) );
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

packet_t* make_packet()
{
	packet_t* result;
	result = packet_ctor();
	result->bytes = bytes_ctor(NTRT_PACKET_LENGTH);
	result->length = NTRT_PACKET_LENGTH;
	result->header = (ip_packet_header_t*) result->bytes;
	result->con_id = -1;
	result->path_out = -1;
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

request_t *make_request(byte_t command, byte_t status, byte_t *message, connection_t *connection, int32_t size)
{
	request_t *result;
	result = request_ctor();
	result->command = command;
	result->connection = connection;
	result->status = status;
	memcpy(result->message, message, size);
	result->size = size;
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
