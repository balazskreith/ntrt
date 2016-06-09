#include "cmp_mprtpsender.h"
#include "cmp_defs.h"
#include "cmp_predefs.h"

//----------------------------------------------------------------------------------------------------
//------------------------- Private type definitions -------------------------------------
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//---------------------------- Component definitions ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void _cmp_mprtpsender_init();
static void _cmp_mprtpsender_deinit();
static void _cmp_mprtpsender_receiver(packet_t* packet);
CMP_DEF(, 			           //type of definitions
		cmp_mprtpsender_t,       	       //type of component
		CMP_MPRTP_SENDER, //name of the component
		 _cmp_mprtpsender,               //variable name of the component
		 cmp_mprtpsender_ctor,           //name of the constructor process implemented automatically
		 cmp_mprtpsender_dtor,           //name of the destructor process implemented automatically
		 _cmp_mprtpsender_init,          //name of the external process called after construction
		 __NO_TEST_FUNC_,      //name of the external process called after initialization
		 _cmp_mprtpsender_deinit         //name of the external process called before destruction
		);

CMP_DEF_GET_PROC(get_cmp_mprtpsender, cmp_mprtpsender_t, _cmp_mprtpsender);

//----------------------------------------------------------------------------------------------------
//------------------------- Private functions definitions -------------------------------------
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_mprtpsender_init()
{
	//constructions
	//Put here the subcomponent constructors. i.e.: _cmp_puffer_ctor();

	//Binding
	//Functions belongs to the component are bound functions. i.e:
	CMP_BIND(_cmp_mprtpsender->receiver, _cmp_mprtpsender_receiver);

	//Connecting
	//Functions

}

void  _cmp_mprtpsender_deinit()
{
	//destructing
}

//----------------------------------------------------------------------------------------------------
//------------------------- Component Processes -------------------------------------
//----------------------------------------------------------------------------------------------------
static volatile uint32_t _actual = 0;

void _cmp_mprtpsender_receiver(packet_t *packet)
{
	CMP_DEF_THIS(cmp_mprtpsender_t, _cmp_mprtpsender);
	uint32_t *end = packet->bytes + packet->length;
	end = _actual++;
	_actual &= 0xFFFFFFFF;
	packet->length += 4;
	this->send(packet);
}

//----------------------------------------------------------------------------------------------------
//------------------------- Private functions -------------------------------------
//----------------------------------------------------------------------------------------------------

