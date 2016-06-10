#include <time.h>
#include <stdarg.h>
#include "cmp_handshaker.h"
#include "lib_tors.h"
#include "cmp_predefs.h"
#include "lib_debuglog.h"
#include "dmap.h"
#include "dmap_sel.h"
#include "etc_auth.h"
#include "inc_mtime.h"
#include "lib_funcs.h"
#include "inc_predefs.h"
#include "inc_texts.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "lib_debuglog.h"
#include "lib_puffers.h"
#include "cmp_defs.h"
#include "sys_ctrls.h"
#include "etc_endian.h"


static void _cmp_handshaker_init();
static void _cmp_handshaker_deinit();
static void* _cmp_handshaker_start();
static void* _cmp_handshaker_stop();
#define CMP_NAME_HANDSHAKER "Handshake handler component"
CMP_DEF(, 			    /*type of definitions*/
		cmp_handshaker_t,          /*type of component*/
		CMP_NAME_HANDSHAKER,   /*name of the component*/
		 _cmp_handshaker,           /*variable name of the component*/
		 cmp_handshaker_ctor,      /*name of the constructor function implemented automatically*/
		 cmp_handshaker_dtor,      /*name of the destructor function implemented automatically*/
		 _cmp_handshaker_init,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 _cmp_handshaker_deinit      /*name of the external function called before destruction*/
		);

CMP_DEF_GET_PROC(get_cmp_handshaker, cmp_handshaker_t, _cmp_handshaker);
//----------------------------------------------------------------------------------------------------
//---------------------------- Subcomponents type definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

/** \typedef cmd_info_t
      \brief describes a command info
  */

typedef enum
{
	HS_ROLE_EXECUTER = 2,
	HS_ROLE_REQUESTER = 1,
	HS_ROLE_UNDEFINED = 0,
}
role_t;

typedef struct handshake_struct_t
{
	int32_t           round;
	bool_t            active;
	role_t            role;
	bool_t            executed;
	signal_t*         signal;
}handshake_t;

typedef struct _cmp_pckreceiver_struct_t
{
	void           (*receiver)(packet_t*);
	void           (*send)(packet_t*);
}_cmp_pckreceiver_t;

typedef struct _cmp_pcksender_struct_t
{
	void           (*receiver)(packet_t*);
	void           (**send)(packet_t*);
}_cmp_pcksender_t;

typedef struct _cmp_cmdsender_struct_t
{
	void           (*receiver)(command_t*);
	void           (**send)(command_t*);
}_cmp_cmdsender_t;

typedef struct _cmp_procselector_struct_t
{
	void      (*receiver)(packet_t*);
	void      (*send2exeproc)(packet_t*);
	void      (*send2exerec)(packet_t*);
	void      (*send2reqrec)(packet_t*);

}_cmp_procselector_t;

typedef struct _cmp_executor_struct_t
{
	void    (*processor)(packet_t*);
	packet_t* received;
	void    (*reciever)(packet_t*);
	void    (*execute)(command_t*);
	void    (*send)(packet_t*);
}_cmp_executor_t;

typedef struct _cmp_requester_struct_t
{
	void    (*processor)(request_t*);
	packet_t* received;
	void    (*reciever)(packet_t*);
	void    (*send)(packet_t*);
}_cmp_requester_t;

//----------------------------------------------------------------------------------------------------
//---------------------------- Subcomponents declarations --------------------------------------------
//----------------------------------------------------------------------------------------------------


static void _cmp_pckreceiver_process(packet_t*);
#define CMP_NAME_PCKRECEIVER	"Handshaker packet receiver component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_pckreceiver_t,          /*type of component*/
		CMP_NAME_PCKRECEIVER,       /*name of the component*/
		 _cmp_pckreceiver,           /*variable name of the component*/
		 _cmp_pckreceiver_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_pckreceiver_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);

static void _cmp_pcksender_process(packet_t*);
#define CMP_NAME_PCKSENDER	"Handshaker packet sender component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_pcksender_t,          /*type of component*/
		CMP_NAME_PCKSENDER,       /*name of the component*/
		 _cmp_pcksender,           /*variable name of the component*/
		 _cmp_pcksender_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_pcksender_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);

static void _cmp_procselector_process(packet_t*);
#define CMP_NAME_PCKSENDER	"Handshaker packet sender component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_procselector_t,          /*type of component*/
		CMP_NAME_PCKSENDER,       /*name of the component*/
		 _cmp_procselector,           /*variable name of the component*/
		 _cmp_procselector_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_procselector_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);

