#include "../cmp/cmp_evaluator.h"
#include "../cmp/cmp_recorder.h"
#include "../cmp/cmp_sniffer.h"
#include "../cmp/cmp_cmdexecutor.h"
#include "fsm.h"
#include "dmap.h"
#include "etc_utils.h"
#include "fsm_actions.h"
#include "inc_opcall.h"

//components:
#include "cmp_predefs.h"
#include "cmp_defs.h"

static void _sys_stop();
//----------------------------------------------------------------------------------------------------
//------------------------- Transitions  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

fsm_states_t _fsm_run_trans(int32_t event, void *arg)
{
	CMP_DEF_THIS(fsm_t, get_fsm());

	switch(event)
	{
	case NTRT_EVENT_STOP:
		_sys_stop();
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

void _sys_stop()
{
	get_cmp_recorder()->stop();
	get_cmp_evaluator()->stop();
	get_cmp_cmdexecutor()->stop();

	dmap_itr_do(dmap_itr_table_pcapls, get_cmp_sniffer()->stop);
}

