#include "cmp_cli.h"
#include "dmap_sel.h"
#include "lib_defs.h"
#include "cmp_defs.h"
#include "lib_descs.h"
#include "lib_threading.h"
#include "lib_tors.h"
#include "lib_makers.h"
#include <time.h>
#include <stdlib.h>
#include "inc_mtime.h"
#include <stdio.h>
#include "inc_texts.h"
#include "inc_unistd.h"
#include "inc_inet.h"
#include "inc_predefs.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "inc_io.h"
#include "cmp_predefs.h"
#include "lib_puffers.h"
#include "etc_cli.h"



//----------------------------------------------------------------------------------------------------
//---------------------------- Private type declarations ---------------------------------------------
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//---------------------------- Public definitions ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void  _cmp_cli_init();
static void  _cmp_cli_deinit();
CMP_DEF(, 			         /*type of definitions*/ 										   \
		cmp_cli_t,       /*type of component*/ 										  	   \
		 CMP_NAME_TUNREADER,   /*name of the component*/ 										   \
		 _cmp_cli,        /*variable name of the component*/  							   \
		 cmp_cli_ctor,    /*name of the constructor function implemented automatically*/     \
		 cmp_cli_dtor,    /*name of the destructor function implemented automatically*/      \
		 _cmp_cli_init,   /*name of the external function called after construction*/        \
		 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
		 _cmp_cli_deinit  /*name of the external function called before destruction*/        \
		);																						   \

CMP_DEF_GET_PROC(get_cmp_cli, cmp_cli_t, _cmp_cli);
static void _cmp_cli_pck_process(packet_t*);

//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_cli_init()
{
	//constructions

	//Binding
	CMP_BIND(_cmp_cli->pck_receiver, _cmp_cli_pck_process);

	//Connecting

	//other
}

void  _cmp_cli_deinit()
{

}


//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _cmp_cli_pck_process(packet_t *packet)
{
	CMP_DEF_THIS(cmp_cli_t, (cmp_cli_t*) _cmp_cli);
	int32_t index, rv;
	command_t* command = NULL;
	//DEBUGPRINT("Received command: %s", packet->bytes);
	//for(index = 0; index < 32; ++index){
	//	DEBUGPRINT("%d: %X;", index, packet->bytes[index]);
	//}
	//return;
	command = make_cmd_from_str(packet->bytes, &rv);
	this->send(command);
}


//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  -----------------------------------------------------------
//----------------------------------------------------------------------------------------------------
