#include "cmp_pthsch.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "etc_ipsearch.h"
#include "inc_texts.h"
#include "inc_predefs.h"
#include "dmap.h"
#include "dmap_sel.h"
#include "cmp_tests.h"
#include "cmp_predefs.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "cmp_defs.h"
#include "lib_tors.h"


typedef struct _pthsch_confs_struct_t
{
	spin_t*  spin;
	void    (*selector)(packet_t*);
	int32_t   selected_index[NTRT_MAX_CONNECTION_NUM];
	int32_t   packet_counter[NTRT_MAX_CONNECTION_NUM];
	path_t*   selected_path[NTRT_MAX_CONNECTION_NUM];
}_pthsch_confs_t;

typedef void (*_packet_scheduler_t)(packet_t*);

static void _cmp_pthsch_init();
static void _cmp_pthsch_deinit();
static void* _cmp_pthsch_start();
static void* _cmp_pthsch_stop();
static void* _cmp_pthsch_restart();

CMP_DEF(, 			      		/*type of definitions*/ 									      \
		 cmp_pthsch_t,       /*type of component*/ 											      \
		 CMP_NAME_PTHSCH,    /*name of the component*/ 										      \
		 _cmp_pthsch,        /*variable name of the component*/  								  \
		 cmp_pthsch_ctor,    /*name of the constructor function implemented automatically*/       \
		 cmp_pthsch_dtor,    /*name of the destructor function implemented automatically*/        \
		 _cmp_pthsch_init,   /*name of the external function called after construction*/          \
		 __NO_TEST_FUNC_,    /*name of the external function called after initialization*/         \
		 _cmp_pthsch_deinit  /*name of the external function called before destruction*/          \
		);

static void _disp_packet(packet_t*);
#define CMP_NAME_PHOLDER "Packet Holder component"
CMP_DEF_RECPUFFER(
		static,				/*type of the declarations*/
		packet_t,			/*name of the type of the items*/
		prov_packet,		/*name of the process used for constructing an item if the puffer is empty.*/
		_disp_packet,  /*name of the process used for deconstruting an item if the puffer is full.*/
		clean_packet,		/*name of the process clear the item when the recycle receives it*/
		CMP_NAME_PHOLDER, 	/*name of the recycle puffer*/
		_cmp_pholder,		/*name of the variable used for referencing to the recycle*/
		1000, /*length of the recycle puffer*/
		_cmp_pholder_ctor,	/*name of the constructor process for this recycle*/
		_cmp_pholder_dtor	/*name of the destructor process for this recycle*/
		)



CMP_DEF_GET_PROC(get_cmp_pthsch, cmp_pthsch_t, _cmp_pthsch);

//----------------------------------------------------------------------------------------------------
//---------------------------- Private declarations --------------------------------------------------
//----------------------------------------------------------------------------------------------------
static _packet_scheduler_t _schedulers[NTRT_SCHEDULING_TYPES_NUM];
static pathring_t* _rings[NTRT_MAX_CONNECTION_NUM];
static int32_t _con2schtype[NTRT_MAX_CONNECTION_NUM];

static void _cmp_pthsch_process(packet_t*);
//static path selector
static void _windowed_pthselector(packet_t*);

//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_pthsch_init()
{
	_cmp_pthsch->is_started = BOOL_FALSE;

	//Constructions
	_cmp_pholder_ctor();

	//initialization
	int32_t index;
	for(index = 0; index < NTRT_SCHEDULING_TYPES_NUM; ++index){
		_schedulers[index] = _cmp_pholder->receiver;
	}

	//Binding
	CMP_BIND(_cmp_pthsch->start, _cmp_pthsch_start);
	CMP_BIND(_cmp_pthsch->stop, _cmp_pthsch_stop);
	CMP_BIND(_cmp_pthsch->restart, _cmp_pthsch_restart);
	CMP_BIND(_cmp_pthsch->receiver, _cmp_pthsch_process);

	//Connecting


}

void _cmp_pthsch_deinit()
{
	_cmp_pholder_dtor();
}

