#include "../cmp/cmp_evaluator.h"
#include "../cmp/cmp_listener.h"
#include "../cmp/cmp_recorder.h"
#include "../cmp/cmp_sniffer.h"
#include "fsm.h"
#include "fsm_actions.h"
#include "cmp_predefs.h"
#include "etc_iniparser.h"
#include "dmap.h"
#include "fsm_cmpactions.h"

//components:
#include "cmp_predefs.h"
#include "cmp_defs.h"

static void _sys_start();
static void _sys_shutdown();

//----------------------------------------------------------------------------------------------------
//------------------------- Transitions  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

fsm_states_t _fsm_halt_trans(int32_t event, void *arg)
{
	CMP_DEF_THIS(fsm_t, get_fsm());

	switch(event)
	{
	case NTRT_EVENT_START:
		_sys_start();
		return NTRT_STATE_RUN;
		break;
	case NTRT_EVENT_SHUTDOWN:
		_sys_shutdown();
		_fsm_mptsrv_dtor();
		return NTRT_STATE_HALT;
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

void _sys_start()
{

	get_cmp_psender()->start();

	get_cmp_evaluator()->start();

	dmap_itr_do(dmap_itr_table_con, get_cmp_precver()->start);
}

void _sys_shutdown()
{

	dmap_itr_do(dmap_itr_table_con, dmap_rem_feature_and_close);
	dmap_itr_do(dmap_itr_table_tun, dmap_rem_tun_and_close);
	dmap_itr_do(dmap_itr_table_net, dmap_rem_net);
	dmap_itr_do(dmap_itr_table_inf, dmap_rem_inf);
	dmap_itr_do(dmap_itr_table_pth, dmap_rem_pth);
	dmap_itr_do(dmap_itr_table_etc, dmap_rem_etc);
}



