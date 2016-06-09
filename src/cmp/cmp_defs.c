#include "cmp_defs.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "lib_threading.h"
#include "lib_tors.h"
#include "lib_makers.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "inc_texts.h"
#include "inc_unistd.h"
#include "inc_inet.h"
#include "inc_predefs.h"
#include "cmp_predefs.h"
#include "lib_puffers.h"

//----------------------------------------------------------------------------------------------------
//---------------------------- Private type declarations ---------------------------------------------
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------
#define CMP_PUFFER_RECYCLE_SIZE 64
#define CMP_NAME_PRECYCLE "Packet Recycle component"
CMP_DEF_RECPUFFER(
		static,				/*type of the declarations*/
		packet_t,			/*name of the type of the items*/
		make_packet,		/*name of the process used for constructing an item if the puffer is empty.*/
		packet_dtor,  /*name of the process used for deconstruting an item if the puffer is full.*/
		clean_packet,		/*name of the process clear the item when the recycle receives it*/
		CMP_NAME_PRECYCLE, 	/*name of the recycle puffer*/
		_cmp_recycle,		/*name of the variable used for referencing to the recycle*/
		CMP_PUFFER_RECYCLE_SIZE, /*length of the recycle puffer*/
		_cmp_recycle_ctor,	/*name of the constructor process for this recycle*/
		_cmp_recycle_dtor	/*name of the destructor process for this recycle*/
		)

packet_t* (*prov_packet)();
void (*disp_packet)(packet_t*);

//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void cmplib_init()
{
	int32_t index, count;
	//constructions
	_cmp_recycle_ctor();

	//Binding

	//Connecting
	CMP_CONNECT(disp_packet, _cmp_recycle->receiver);
	CMP_CONNECT(prov_packet, _cmp_recycle->supplier);
	//other
	count = _cmp_recycle->puffer->length >> 1;
	for(index = 0; index < count; ++index){
		_cmp_recycle->receiver(make_packet());
	}
}

void cmplib_deinit()
{
	_cmp_recycle_dtor();
}

void clean_packet(packet_t *packet)
{
	packet->connection = NULL;
	packet->con_id = -1;
	packet->path_out = -1;
	packet->park_delay = 0;
#ifndef NDEBUG
	set_mtime(&packet->mtime);
#endif
}