static void _cmp_cmdsender_process(command_t *command);
#define CMP_NAME_CMDSENDER	"Handshaker command sender component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_cmdsender_t,          /*type of component*/
		CMP_NAME_CMDSENDER,       /*name of the component*/
		 _cmp_cmdsender,           /*variable name of the component*/
		 _cmp_cmdsender_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_cmdsender_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);


static void _cmp_requester_receiver(packet_t*);
static void _cmp_requester_processor(request_t*);
#define CMP_NAME_PCKRECEIVER	"Handshaker packet receiver component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_requester_t,          /*type of component*/
		CMP_NAME_PCKRECEIVER,       /*name of the component*/
		 _cmp_requester,           /*variable name of the component*/
		 _cmp_requester_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_requester_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);


static void _cmp_executor_receiver(packet_t*);
static void _cmp_executor_processor(packet_t*);
#define CMP_NAME_PCKRECEIVER	"Handshaker packet receiver component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_executor_t,          /*type of component*/
		CMP_NAME_PCKRECEIVER,       /*name of the component*/
		 _cmp_executor,           /*variable name of the component*/
		 _cmp_executor_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_executor_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);


#define CMP_NAME_PCKFORWARDER	"Executor processor threader"
CMP_PUSH_THREADER(packet_t,
		pckforwarder,
		CMP_NAME_PCKFORWARDER,
		_pckforwarder_start,
		_pckforwarder_stop);

#define CMP_NAME_REQFORWARDER	"Request processor threader"
CMP_PUSH_THREADER(request_t,
		reqforwarder,
		CMP_NAME_REQFORWARDER,
		_reqforwarder_start,
		_reqforwarder_stop);


static request_t   _actual;
static handshake_t _process;

//private tools
static void         _reset();
static bool_t       _conTest(packet_t*);
static bool_t       _roundTest(packet_t*);

static byte_t*      _request_command(packet_t *packet);
static byte_t*      _request_status(packet_t *packet);
static byte_t       _request_auth_type(packet_t *packet);
static byte_t       _request_auth_length(packet_t *packet);
static byte_t*      _request_round(packet_t *packet);
static ipv6_addr_t* _request_ip_local(packet_t *packet);
static ipv6_addr_t* _request_ip_remote(packet_t *packet);
static int32_t*     _request_size(packet_t *packet);
static byte_t*      _request_message(packet_t *packet);
static packet_t*    _move_received(packet_t**);
static packet_t*    _prov_packet();
static void         _setup_packet(packet_t *packet);
static request_t*   _make_request();
static void         _command_executed();
static command_t*   _get_command();

static void _process_begin();
static void _process_lock();
static void _process_unlock();
static void _process_end();
static void _process_suspend(int16_t timein_ms);
static void _process_wait();
static void _process_set();

//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _cmp_handshaker_init()
{
	_process.signal = signal_ctor();
	_reset();

	_cmp_pckreceiver_ctor();
	_cmp_pcksender_ctor();
	_cmp_pckforwarder_ctor();
	_cmp_reqforwarder_ctor();
	_cmp_procselector_ctor();
	_cmp_cmdsender_ctor();
	_cmp_requester_ctor();
	_cmp_executor_ctor();

	//Bind
	CMP_BIND(_cmp_handshaker->start,   _cmp_handshaker_start);
	CMP_BIND(_cmp_handshaker->stop,    _cmp_handshaker_stop);

	CMP_BIND(_cmp_pckreceiver->receiver,     _cmp_pckreceiver_process);
	CMP_BIND(_cmp_pcksender->receiver,       _cmp_pcksender_process);
	CMP_BIND(_cmp_cmdsender->receiver,       _cmp_cmdsender_process);
	CMP_BIND(_cmp_executor->processor,       _cmp_executor_processor);
	CMP_BIND(_cmp_executor->reciever,        _cmp_executor_receiver);
	CMP_BIND(_cmp_procselector->receiver,    _cmp_procselector_process);
	CMP_BIND(_cmp_requester->processor,      _cmp_requester_processor);
	CMP_BIND(_cmp_requester->reciever,       _cmp_requester_receiver);

	//Connect
	CMP_CONNECT(_cmp_handshaker->pck_receiver, _cmp_pckreceiver->receiver);
	CMP_CONNECT(_cmp_handshaker->req_receiver, _cmp_reqforwarder->receiver);
	CMP_CONNECT(_cmp_handshaker->start,        _cmp_handshaker_start);
	CMP_CONNECT(_cmp_handshaker->stop,         _cmp_handshaker_stop);
	CMP_CONNECT(_cmp_cmdsender->send,          &_cmp_handshaker->send_cmd);
	CMP_CONNECT(_cmp_pcksender->send,          &_cmp_handshaker->send_pck);

	CMP_CONNECT(_cmp_pckreceiver->send,          _cmp_procselector->receiver);
	CMP_CONNECT(_cmp_procselector->send2exeproc, _cmp_pckforwarder->receiver);
	CMP_CONNECT(_cmp_procselector->send2exerec,  _cmp_executor->reciever);
	CMP_CONNECT(_cmp_procselector->send2reqrec,  _cmp_requester->reciever);
	CMP_CONNECT(_cmp_pckforwarder->send,         _cmp_executor->processor)
	CMP_CONNECT(_cmp_reqforwarder->send,         _cmp_requester->processor);
	CMP_CONNECT(_cmp_requester->send,            _cmp_pcksender->receiver);
	CMP_CONNECT(_cmp_executor->send,             _cmp_pcksender->receiver);
	CMP_CONNECT(_cmp_executor->execute,          _cmp_cmdsender->receiver);
}


