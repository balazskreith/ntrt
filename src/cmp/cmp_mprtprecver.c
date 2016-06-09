#include "cmp_mprtprecver.h"
#include "cmp_defs.h"
#include "cmp_predefs.h"
#include "lib_heap.h"
#include "dmap.h"

//----------------------------------------------------------------------------------------------------
//------------------------- Private type definitions -------------------------------------
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//---------------------------- Component definitions ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void _cmp_mprtprecver_init();
static void _cmp_mprtprecver_deinit();
static void _cmp_mprtprecver_receiver(packet_t* packet);
CMP_DEF( , 			           //type of definitions
		cmp_mprtprecver_t,       	       //type of component
		CMP_MPRTP_RECEIVER, //name of the component
		 _cmp_mprtprecver,               //variable name of the component
		 cmp_mprtprecver_ctor,           //name of the constructor process implemented automatically
		 cmp_mprtprecver_dtor,           //name of the destructor process implemented automatically
		 _cmp_mprtprecver_init,          //name of the external process called after construction
		 __NO_TEST_FUNC_,      //name of the external process called after initialization
		 _cmp_mprtprecver_deinit         //name of the external process called before destruction
		);

CMP_DEF_GET_PROC(get_cmp_mprtprecver, cmp_mprtprecver_t, _cmp_mprtprecver);

static struct Heap _heap;
static uint32_t _stored_bytes = 0;
static tunnel_t *_tunnel;
static volatile bool_t _playout_happened = BOOL_FALSE;
//----------------------------------------------------------------------------------------------------
//------------------------- Private functions definitions -------------------------------------
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_mprtprecver_init()
{
	int32_t index;
	//constructions
	//Put here the subcomponent constructors. i.e.: _cmp_puffer_ctor();
	_heap_init(&_heap);
	dmap_rdlock_table_tun();
	for(index = 0; dmap_itr_table_tun(&index, &_tunnel) == BOOL_TRUE; ++index){break;}
	dmap_rdunlock_table_tun();

	//Binding
	//Functions belongs to the component are bound functions. i.e:
	CMP_BIND(_cmp_mprtprecver->receiver, _cmp_mprtprecver_receiver);

	//Connecting
	//Functions

}

void  _cmp_mprtprecver_deinit()
{
	//destructing
	heap_term(&_heap);
}

//----------------------------------------------------------------------------------------------------
//------------------------- Component Processes -------------------------------------
//----------------------------------------------------------------------------------------------------

void _cmp_mprtprecver_receiver(packet_t *packet)
{
	CMP_DEF_THIS(cmp_mprtprecver_t, _cmp_mprtprecver);

	memcpy(&packet->seq_num, packet->bytes + packet->length - 4, 4);
	packet->length -= 4;
	_heap_push(&_heap, packet);
	_stored_bytes += packet->length;
	if(_stored_bytes < _tunnel->playout_cut_size){
		_playout_happened = BOOL_FALSE;
		return;
	}

	do{
		packet = heap_front(&_heap);
		_stored_bytes -= packet->length;
		_heap_pop(&_heap);
		this->send(packet);
	}while(_stored_bytes > 0 && _heap.count > 0);

	_playout_happened = BOOL_TRUE;
}

//----------------------------------------------------------------------------------------------------
//------------------------- Private functions -------------------------------------
//----------------------------------------------------------------------------------------------------

