#include "cmp_screener.h"
#include "etc_utils.h"
#include "lib_tors.h"
#include "dmap.h"
#include "inc_mtime.h"
#include "lib_funcs.h"
#include "inc_predefs.h"
#include "inc_texts.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "lib_debuglog.h"
#include "lib_descs.h"
#include "cmp_predefs.h"
#include "lib_descs.h"
#include "sys_ctrls.h"
#include "lib_puffers.h"

static void _cmp_screener_init();
static void _cmp_screener_deinit();
static void* _cmp_screener_start();
static void* _cmp_screener_stop();
#define CMP_NAME_SCREENER "Screener component"
CMP_DEF(, 			    /*type of definitions*/
		cmp_screener_t,          /*type of component*/
		CMP_NAME_SCREENER,   /*name of the component*/
		 _cmp_screener,           /*variable name of the component*/
		 cmp_screener_ctor,      /*name of the constructor function implemented automatically*/
		 cmp_screener_dtor,      /*name of the destructor function implemented automatically*/
		 _cmp_screener_init,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 _cmp_screener_deinit      /*name of the external function called before destruction*/
		);

CMP_DEF_GET_PROC(get_cmp_screener, cmp_screener_t, _cmp_screener);
//----------------------------------------------------------------------------------------------------
//---------------------------- Subcomponents type definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

typedef struct _cmp_router_struct_t
{
	void    (*receiver)(packet_t*);
	void    (*send_keepalive)(packet_t*);
}_cmp_router_t;

typedef struct _cmp_checker_struct_t
{
	void         (*check_keepalives)();
	void         (*check_interfaces)();
}_cmp_checker_t;

typedef struct _cmp_keepalives_struct_t
{
	time_t         sent[NTRT_MAX_PATH_NUM];
	time_t         received[NTRT_MAX_PATH_NUM];
	time_t         commanded[NTRT_MAX_PATH_NUM];
	packet_t*    (*demand_pck)();
	spin_t*        spin;
	void         (*receiver)(packet_t*);
	void         (*checker)();
	void         (**send_pck)(packet_t*);
	void         (**send_cmd)(command_t*);
	void         (**send_req)(request_t*);
}_cmp_keepalives_t;


typedef struct _cmp_interfaces_struct_t
{
	void        (*checker)();
}_cmp_interfaces_t;

//----------------------------------------------------------------------------------------------------
//---------------------------- Subcomponents declarations --------------------------------------------
//----------------------------------------------------------------------------------------------------

#define CMP_NAME_CHECKER "Screener checker component"
static void _thr_checker_main_proc(thread_t*);
CMP_THREAD(
		static,				/*type of declaration*/
		_cmp_checker_t,	/*type of the threaded component*/
	   checker,		/*unique name used for thread and component*/
	   CMP_NAME_CHECKER,	/*name of the component*/
	   __CMP_NO_INIT_FUNC_,		/*name of the process initialize the component*/
	   __CMP_NO_DEINIT_FUNC_,	/*name of the process deinitialize the component*/
	   _checker_start,	/*name of the process activate the thread*/
	   _checker_stop,		/*name of the process deactivate the thread*/
	   _thr_checker_main_proc	/*name of the process called in the thread*/
	   );

static void _cmp_keepalives_receiver(packet_t*);
static void _cmp_keepalives_checker();
#define CMP_NAME_KEEPALIVES "Keepalives handler component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_keepalives_t,          /*type of component*/
		CMP_NAME_KEEPALIVES,   /*name of the component*/
		 _cmp_keepalives,           /*variable name of the component*/
		 _cmp_keepalives_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_keepalives_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);

static void _cmp_interfaces_checker();
#define CMP_NAME_INTERFACES "Interfaces checker component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_interfaces_t,          /*type of component*/
		CMP_NAME_INTERFACES,   /*name of the component*/
		 _cmp_interfaces,           /*variable name of the component*/
		 _cmp_interfaces_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_interfaces_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);


static void _cmp_router_process(packet_t*);
#define CMP_NAME_ROUTER "Screeener packet router component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_router_t,          /*type of component*/
		CMP_NAME_ROUTER,   /*name of the component*/
		 _cmp_router,           /*variable name of the component*/
		 _cmp_router_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_router_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);