void _cmp_handshaker_deinit()
{
	signal_dtor(_process.signal);

	_cmp_pckreceiver_dtor();
	_cmp_pcksender_dtor();
	_cmp_pckforwarder_dtor();
	_cmp_reqforwarder_dtor();
	_cmp_procselector_dtor();
	_cmp_cmdsender_dtor();
	_cmp_requester_dtor();
	_cmp_executor_dtor();
}

//----------------------------------------------------------------------------------------------------
//------------------------- Activator  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void* _cmp_handshaker_start()
{
	_pckforwarder_start();
	_reqforwarder_start();
	return NULL;
}

void* _cmp_handshaker_stop()
{
	_pckforwarder_stop();
	_reqforwarder_stop();
	return NULL;
}

//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


void _cmp_pckreceiver_process(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_pckreceiver_t, _cmp_pckreceiver);
	//authenticate packet

	dmap_rdlock_table_con();
	if(!authTest(packet->connection, packet->bytes, packet->length))
	{
		dmap_rdunlock_table_con();
		ERRORPRINT("Handshaker: packet authentication error");
		disp_packet(packet);
		return;
	}
	//DEBUGPRINT("Packet auth is ok");
	dmap_rdunlock_table_con();
	_process_lock();
	//connection test
	if(_conTest(packet) == BOOL_FALSE){
		DEBUGPRINT("Handshaker connection check is failed.");
		disp_packet(packet);
		_process_unlock();
		return;
	}
	//DEBUGPRINT("Packet connection is ok");
	//round test
	if(_roundTest(packet) == BOOL_FALSE){
		DEBUGPRINT("Handshaker round check is failed. Expected round: %d, received round: %d", _process.round, *_request_round(packet));
		disp_packet(packet);
		_process_unlock();
		return;
	}
	//DEBUGPRINT("Packet round is ok");
	if(_process.round == 1){
		_process.role = HS_ROLE_EXECUTER;
	}
	_process_unlock();
	this->send(packet);
}

void _cmp_procselector_process(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_procselector_t, _cmp_procselector);
	void (*send)(packet_t*);

	_process_lock();
	switch(_process.role)
	{
	case HS_ROLE_EXECUTER:
		send = (_process.round == 1) ? this->send2exeproc : this->send2exerec;
		break;
	case HS_ROLE_REQUESTER:
		send = this->send2reqrec;
		break;
	default:
		ERRORPRINT("Undefined role at Handshake process");
		disp_packet(packet);
		return;

	}
	_process_unlock();
	send(packet);
}


void _cmp_executor_receiver(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_executor_t, _cmp_executor);
	_process_lock();
	if(this->received != NULL){
		disp_packet(this->received);
	}
	this->received = packet;
	_process_unlock();
}

