#include "fsm.h"
#include "fsm_exist.h"
#include "fsm_actions.h"
#include "fsm_cmpactions.h"
#include "cmp_predefs.h"
#include "etc_iniparser.h"
#include "sys_prints.h"
#include "lib_tors.h"


static void _process();
static void _set_key(char_t *key, int32_t length);
static void _set_auth(char_t *auth, int32_t length);
static void _set_ipaddr(char_t *ip, int32_t length);
static void _set_port(int32_t number);
static void _setup_con();
static connection_t *_con = NULL;
//----------------------------------------------------------------------------------------------------
//------------------------- Transitions  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

fsm_states_t _fsm_client_trans(int32_t event, void *arg)
{
	CMP_DEF_THIS(fsm_t, get_fsm());
	if(_con == NULL){
		_setup_con();
	}
	switch(event)
	{
	case MPT_EVENT_SHUTDOWN:
		_fsm_mptclt_dtor();
		return MPT_STATE_EXIST;
		break;
	case MPT_EVENT_SET_CLIENT_AUTH:
		_set_auth((char_t*) arg, strlen((const char_t*)arg));
		break;
	case MPT_EVENT_SET_CLIENT_KEY:
		_set_key((char_t*) arg, strlen((const char_t*)arg));
		break;
	case MPT_EVENT_SET_CLIENT_PORT:
		_set_port((int32_t*) arg);
		break;
	case MPT_EVENT_SET_CLIENT_SERVER_ADDR:
		_set_ipaddr((char_t*) arg, strlen((const char_t*)arg));
		break;
	case MPT_EVENT_CLIENT_DO:
		_process();
		break;
	case MPT_EVENT_SET_CLIENT_SERVER_IP6:
		_con->ip_version = 6;
		break;
	default:
		WARNINGPRINT("The event (%s) you required doesn't have transition in the current state (%s)", this->event_str, this->state_str);
		break;
	}

	return this->current;
}



//----------------------------------------------------------------------------------------------------
//------------------------- Actions  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _process(void *cmd_str)
{


}

void _setup_con()
{
	_con = conn_ctor();
	_con->cmd_port_remote = 65050;
}

void _set_key(char_t *key, int32_t length)
{

}
void _set_auth(char_t *auth, int32_t length)
{

}

void _set_ipaddr(char_t *ip, int32_t length)
{

}

void _set_port(int32_t number)
{

}
