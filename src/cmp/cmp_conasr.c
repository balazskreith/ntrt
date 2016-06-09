#include "cmp_conasr.h"
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
#include "lib_interrupting.h"

static void _cmp_conasr_init();
static void _cmp_conasr_deinit();


CMP_DEF(, 			      	/*type of definitions*/ 									      \
		 cmp_conasr_t,       /*type of component*/ 											      \
		 CMP_NAME_CONASR,    /*name of the component*/ 										      \
		 _cmp_conasr,        /*variable name of the component*/  								  \
		 cmp_conasr_ctor,    /*name of the constructor function implemented automatically*/       \
		 cmp_conasr_dtor,    /*name of the destructor function implemented automatically*/        \
		 _cmp_conasr_init,   /*name of the external function called after construction*/          \
		 __NO_TEST_FUNC_,   /*name of the external function called after initialization*/         \
		 _cmp_conasr_deinit  /*name of the external function called before destruction*/          \
		);


CMP_DEF_GET_PROC(get_cmp_conasr, cmp_conasr_t, _cmp_conasr);

#define CMP_NAME_CONASR_PROCESS CMP_NAME_CONASR" process"
typedef struct _cmp_conasr_proc_struct_t
{
	packet_t*  (**demand)();
	void       (**send)(packet_t*);
}_cmp_conasr_proc_t;

static void _thr_conasr_proc_main(thread_t*);
CMP_THREAD(
		static,				/*type of declaration*/
		_cmp_conasr_proc_t,	/*type of the threaded component*/
	   conasr_proc,			/*unique name used for thread and component*/
	   CMP_NAME_CONASR,	/*name of the component*/
	   __CMP_NO_INIT_FUNC_,		/*name of the process initialize the component*/
	   __CMP_NO_DEINIT_FUNC_,	/*name of the process deinitialize the component*/
	   _cmp_conasr_start,	/*name of the process activate the thread*/
	   _cmp_conasr_stop,		/*name of the process deactivate the thread*/
	   _thr_conasr_proc_main	/*name of the process called in the thread*/
	   );


static void* _conasr_restart();
static void* _conasr_start();
static void* _conasr_stop();

//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_conasr_init()
{
	//constructions
	_cmp_conasr_proc_ctor();

	//Binding
	CMP_BIND(_cmp_conasr->start,     _conasr_start);
	CMP_BIND(_cmp_conasr->stop,      _conasr_stop);
	CMP_BIND(_cmp_conasr->restart,   _conasr_restart);

	//Connect
	CMP_CONNECT(_cmp_conasr_proc->demand, &_cmp_conasr->demand);
	CMP_CONNECT(_cmp_conasr_proc->send,   &_cmp_conasr->send);
}

void _cmp_conasr_deinit()
{

}

void* _conasr_start()
{
	dmap_rdlock_table_con();
	dmap_rdlock_table_net();
	_cmp_conasr_start();
	return NULL;
}

void* _conasr_stop()
{
	_cmp_conasr_stop();
	dmap_rdunlock_table_con();
	dmap_rdunlock_table_net();
	return NULL;
}

void* _conasr_restart()
{
	_conasr_stop();
	_conasr_start();
	return NULL;
}
//----------------------------------------------------------------------------------------------------
//---------------------------- Processes --------------------------------------------
//----------------------------------------------------------------------------------------------------

void _thr_conasr_proc_main(thread_t *thread)
{
	CMP_DEF_THIS(_cmp_conasr_proc_t, (_cmp_conasr_proc_t*) thread->arg);
	packet_t*  			  packet = NULL;
	int32_t        		  index;
	connection_t  		 *con = NULL;
	network_t     		 *net = NULL;
	bool_t         		  found = BOOL_FALSE;
	void                (*send)(packet_t*);
	packet_t*           (*demand)();

	send = *(this->send);
	demand = *(this->demand);

	do{
		packet = demand();
		set_mtime(&packet->mtime);
		//DEBUGPRINT("%p _thr_conasr_proc_main: %f", packet, diffmtime_fromnow(&packet->mtime));
		found = BOOL_FALSE;
		for(index = 0; dmap_itr_table_con(&index, &con) == BOOL_TRUE; ++index)
		{
			if((found = is_con_addr_matched(packet, con)) == BOOL_TRUE)
			{
				break;
			}
		}

		if(found == BOOL_TRUE)
		{

			packet->con_id = index;
			packet->connection = con;
			//CMP_SEND(CMP_NAME_CONASR, send, packet);
			send(packet);
			continue;
		}

		for(index = 0; dmap_itr_table_net(&index, &net) == BOOL_TRUE; ++index)
		{
			if((found = is_net_addr_matched(packet, net)) == BOOL_TRUE)
			{
				break;
			}
		}

		if(found == BOOL_TRUE)
		{

			packet->connection = dmap_get_con(net->con_dmap_id);
			packet->con_id = net->con_dmap_id;
			//CMP_SEND(CMP_NAME_CONASR, send, packet);
			send(packet);
			continue;
		}
	}
	while(thread->state == THREAD_STATE_RUN);
}

