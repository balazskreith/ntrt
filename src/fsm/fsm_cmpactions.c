#include "fsm_cmpactions.h"

#include "../cmp/cmp_evaluator.h"
#include "../cmp/cmp_listener.h"
#include "../cmp/cmp_recorder.h"
#include "../cmp/cmp_sniffer.h"
#include "fsm.h"
#include "lib_tors.h"

//components:
#include "cmp_predefs.h"
#include "cmp_defs.h"

void _fsm_mptsrv_ctor()
{

	fsm_t* machine = get_fsm();

	cmp_cmdexetor_ctor();
	cmp_pcapreader_ctor();
	cmp_sniffer_ctor();
	cmp_evaluator_ctor();
	cmp_pthsch_ctor();
	cmp_psender_ctor();
	cmp_screener_ctor();
	cmp_handshaker_ctor();
	cmp_cli_ctor();

	cmp_evaluator_t *conasr = get_cmp_evaluator();
	cmp_screener_t* screener = get_cmp_screener();
	cmp_handshaker_t* handshaker = get_cmp_handshaker();

	//get_cmp_psender()->syncronizer = conasr->syncronizer = make_barrier(2);

	//Bind:

	PRINTING_CONNECT_COMPONENTS;

	//CMP_CONNECT(*(get_cmp_precver()->send_cmdpck), get_cmp_cmdexetor()->0);

	CMP_CONNECT(conasr->demand, get_cmp_pcapreader()->supplier);
	CMP_CONNECT(conasr->send,   get_cmp_pthsch()->receiver);
	//CMP_CONNECT(get_cmp_pthsch()->send, get_cmp_fpsqsender()->receiver);
	//CMP_CONNECT(get_cmp_fpsqsender()->send, get_cmp_psender()->datpck_receiver);
	CMP_CONNECT(get_cmp_pthsch()->send, get_cmp_psender()->datpck_receiver);

	//get_cmp_pthsch()->send = get_cmp_psender()->datpck_receiver;

	CMP_CONNECT(get_cmp_psender()->send, get_cmp_pcapreader()->receiver);
	//CMP_CONNECT(get_cmp_pthsch()->send,   get_cmp_psender()->datpck_receiver);
	//CMP_CONNECT(get_cmp_psender()->send,  get_cmp_tunreader()->receiver);

	CMP_CONNECT(get_cmp_precver()->send_diagpck, screener->receiver);
	CMP_CONNECT(screener->send_pck, get_cmp_psender()->datpck_receiver);
	CMP_CONNECT(screener->send_cmd, get_cmp_cmdexetor()->receiver);

	CMP_CONNECT(get_cmp_precver()->send_cmdpck, handshaker->pck_receiver);
	CMP_CONNECT(get_cmp_precver()->send_locpck, get_cmp_cli()->pck_receiver);
	CMP_CONNECT(get_cmp_cli()->send, get_cmp_cmdexetor()->receiver);
	CMP_CONNECT(handshaker->send_cmd, get_cmp_cmdexetor()->receiver);
	CMP_CONNECT(get_cmp_cmdexetor()->send, handshaker->req_receiver);
	CMP_CONNECT(handshaker->send_pck, get_cmp_psender()->cmdpck_receiver);
	CMP_CONNECT(screener->send_req,   handshaker->req_receiver);

	get_cmp_cmdexetor()->start();
}

void _fsm_mptsrv_dtor()
{
	get_cmp_cmdexetor()->stop();

	//Destruct
	cmp_psender_dtor();
	cmp_pthsch_dtor();
	cmp_evaluator_dtor();
	cmp_sniffer_dtor();
	cmp_pcapreader_dtor();
	cmp_screener_dtor();
	cmp_handshaker_dtor();
	cmp_cmdexetor_dtor();
	cmp_cli_dtor();

	cmplib_deinit();
	dmap_deinit();
}


//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

