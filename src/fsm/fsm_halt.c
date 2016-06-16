#include "../cmp/cmp_evaluator.h"
#include "../cmp/cmp_recorder.h"
#include "../cmp/cmp_sniffer.h"
#include "../cmp/cmp_cmdexecutor.h"
#include "fsm.h"
#include "fsm_actions.h"
#include "cmp_predefs.h"
#include "etc_iniparser.h"
#include "dmap.h"
#include "fsm_cmpactions.h"
#include "sys_confs.h"

//components:
#include "cmp_predefs.h"
#include "cmp_defs.h"
#include "dmap.h"

static void _sys_setup();
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
	  case NTRT_EVENT_SETUP:
	    _cmps_ntrt_ctor();
	    _sys_setup();
	    return NTRT_STATE_HALT;
            break;
	  case NTRT_EVENT_START:
            _sys_start();
            return NTRT_STATE_RUN;
            break;
	  case NTRT_EVENT_SHUTDOWN:
            _sys_shutdown();
            _cmps_ntrt_dtor();
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

void _sys_setup()
{
  features_load();
  con_load_from_file(sysio->config_file);
}

void _sys_start()
{

  get_cmp_recorder()->start();
  get_cmp_evaluator()->start();
  get_cmp_cmdexecutor()->start();

  dmap_itr_do(dmap_itr_table_pcapls, get_cmp_sniffer()->start);

}

void _sys_shutdown()
{

  dmap_itr_do(dmap_itr_table_features, dmap_rem_feature);
  dmap_itr_do(dmap_itr_table_pcapls, dmap_rem_pcapls);
  dmap_itr_do(dmap_itr_table_thr, dmap_rem_thr);

}