void _cmp_executor_processor(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_executor_t, _cmp_executor);
	command_t *command;
	int32_t wait;

	_process_begin();
	INFOPRINT("Handshake started, role: Executor");
	//initialization
	_actual.command = *_request_command(packet);
	_actual.status = *_request_status(packet);
	_actual.connection = packet->connection;
	_actual.size = *_request_size(packet);
	disp_packet(packet);

	//send acknowledge
	_process.round = 3;
	for(this->received = NULL, wait = 0; this->received == NULL && wait < 5; ++wait)
	{
		packet = _prov_packet();
		*_request_round(packet) = 2;
		this->send(packet);
		_process_suspend(100);
	}

	if(this->received == NULL){
		ERRORPRINT("Handshake executor didn't received the command at round 3");
		_process_end();
		return;
	}

	//execute command
	packet = _move_received(&this->received);
	memcpy(_actual.message,  _request_message(packet), _actual.size);
	command = _get_command();
	strcpy((char*)_actual.message, "Request is not executed");
	_actual.size = 23;
	this->execute(command);
	_process_wait();
	INFOPRINT("Handshake executor response is: %s", _actual.message);
	disp_packet(packet);

	//send response
	packet = _prov_packet();
	*_request_round(packet) = 4;
	memcpy(_request_message(packet), _actual.message, _actual.size);
	packet->length = _request_auth_length(packet) + 40 + _actual.size;
	this->send(packet);

	_process_end();
}


void _cmp_requester_receiver(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_requester_t, _cmp_requester);
	_process_lock();
	if(this->received != NULL){
		disp_packet(this->received);
	}
	this->received = packet;
	_process_unlock();
}

void _cmp_requester_processor(request_t *request)
{
	CMP_DEF_THIS(_cmp_requester_t, _cmp_requester);
	packet_t* packet;
	int32_t wait;

	_process_begin();
	for(wait = 0; _process.role != HS_ROLE_UNDEFINED && wait < 30; ++wait){
		_process_suspend(40);
	}
	if(wait == 30){
		ERRORPRINT("Request failed at waiting");
		_process_unlock();
		return;
	}
	INFOPRINT("Handshake started, role: Requester");
	_process.role = HS_ROLE_REQUESTER;
	_actual = *request;

	//send init packet
	_process.round = 2;
	for(wait = 0, this->received = NULL; this->received == NULL && wait < 5; ++wait)
	{
		packet = _prov_packet();
		*_request_round(packet) = 1;
		*_request_size(packet) = _actual.size;
		packet->length = _request_auth_length(packet) + 44;
		this->send(packet);
		_process_suspend(100);
	}
	packet = _move_received(&this->received);
	if(packet == NULL){
		ERRORPRINT("Handshake requester didn't received an acknowledgment at round 2");
		_process_end();
		return;
	}

	//send command
	_process.round = 4;
	for(wait = 0, this->received = NULL; this->received == NULL && wait < 5; ++wait)
	{
		packet = _prov_packet();
		*_request_round(packet) = 3;
		memcpy(_request_message(packet), _actual.message, _actual.size);
		packet->length = _request_auth_length(packet) + 40 + _actual.size;
		this->send(packet);
		_process_suspend(100);
	}
	packet = _move_received(&this->received);
	if(packet == NULL){
		ERRORPRINT("Handshake requester didn't received the response at round 4");
		_process_end();
		return;
	}
	_actual.size = packet->length - 40 - _request_auth_length(packet);
	memcpy(_actual.message, _request_message(packet), _actual.size);
	INFOPRINT("Handshake requestion received an answer: %s", _actual.message);
	_process_end();
}

void _cmp_cmdsender_process(command_t *command)
{
	CMP_DEF_THIS(_cmp_cmdsender_t, _cmp_cmdsender);
	void      (*send)(command_t*);

	if(command == NULL){
		ERRORPRINT("Command can not be interpreted");
		return;
	}

	send = *(this->send);
	command->callback = make_callback_sa(_command_executed);
	command->signal = _process.signal;
	send(command);
}

void _cmp_pcksender_process(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_pcksender_t, _cmp_pcksender);
	void (*send)(packet_t*);
	send = *(this->send);
	authSet(packet->connection, (char*) packet->bytes, NTRT_PACKET_LENGTH);
	//CMP_SEND(CMP_NAME_PCKSENDER, send, packet);
	send(packet);
}

//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  -----------------------------------------------------------
//----------------------------------------------------------------------------------------------------

packet_t* _move_received(packet_t **packet_ptr)
{
	packet_t *result = *packet_ptr;
	*packet_ptr = NULL;
	return result;
}

void _command_executed()
{
	_process_lock();
	switch(_actual.command){
		default:
			strcpy((char*)_actual.message, "Request is executed successfully");
			_actual.size = 32;
		break;
	}
	_process.executed = BOOL_TRUE;
	_process_unlock();
	return;
}