#define CMP_PUFFER_RECYCLE_SIZE 64
#define CMP_NAME_PRECYCLE "Packet Recycle component (Screener component)"
static void _clean_packet(packet_t*);
CMP_DEF_RECPUFFER(
		static,				/*type of the declarations*/
		packet_t,			/*name of the type of the items*/
		make_packet,		/*name of the process used for constructing an item if the puffer is empty.*/
		packet_dtor,		/*name of the process used for deconstruting an item if the puffer is full.*/
		_clean_packet,		/*name of the process clear the item when the recycle receives it*/
		CMP_NAME_PRECYCLE, 	/*name of the recycle puffer*/
		_cmp_recycle,		/*name of the variable used for referencing to the recycle*/
		CMP_PUFFER_RECYCLE_SIZE, /*length of the recycle puffer*/
		_cmp_recycle_ctor,	/*name of the constructor process for this recycle*/
		_cmp_recycle_dtor	/*name of the destructor process for this recycle*/
		)
//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _cmp_screener_init()
{
	int32_t index;
	_cmp_keepalives_ctor();
	_cmp_interfaces_ctor();
	_cmp_checker_ctor();
	_cmp_router_ctor();
	_cmp_recycle_ctor();

	_cmp_keepalives->spin = spin_ctor();
	for(index = 0; index < NTRT_MAX_PATH_NUM; ++index){
		_cmp_keepalives->received[index] = time(NULL);
		_cmp_keepalives->sent[index] = time(NULL);
		_cmp_keepalives->commanded[index] = time(NULL);
	}

	//Bind
	CMP_BIND(_cmp_screener->start,   _cmp_screener_start);
	CMP_BIND(_cmp_screener->stop,    _cmp_screener_stop);

	CMP_BIND(_cmp_keepalives->receiver, _cmp_keepalives_receiver);
	CMP_BIND(_cmp_keepalives->checker,  _cmp_keepalives_checker);
	CMP_BIND(_cmp_interfaces->checker,  _cmp_interfaces_checker);
	CMP_BIND(_cmp_router->receiver,     _cmp_router_process);

	//Connect
	CMP_CONNECT(_cmp_screener->receiver,        _cmp_router->receiver);
	CMP_CONNECT(_cmp_checker->check_keepalives, _cmp_keepalives->checker);
	CMP_CONNECT(_cmp_checker->check_interfaces, _cmp_interfaces->checker);
	CMP_CONNECT(_cmp_router->send_keepalive,    _cmp_keepalives->receiver);

	CMP_CONNECT(_cmp_keepalives->demand_pck,    _cmp_recycle->supplier);
	CMP_CONNECT(_cmp_keepalives->send_cmd,  &_cmp_screener->send_cmd);
	CMP_CONNECT(_cmp_keepalives->send_pck,  &_cmp_screener->send_pck);
	CMP_CONNECT(_cmp_keepalives->send_req,  &_cmp_screener->send_req);
}


void _cmp_screener_deinit()
{
	_cmp_keepalives_dtor();
	_cmp_interfaces_dtor();
	_cmp_checker_dtor();
	_cmp_router_dtor();
	_cmp_recycle_dtor();

	spin_dtor(_cmp_keepalives->spin);
}

//----------------------------------------------------------------------------------------------------
//------------------------- Activator  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void* _cmp_screener_start()
{
	_checker_start();
	return NULL;
}

void* _cmp_screener_stop()
{
	_checker_stop();
	return NULL;
}

//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _thr_checker_main_proc(thread_t *thread)
{
	CMP_DEF_THIS(_cmp_checker_t, (_cmp_checker_t*) thread->arg);
	void                 (*send_req)(request_t*);
	void                 (*send_cmd)(command_t*);
	uint64_t tick = 0;

	send_req = *(_cmp_keepalives->send_req);
	send_cmd = *(_cmp_keepalives->send_cmd);

	do
	{
		++tick;
		thread_sleep(1000);
		this->check_keepalives();
		this->check_interfaces();
		continue;
		if(tick == 15){
			send_cmd(make_cmd_path_down(dmap_get_pth(0)));
			//send_req(NTRT_REQ_CMD_P_STATUS_CHANGE, NTRT_REQ_STAT_PATH_DOWN, dmap_get_con(0), dmap_get_pth(0));
		}
	}while(thread->state == THREAD_STATE_RUN);
}

void _cmp_router_process(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_router_t, _cmp_router);
	switch(packet->bytes[0])
	{
	case NTRT_REQ_CMD_KEEPALIVE:
		this->send_keepalive(packet);
		break;
	}
}

void _cmp_interfaces_checker()
{
	CMP_DEF_THIS(_cmp_interfaces_t, _cmp_interfaces);
	int32_t index;
	interface_t* interface;
}

