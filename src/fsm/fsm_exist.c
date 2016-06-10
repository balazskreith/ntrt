#include "fsm.h"
#include "fsm_exist.h"
#include "fsm_actions.h"
#include "cmp_predefs.h"
#include "etc_iniparser.h"
#include "sys_prints.h"
#include "fsm_cmpactions.h"

static void _sys_load();

//----------------------------------------------------------------------------------------------------
//------------------------- Transitions  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

fsm_states_t _fsm_exist_trans(int32_t event, void *arg)
{
	CMP_DEF_THIS(fsm_t, get_fsm());

	switch(event)
	{
	case NTRT_EVENT_SETUP:
		_fsm_mptsrv_ctor();
		_sys_load();
		return NTRT_STATE_HALT;
		break;
	case NTRT_EVENT_CREATE_CLIENT:
		_fsm_mptclt_ctor(arg);
		return NTRT_STATE_CLIENT;
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

void _sys_load()
{
	char_t path[255];

	//load interface files
	sprintf(path, "%s", sysio->config_dir);
	inf_load_from_dir(path);

	//load config files
	sprintf(path, "%s/%s", sysio->config_dir, "connections");
	con_load_from_dir(path);

	sys_print_all();
}