packet_t* _prov_packet()
{
	packet_t* result;
	result = prov_packet();
	_setup_packet(result);
	return result;
}

void _reset()
{
	_actual.connection = NULL;
	_actual.status = 0;
	memset(_actual.message, 0, 128);
	_actual.size = 0;

	_process.active = BOOL_FALSE;
	_process.executed = BOOL_FALSE;
	_process.role = HS_ROLE_UNDEFINED;
	_process.round = 1;
}

void _process_begin()
{
	signal_lock(_process.signal);
}

void _process_lock()
{
	signal_lock(_process.signal);
}

void _process_unlock()
{
	signal_unlock(_process.signal);
}

void _process_end()
{
	signal_unlock(_process.signal);
	_reset();
}

void _process_suspend(int16_t timein_ms)
{
	_process_unlock();
	thread_sleep(timein_ms);
	_process_lock();
}

void _process_wait()
{
	signal_wait(_process.signal);
}

void _process_set()
{
	signal_set(_process.signal);
}

void _setup_packet(packet_t *packet)
{
	//packet->connection = con;
	//packet->con_id = dmap_getid_bycon(con);
	connection_t* con = _actual.connection;
	packet->connection = con;
	packet->con_id = dmap_getid_bycon(con);
	memcpy(_request_ip_local(packet),  con->ip_local, SIZE_IN6ADDR);
	memcpy(_request_ip_remote(packet), con->ip_remote,SIZE_IN6ADDR);
	*_request_command(packet) = _actual.command;
	*_request_status(packet) =  _actual.status;
}

request_t* _make_request()
{
	request_t *result = request_ctor();
	*result = _actual;
	return result;
}

byte_t* _request_command(packet_t *packet)
{
	return (byte_t*) &packet->bytes[0];
}

byte_t* _request_status(packet_t *packet)
{
	return (byte_t*) &packet->bytes[1];
}

byte_t _request_auth_type(packet_t *packet)
{
	return packet->bytes[2];
}

byte_t _request_auth_length(packet_t *packet)
{
	byte_t auth_type = _request_auth_type(packet);
	return  authSize(auth_type);
}

byte_t* _request_round(packet_t *packet)
{
	int32_t       sh;
	sh = _request_auth_length(packet);
	return (byte_t*)  &packet->bytes[sh + 4];
}

ipv6_addr_t* _request_ip_local(packet_t *packet)
{
	int32_t       sh;
	sh = _request_auth_length(packet);
	return (ipv6_addr_t*)  &packet->bytes[sh + 8];
}

ipv6_addr_t* _request_ip_remote(packet_t *packet)
{
	int32_t       sh;
	sh = _request_auth_length(packet);
	return (ipv6_addr_t*)  &packet->bytes[sh + 24];
}


int32_t* _request_size(packet_t *packet)
{
	int32_t       sh;
	sh = _request_auth_length(packet);
	return (int32_t*)  &packet->bytes[sh + 40];
}


byte_t* _request_message(packet_t *packet)
{
	int32_t       sh;
	sh = _request_auth_length(packet);
	return (byte_t*)  &packet->bytes[sh + 40];
}


bool_t _conTest(packet_t* packet)
{
	bool_t result;
	if(_actual.connection == NULL){
		return BOOL_TRUE;
	}
	//connection check
	result = _actual.connection == packet->connection;
	return result;
}


bool_t _roundTest(packet_t* packet)
{
	bool_t result;
	//round check
	result = _process.round == *_request_round(packet) ? BOOL_TRUE : BOOL_FALSE;
	return result;
}

command_t* _get_command()
{
	request_t* request = _make_request();
	command_t* result = make_command_from_request(request);
	request_dtor(request);
	return result;
}

void _debug_actual()
{
	int32_t index;

	DEBUGPRINT("Handshake.actual.active: %d", _process.active);
	DEBUGPRINT("Handshake.actual.connection: %p", _actual.connection);
	DEBUGPRINT("Handshake.actual.role: %d", _process.role);
	DEBUGPRINT("Handshake.actual.round: %d", _process.round);
	DEBUGPRINT("Handshake.actual.status: %d", _actual.status);
	DEBUGPRINT("Handshake.actual.size: %d", _actual.size);
	for(index = 0; index < _actual.size; ++index){
		printf("%x", _actual.message[index]);
	}
	printf("\n");
}
