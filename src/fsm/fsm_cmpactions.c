#include "fsm_cmpactions.h"

#include "../cmp/cmp_evaluator.h"
#include "../cmp/cmp_recorder.h"
#include "../cmp/cmp_sniffer.h"
#include "../cmp/cmp_cmdexecutor.h"
#include "fsm.h"
#include "lib_tors.h"

//components:
#include "cmp_predefs.h"
#include "cmp_defs.h"
#include "dmap.h"

void _cmps_ntrt_ctor()
{

	fsm_t* machine = get_fsm();

	cmp_sniffer_ctor();
	cmp_evaluator_ctor();
	cmp_recorder_ctor();
	cmp_cmdexecutor_ctor();

	//Bind:
	PRINTING_CONNECT_COMPONENTS;

	CMP_CONNECT(get_cmp_sniffer()->send, get_cmp_evaluator()->sniff_receiver);
	CMP_CONNECT(get_cmp_evaluator()->send, get_cmp_recorder()->receiver);
	CMP_CONNECT(get_cmp_recorder()->send, get_cmp_evaluator()->record_receiver);
}

void _cmps_ntrt_dtor()
{
  cmp_sniffer_dtor();
  cmp_evaluator_dtor();
  cmp_recorder_dtor();
  cmp_cmdexecutor_dtor();

}


//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