//----------------------------------------------------------------------------------------------------
//------------------------- Activators  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void* _cmp_pthsch_restart()
{
	_cmp_pthsch_stop();
	_cmp_pthsch_start();

	return NULL;
}

void* _cmp_pthsch_start()
{
	connection_t* con;
	if(_cmp_pthsch->is_started == BOOL_TRUE){
		runtime_warning("Path scheduler is already started.");
		return NULL;
	}
	PRINTING_STARTING_SG(CMP_NAME_PTHSCH);
	int32_t index;
	dmap_rdlock_table_con();
	for(index = 0; dmap_itr_table_con(&index, &con) == BOOL_TRUE; ++index){
		_con2schtype[index] = con->scheduling_type;
	}
	dmap_rdunlock_table_con();

	dmap_rdlock_table_pth();
	path_t *path;
	pathring_t *ring;
	for(index = 0; dmap_itr_table_pth(&index, &path) == BOOL_TRUE; ++index){
		if(path->status != NTRT_REQ_STAT_OK){
			continue;
		}
		ring = pathring_add(&_rings[path->con_dmap_id], path);
		ring->counter = 0;
		ring->max = path->packet_max;
		ring->path_id = index;
	}
	_schedulers[PACKET_SCHEDULING_WINDOWED] = _windowed_pthselector;
	_schedulers[PACKET_SCHEDULING_BALANCED] = _windowed_pthselector;

	_cmp_pthsch->is_started = BOOL_TRUE;
	PRINTING_SG_IS_STARTED(CMP_NAME_PTHSCH);

	while(_cmp_pholder->puffer->count > 0){
		packet_t *packet = _cmp_pholder->supplier();
		_cmp_pthsch->receiver(packet);
	}
	return NULL;
}

void* _cmp_pthsch_stop()
{
	int32_t index;
	if(_cmp_pthsch->is_started == BOOL_FALSE){
		runtime_warning("Path scheduler is already stopped.");
		return NULL;
	}
	PRINTING_STOPPING_SG(CMP_NAME_PTHSCH);

	dmap_rdunlock_table_pth();

	for(index = 0; index < NTRT_MAX_CONNECTION_NUM; ++index){
		if(_rings[index] == NULL){
			continue;
		}
		DEBUGPRINT("Destroying path ring");
		pathring_dtor(_rings[index]);
		_rings[index] = NULL;
	}

	_cmp_pthsch->is_started = BOOL_FALSE;
	_schedulers[PACKET_SCHEDULING_WINDOWED] = _cmp_pholder->receiver;
	_schedulers[PACKET_SCHEDULING_BALANCED] = _cmp_pholder->receiver;
	PRINTING_SG_IS_STOPPED(CMP_NAME_PTHSCH);
	return NULL;
}

//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void _cmp_pthsch_process(packet_t *packet)
{
	CMP_DEF_THIS(cmp_pthsch_t, _cmp_pthsch);
	if(packet->path_out >= 0){
		this->send(packet);
		return;
	}
	int32_t scheduling_type = _con2schtype[packet->con_id];
	_packet_scheduler_t scheduler = _schedulers[scheduling_type];
	scheduler(packet);
}

void _windowed_pthselector(packet_t *packet)
{
	CMP_DEF_THIS(cmp_pthsch_t, _cmp_pthsch);
	pathring_t *ring = _rings[packet->con_id];
	if(ring == NULL){
		WARNINGPRINT("No available path for sending", 0);
		_cmp_pholder->receiver(packet);
		//disp_packet(packet);
		return;
	}

	packet->path_out = ring->path_id;
	//DEBUGPRINT("path_out: %d, counter: %d, max: %d", packet->path_out, ring->counter, ring->max);
	if(ring->max <= ++ring->counter){
		ring->counter = 0;
		_rings[packet->con_id] = ring->next;
	}
	//CMP_SEND(CMP_NAME_PTHSCH, this->send, packet);
	this->send(packet);
}


//----------------------------------------------------------------------------------------------------
//------------------------- Private stuff  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void _disp_packet(packet_t *packet)
{
	disp_packet(packet);
	DEBUGPRINT("Holder puffer is full, oldest packet is dropped");
}