void _cmp_keepalives_checker()
{
	CMP_DEF_THIS(_cmp_keepalives_t, _cmp_keepalives);
	command_t             *command;
	packet_t              *packet;
	path_t				  *path;
	connection_t          *con;
	char_t                 text[255];
	int32_t                con_index,path_index;
	time_t                 now;
	double                 elapsed;
	void                 (*send_pck)(packet_t*);
	void                 (*send_cmd)(command_t*);
	void                 (*send_req)(request_t*);

	send_pck = *(this->send_pck);
	send_cmd = *(this->send_cmd);
	send_req = *(this->send_req);
	now = time(NULL);

	dmap_rdlock_table_con();
	dmap_rdlock_table_pth();
	for(con_index = 0; dmap_itr_table_con(&con_index, &con) == BOOL_TRUE; ++con_index)
	{
		if(con->keepalive == 0){
			continue;
		}
		spin_lock(this->spin);
		for(path_index = 0; dmap_itr_table_pth(&path_index, &path) == BOOL_TRUE; ++path_index){
			if(path->con_dmap_id != con_index){
				continue;
			}
			elapsed = difftime(now, this->received[path_index]);
			if(con->deadtimer < elapsed && path->status == NTRT_REQ_STAT_OK){
				if(difftime(now, this->commanded[path_index]) < 10){
					continue;
				}
				command = make_event_cmd(NTRT_EVENT_PATH_DOWN, path);
				command->request = make_request_path_down(path);
				set_ipstr(text, 255, path->ip_remote, path->ip_version);
				INFOPRINT("Keepalive is not received from %s", text);
				send_cmd(command);
				//send_req(NTRT_REQ_CMD_P_STATUS_CHANGE, NTRT_REQ_STAT_PATH_DOWN, con, path);
				_cmp_keepalives->commanded[path_index] = now;
				continue;
			}
			elapsed = difftime(now, this->sent[path_index]);
			if(elapsed < con->keepalive || path->status != NTRT_REQ_STAT_OK){
				continue;
			}
			//CMP_DEMAND(CMP_NAME_CHECKER, packet, this->demand_pck);
			packet = this->demand_pck();
			packet->bytes[0] = NTRT_REQ_CMD_KEEPALIVE;
			packet->bytes[5] = path->ip_version;
			memcpy(&packet->bytes[8], con->ip_local, SIZE_IN6ADDR);
			memcpy(&packet->bytes[24], con->ip_remote, SIZE_IN6ADDR);
			packet->connection = con;
			packet->path_out = path_index;
			packet->length = 128;

			//CMP_SEND(CMP_NAME_CHECKER, send_pck, packet);
			send_pck(packet);
			this->sent[path_index] = time(NULL);
		}
		spin_unlock(this->spin);
	}
	dmap_rdunlock_table_con();
	dmap_rdunlock_table_pth();
}

void _cmp_keepalives_receiver(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_keepalives_t, _cmp_keepalives);

	command_t             *command;
	path_t				  *path;
	connection_t          *con;
	char_t                 text[255];
	int32_t                con_index,path_index;
	void                  (*send_cmd)(command_t*);
	void                  (*send_req)(request_t*);
	time_t                now;

	//CMP_RECEIVE(CMP_NAME_KEEPALIVES, packet);

	send_cmd = *(this->send_cmd);
	send_req = *(this->send_req);
	now = time(NULL);

	dmap_rdlock_table_con();
	dmap_rdlock_table_pth();
	for(con_index = 0; dmap_itr_table_con(&con_index, &con) == BOOL_TRUE; ++con_index)
	{
		if(con->keepalive == 0){
			continue;
		}
		spin_lock(this->spin);
		for(path_index = 0; dmap_itr_table_pth(&path_index, &path) == BOOL_TRUE; ++path_index){
			if(memcmp(packet->source, path->ip_remote, SIZE_IN6ADDR) != 0){
				continue;
			}
			if(path->status == NTRT_REQ_STAT_PATH_DOWN){
				if(difftime(now, this->commanded[path_index]) < 10){
					continue;
				}
				command = make_event_cmd(NTRT_EVENT_PATH_UP, path);
				command->request = make_request_path_up(path);
				set_ipstr(text, 255, path->ip_remote, path->ip_version);
				INFOPRINT("Keepalive is received from %s", text);
				send_cmd(command);
				//send_req(NTRT_REQ_CMD_P_STATUS_CHANGE, NTRT_REQ_STAT_OK, con, path);
				_cmp_keepalives->commanded[path_index] = now;
			}
			this->received[path_index] = now;
			logging("Keepalive received from: %X:%X:%X:%X",
					path->ip_remote[0], path->ip_remote[1],
					path->ip_remote[2], path->ip_remote[3]);
			break;
		}
		spin_unlock(this->spin);
	}
	dmap_rdunlock_table_con();
	dmap_rdunlock_table_pth();
}

//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  -----------------------------------------------------------
//----------------------------------------------------------------------------------------------------


void _clean_packet(packet_t *packet)
{
	//memset(packet->bytes, 0, NTRT_PACKET_LENGTH);
	packet->connection = NULL;
	packet->path_out = -1;
}
