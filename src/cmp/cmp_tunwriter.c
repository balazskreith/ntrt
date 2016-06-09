#include "cmp_tunwriter.h"
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

//----------------------------------------------------------------------------------------------------
//---------------------------- Private type declarations ---------------------------------------------
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//---------------------------- Public definitions ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void  _cmp_tunwriter_init();
static void  _cmp_tunwriter_deinit();
CMP_DEF(, 			         /*type of definitions*/ 										   \
		cmp_tunwriter_t,       /*type of component*/ 										  	   \
		 CMP_NAME_TUNWRITER,   /*name of the component*/ 										   \
		 _cmp_tunwriter,        /*variable name of the component*/  							   \
		 cmp_tunwriter_ctor,    /*name of the constructor function implemented automatically*/     \
		 cmp_tunwriter_dtor,    /*name of the destructor function implemented automatically*/      \
		 _cmp_tunwriter_init,   /*name of the external function called after construction*/        \
		 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
		 _cmp_tunwriter_deinit  /*name of the external function called before destruction*/        \
		);																						   \

CMP_DEF_GET_PROC(get_cmp_tunwriter, cmp_tunwriter_t, _cmp_tunwriter);

//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------
static void _writer_process(packet_t*);
static void _writer_start();
static tunnel_t* _tunnel;
static int32_t   _fd;

//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_tunwriter_init()
{
	//constructions


	//Binding
	CMP_BIND(_cmp_tunwriter->receiver, _writer_process);
	CMP_BIND(_cmp_tunwriter->start, _writer_start);

	//Connecting


	//other

}

void _writer_start()
{
	int index = 0;
	dmap_rdlock_table_tun();
	for(index = 0; dmap_itr_table_tun(&index, &_tunnel) == BOOL_TRUE; ++index){break;}
	_fd = _tunnel->fd;
	dmap_rdunlock_table_tun();
}

void  _cmp_tunwriter_deinit()
{

}



//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void  _writer_process(packet_t *packet)
{
	CMP_DEF_THIS(cmp_tunwriter_t, _cmp_tunwriter);
	sysio->tunnel_write(_fd, packet->bytes, packet->length);
	//DEBUGPRINT("Packet is written to the tunnel");
	this->send(packet);
}

//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  -----------------------------------------------------------
//----------------------------------------------------------------------------------------------------
