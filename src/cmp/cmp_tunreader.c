#include "cmp_tunreader.h"
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

static void* _reader_start();
static void* _reader_stop();
static packet_t* _reader_process();
#define CMP_NAME_READER "Reader component"
typedef struct _cmp_reader_struct_t
{
	tunnel_t*        tunnel;
	int32_t          fd;
	packet_t*        (*demand)();
	packet_t*        (*supplier)(); //send to the recycle
}_cmp_reader_t;

//----------------------------------------------------------------------------------------------------
//---------------------------- Public definitions ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void  _cmp_tunreader_init();
static void  _cmp_tunreader_deinit();
CMP_DEF(, 			         /*type of definitions*/ 										   \
		cmp_tunreader_t,       /*type of component*/ 										  	   \
		 CMP_NAME_TUNREADER,   /*name of the component*/ 										   \
		 _cmp_tunreader,        /*variable name of the component*/  							   \
		 cmp_tunreader_ctor,    /*name of the constructor function implemented automatically*/     \
		 cmp_tunreader_dtor,    /*name of the destructor function implemented automatically*/      \
		 _cmp_tunreader_init,   /*name of the external function called after construction*/        \
		 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
		 _cmp_tunreader_deinit  /*name of the external function called before destruction*/        \
		);																						   \

CMP_DEF_GET_PROC(get_cmp_tunreader, cmp_tunreader_t, _cmp_tunreader);

//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------
CMP_DEF(static,	      		/*type of definitions*/ 									      \
		 _cmp_reader_t,     /*type of component*/ 											      \
		 CMP_NAME_READER,   /*name of the component*/ 										      \
		 _cmp_reader,       /*variable name of the component*/  								  \
		 _cmp_reader_ctor,  /*name of the constructor function implemented automatically*/       \
		 _cmp_reader_dtor,  /*name of the destructor function implemented automatically*/        \
		 __CMP_NO_INIT_FUNC_,  /*name of the external function called after construction*/          \
		 __NO_TEST_FUNC_,   /*name of the external function called after initialization*/         \
		 __CMP_NO_DEINIT_FUNC_  /*name of the external function called before destruction*/          \
		);

#define CMP_PUFFER_RECYCLE_SIZE 64
#define CMP_NAME_PRECYCLE "Packet Recycle component"
CMP_DEF_RECPUFFER(
		static,				/*type of the declarations*/
		packet_t,			/*name of the type of the items*/
		prov_packet,		/*name of the process used for constructing an item if the puffer is empty.*/
		disp_packet,  /*name of the process used for deconstruting an item if the puffer is full.*/
		clean_packet,		/*name of the process clear the item when the recycle receives it*/
		CMP_NAME_PRECYCLE, 	/*name of the recycle puffer*/
		_cmp_recycle,		/*name of the variable used for referencing to the recycle*/
		CMP_PUFFER_RECYCLE_SIZE, /*length of the recycle puffer*/
		_cmp_recycle_ctor,	/*name of the constructor process for this recycle*/
		_cmp_recycle_dtor	/*name of the destructor process for this recycle*/
		)



//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_tunreader_init()
{
	int32_t index, count;
	//constructions
	_cmp_recycle_ctor();
	_cmp_reader_ctor();

	//Binding
	CMP_BIND(_cmp_tunreader->start, _reader_start);
	CMP_BIND(_cmp_tunreader->stop,  _reader_stop);
	CMP_BIND(_cmp_reader->supplier, _reader_process);

	//Connecting
	CMP_CONNECT(_cmp_tunreader->receiver, _cmp_recycle->receiver);
	CMP_CONNECT(_cmp_tunreader->supplier, _cmp_reader->supplier);
	CMP_CONNECT(_cmp_reader->demand,      _cmp_recycle->supplier);

	//other
	count = _cmp_recycle->puffer->length >> 1;
	for(index = 0; index < count; ++index){
		_cmp_recycle->receiver(make_packet());
	}
}

void  _cmp_tunreader_deinit()
{
	_cmp_recycle_dtor();
	_cmp_reader_dtor();
}

void* _reader_start()
{
	CMP_DEF_THIS(_cmp_reader_t, _cmp_reader);
	int32_t index;
	tunnel_t *tunnel = NULL;

	dmap_rdlock_table_tun();
	for(index = 0; dmap_itr_table_tun(&index, &tunnel) == BOOL_TRUE; ++index) break;
	this->tunnel = tunnel;
	this->fd = tunnel->fd;
	//DEBUGPRINT("fd:%d", this->fd);
	dmap_rdunlock_table_tun();
	return NULL;
}

void* _reader_stop()
{
	return NULL;
}



//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
packet_t*  _reader_process()
{
	CMP_DEF_THIS(_cmp_reader_t, (_cmp_reader_t*) _cmp_reader);
	packet_t            *result;

	//CMP_DEMAND(CMP_NAME_TUNREADER, result, this->demand);
	result = this->demand();
	//result = this->demand();
	//comment: tunnel_read in inc.io.h for platform independency
	result->length = read(this->fd, result->bytes, NTRT_PACKET_LENGTH);

	//CMP_SUPPLY(CMP_NAME_TUNREADER, result);
	return result;
}

//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  -----------------------------------------------------------
//----------------------------------------------------------------------------------------------------
