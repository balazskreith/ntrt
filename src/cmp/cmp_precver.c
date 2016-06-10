#include "cmp_precver.h"
#include "dmap_sel.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "cmp_predefs.h"
#include "lib_descs.h"
#include "lib_puffers.h"
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
#include "lib_funcs.h"
#include "etc_ipsearch.h"
#include "etc_utils.h"
#include "cmp_defs.h"
#include "etc_endian.h"
//----------------------------------------------------------------------------------------------------
//---------------------------- Private type declarations ---------------------------------------------
//----------------------------------------------------------------------------------------------------
#define CMP_NAME_PCKSWITCHER "Packet switcher component"
typedef struct _cmp_pckswitcher_struct_t
{
	void      (*receiver)(packet_t*);
	void      (**send_diagpck)(packet_t*);
	void      (**send_cmdpck)(packet_t*);
	void      (**send_locpck)(packet_t*);
}_cmp_pckswitcher_t;

#define CMP_NAME_TUNHANDLER_CONASR "Connection assigner for tunnel handler"
typedef struct _cmp_conasr_struct_t
{
	void      (*receiver)(packet_t*);
	void      (*send2rout)(packet_t*);
}_cmp_conasr_t;

#define CMP_NAME_CONLISTENER "Connection listener component"
typedef struct _cmp_conlistener_struct_t
{
	int32_t              sockd;
	connection_t*        connection;
	int32_t              con_id;
	void               (*send2rout)(packet_t*); //send to the packet router
	void               (*send2writ)(packet_t*); //send to the packet router
	packet_t* 		   (*demand)();
}_cmp_conlistener_t;


#define CMP_NAME_TUNHANDLER "Tunnel handler component"
typedef struct _cmp_tunhandler_struct_t
{
	tunnel_t*        	 tunnel;
	int32_t              sockd;
	packet_t* 		   (*demand)();
	void               (*send2rout)(packet_t*); //send to the recycle
}_cmp_tunhandler_t;

//----------------------------------------------------------------------------------------------------
//---------------------------- Public definitions ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void  _cmp_precver_init();
static void  _cmp_precver_deinit();

CMP_DEF(, 			      		/*type of definitions*/ 										   \
		cmp_precver_t,       /*type of component*/ 										  	   \
		 CMP_NAME_PRECVER, /*name of the component*/ 										   \
		 _cmp_precver,        /*variable name of the component*/  							   \
		 cmp_precver_ctor,    /*name of the constructor function implemented automatically*/     \
		 cmp_precver_dtor,    /*name of the destructor function implemented automatically*/      \
		 _cmp_precver_init,   /*name of the external function called after construction*/        \
		 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
		 _cmp_precver_deinit  /*name of the external function called before destruction*/        \
		);																						   \

CMP_DEF_GET_PROC(get_cmp_precver, cmp_precver_t, _cmp_precver);

//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

static void _thr_conlistener_init(thread_t*);
static void _thr_conlistener_proc_main(thread_t*);
static void _cmp_conlistener_deinit(_cmp_conlistener_t*);
CMP_THREADS(static,          //type of declarations
		_cmp_conlistener_t,  //type of components
		conlistener,		 //unique name used for identifying generated variables and processes
		CMP_NAME_CONLISTENER,//name of the component running in the thread
		_conlisteners_ctor,  //name of the constructor process used for creating a thread chain
		_conlisteners_dtor,  //name of the destructor process used for disposing a thread chain
		_itr_conlisteners,   //name of the iterator process
		_thr_conlistener_init,  //name of the process initialize a thread
		__CMP_NO_INIT_MFUNC__,   //name of the process initialize a component
		__THR_NO_DEINIT_PROC__, //name of the process deinitialize a thread
		_cmp_conlistener_deinit,//name of the process deinitialize a component
		_thr_conlistener_proc_main, //name of the thread process called after initialization
		connection_t,               //name of the type used for activating different thread
		connection,                 //name of the attribute inside the component used for pointing to the activator type
		_conlistener_start,         //name of the process used creating and activating a component
		_conlistener_stop           //name of the process used for deactivating and disposing a threaded component
		)

static void _cmp_tunhandler_init();
static void _cmp_tunhandler_deinit();
static void _thr_tunhandler_cmdrec_proc(thread_t*);
//static void _cmp_tunhandler_writer(packet_t*);
CMP_THREAD(
		static,				/*type of declaration*/
	   _cmp_tunhandler_t,	/*type of the threaded component*/
	   tunhandler,			/*unique name used for thread and component*/
	   CMP_NAME_TUNHANDLER,	/*name of the component*/
	   _cmp_tunhandler_init,		/*name of the process initialize the component*/
	   _cmp_tunhandler_deinit,	/*name of the process deinitialize the component*/
	   _tunhandler_start,	/*name of the process activate the thread*/
	   _tunhandler_stop,		/*name of the process deactivate the thread*/
	   _thr_tunhandler_cmdrec_proc	/*name of the process called in the thread*/
	   );



#define CMP_PUFFER_RECYCLE_SIZE 64
#define CMP_NAME_PRECYCLE "Packet Recycle component"
CMP_DEF_RECPUFFER(
		static,				/*type of the declarations*/
		packet_t,			/*name of the type of the items*/
		prov_packet,		/*name of the process used for constructing an item if the puffer is empty.*/
		disp_packet,	/*name of the process used for deconstruting an item if the puffer is full.*/
		clean_packet,		/*name of the process clear the item when the recycle receives it*/
		CMP_NAME_PRECYCLE, 	/*name of the recycle puffer*/
		_cmp_recycle,		/*name of the variable used for referencing to the recycle*/
		CMP_PUFFER_RECYCLE_SIZE, /*length of the recycle puffer*/
		_cmp_recycle_ctor,	/*name of the constructor process for this recycle*/
		_cmp_recycle_dtor	/*name of the destructor process for this recycle*/
		);

static void _cmp_pckswitcher_process(packet_t*);
CMP_DEF(static, 			      /*type of definitions*/ 										   \
		_cmp_pckswitcher_t,       /*type of component*/ 										  	   \
		 CMP_NAME_PCKSWITCHER, /*name of the component*/ 										   \
		 _cmp_pckswitcher,        /*variable name of the component*/  							   \
		 _cmp_pckswitcher_ctor,    /*name of the constructor function implemented automatically*/     \
		 _cmp_pckswitcher_dtor,    /*name of the destructor function implemented automatically*/      \
		 __CMP_NO_INIT_FUNC_,   /*name of the external function called after construction*/        \
		 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
		 __CMP_NO_DEINIT_FUNC_  /*name of the external function called before destruction*/        \
		);


static void _setup_packet(packet_t *packet);
//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------


void  _cmp_precver_init()
{
	int32_t   index, count;

	//constructions
	_cmp_recycle_ctor();
	_cmp_pckswitcher_ctor();
	_conlisteners_ctor();

	//Binding
	CMP_BIND(_cmp_precver->start,        _conlistener_start);
	CMP_BIND(_cmp_precver->stop,         _conlistener_stop);
	CMP_BIND(_cmp_pckswitcher->receiver, _cmp_pckswitcher_process);

	//Connecting
	CMP_CONNECT(_cmp_precver->receiver,             _cmp_recycle->receiver);
	CMP_CONNECT(_cmp_pckswitcher->send_cmdpck,     &_cmp_precver->send_cmdpck);
	CMP_CONNECT(_cmp_pckswitcher->send_diagpck,    &_cmp_precver->send_diagpck);
	CMP_CONNECT(_cmp_pckswitcher->send_locpck,     &_cmp_precver->send_locpck);

	//other
	count = _cmp_recycle->puffer->length >> 1;
	for(index = 0; index < count; ++index){
		_cmp_recycle->receiver(make_packet());
	}

}

void  _cmp_precver_deinit()
{
	_tunhandler_stop();

	_conlisteners_dtor();
	_cmp_recycle_dtor();
	_cmp_pckswitcher_dtor();
}


void _thr_conlistener_init(thread_t *thread)
{
	_cmp_conlistener_t     *cmp = (_cmp_conlistener_t*) thread->arg;
	connection_t           *con = cmp->connection;

	dmap_rdlock_table_con();
	cmp->con_id = dmap_getid_bycon(con);
	cmp->sockd = con->sockd;
	dmap_rdunlock_table_con();

	//connect
	CMP_CONNECT(cmp->demand,     _cmp_recycle->supplier);
	CMP_CONNECT(cmp->send2rout,  _cmp_pckswitcher->receiver);

	if(_cmp_tunhandler == NULL){
		_tunhandler_start();
	}

	CMP_CONNECT(cmp->send2writ,  _cmp_precver->send_datpck);
}

void _cmp_conlistener_deinit(_cmp_conlistener_t *this)
{


}


void _cmp_tunhandler_init()
{
	CMP_DEF_THIS(_cmp_tunhandler_t, _cmp_tunhandler);
	tunnel_t     *tunnel;
	int32_t       index;

	dmap_rdlock_table_tun();
	for(index = 0; dmap_itr_table_tun(&index, &tunnel) == BOOL_TRUE; ++index) break;
	this->tunnel = tunnel;

	this->sockd = tunnel->sockd;
	dmap_rdunlock_table_tun();

	//bind

	//connect
	CMP_CONNECT(_cmp_tunhandler->demand,     _cmp_recycle->supplier);
	CMP_CONNECT(_cmp_tunhandler->send2rout,  _cmp_pckswitcher->receiver);
}

void _cmp_tunhandler_deinit()
{

}


//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _thr_conlistener_proc_main(thread_t *thread)
{
	CMP_DEF_THIS(_cmp_conlistener_t, (_cmp_conlistener_t*) thread->arg);
	packet_t            *packet;
	int32_t              sockd;
	struct sockaddr_in6  client;
	int32_t              client_size = sizeof client;

	sockd = this->sockd;
	do{
		//CMP_DEMAND(CMP_NAME_CONLISTENER, packet, this->demand);
		packet = this->demand();

		packet->length = sysio->udpsock_recv(sockd, packet->bytes, NTRT_PACKET_LENGTH, 0,
						(struct sockaddr *)&client, &client_size);
		/*
		DEBUGPRINT("Packet source: %x.%x.%x.%x:ver:%d, port:%d<->%d",
						client.sin6_addr.__in6_u.__u6_addr32[0],
						client.sin6_addr.__in6_u.__u6_addr32[1],
						client.sin6_addr.__in6_u.__u6_addr32[2],
						client.sin6_addr.__in6_u.__u6_addr32[3],
						packet->bytes[0], ntohs(client.sin6_port), client.sin6_port);
		/**/
		if(packet->bytes[0] < 0xA0){
			this->send2writ(packet);
			//CMP_SEND(CMP_NAME_CONLISTENER, this->send2writ, packet);
			continue;
		}
		packet->connection = this->connection;
		packet->con_id = this->con_id;
		memcpy(packet->source, &client.sin6_addr.__in6_u.__u6_addr32, SIZE_IN6ADDR);
		//DEBUGPRINT("Received, %d. %x:%d", packet->bytes[0], packet->source[3], ntohs(client.sin6_port));
		//CMP_SEND(CMP_NAME_CONLISTENER, this->send2rout, packet);
		this->send2rout(packet);
	}
	while(thread->state == THREAD_STATE_RUN);
}

void _thr_tunhandler_cmdrec_proc(thread_t *thread)
{
	CMP_DEF_THIS(_cmp_tunhandler_t, (_cmp_tunhandler_t*) thread->arg);
	packet_t            *packet;
	int32_t              sockd;
	struct sockaddr_in6  client;
	int32_t              client_size = sizeof client;

	sockd = this->sockd;
	do{
		//CMP_DEMAND(CMP_NAME_TUNHANDLER, packet, this->demand);
		packet = this->demand();
		packet->length = sysio->udpsock_recv(sockd, packet->bytes, NTRT_PACKET_LENGTH, 0,
				(struct sockaddr *)&client, &client_size);

		memcpy(packet->source, &client.sin6_addr.__in6_u.__u6_addr32, SIZE_IN6ADDR);

		/*
		DEBUGPRINT("Packet source: %x.%x.%x.%x(%d.%d.%d.%d):ver:%d, port:%d<->%d",
								client.sin6_addr.__in6_u.__u6_addr32[0],
								client.sin6_addr.__in6_u.__u6_addr32[1],
								client.sin6_addr.__in6_u.__u6_addr32[2],
								client.sin6_addr.__in6_u.__u6_addr32[3],
								(byte_t) client.sin6_addr.__in6_u.__u6_addr32[3],
								(byte_t)(client.sin6_addr.__in6_u.__u6_addr32[3] >> 8),
								(byte_t)(client.sin6_addr.__in6_u.__u6_addr32[3] >> 16),
								(byte_t)(client.sin6_addr.__in6_u.__u6_addr32[3] >> 24),
								packet->bytes[0], ntohs(client.sin6_port), client.sin6_port);
		/**/
		//DEBUGPRINT("Received: %d",packet->bytes[4]);
		//CMP_SEND(CMP_NAME_TUNHANDLER" cmd receiver", this->send2conasr, packet);
		set_packet_for_receiving(packet);
		this->send2rout(packet);
	}
	while(thread->state == THREAD_STATE_RUN);
}


void _cmp_pckswitcher_process(packet_t *packet)
{
	CMP_DEF_THIS(_cmp_pckswitcher_t, _cmp_pckswitcher);
	void       (*send_diagpck)(packet_t*);
	void       (*send_cmdpck)(packet_t*);
	void       (*send_locpck)(packet_t*);

	send_diagpck = *(this->send_diagpck);
	send_cmdpck = *(this->send_cmdpck);
	send_locpck = *(this->send_locpck);
	_setup_packet(packet);

	if(packet->local == BOOL_TRUE){
		//Mpt local send the command in host order byte format
		set_packet_for_receiving(packet);
		send_locpck(packet);
		return;
	}
	//CMP_RECEIVE(CMP_NAME_PCKSWITCHER, packet);
	switch(packet->bytes[0])
	{
	case NTRT_REQ_CMD_KEEPALIVE:
	case NTRT_REQ_CMD_ECHO:
		if(packet->connection == NULL){
			ERRORPRINT("Diagnostic packet can not exists without connection");
			break;
		}
		//CMP_SEND(CMP_NAME_PCKSWITCHER, send_diagpck, packet);
		send_diagpck(packet);
		break;

	case NTRT_REQ_CMD_CONNECTION_SEND:
	case NTRT_REQ_CMD_NETWORK_SEND:
	case NTRT_REQ_CMD_PATH_SEND:
	case NTRT_REQ_CMD_P_STATUS_CHANGE:
		//CMP_SEND(CMP_NAME_PCKSWITCHER, send_cmdpck, packet);
		send_cmdpck(packet);
		break;

	default:
		runtime_warning("Unknown request with id: %x", packet->bytes[0]);
		packet_dtor(packet);
	}
}

//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  -----------------------------------------------------------
//----------------------------------------------------------------------------------------------------



void _setup_packet(packet_t *packet)
{
	connection_t *con;
	network_t    *net;
	int32_t       index;
	//CMP_RECEIVE(CMP_NAME_TUNHANDLER_CONASR, packet);
	if(check_loopback((char_t *)packet->source) > 0){
		DEBUGPRINT("Packet is local");
		packet->local = BOOL_TRUE;
		return;
	}
	dmap_rdlock_table_con();
	for(index = 0; dmap_itr_table_con(&index, &con) == BOOL_TRUE; ++index)
	{
		if(is_con_addr_matched(packet, con) == BOOL_TRUE)
		{
			dmap_rdunlock_table_con();
			packet->connection = con;
			packet->con_id = index;
			//CMP_SEND(CMP_NAME_TUNHANDLER_CONASR, this->send2rout, packet);
			return;
		}
	}

	dmap_rdlock_table_net();
	for(index = 0; dmap_itr_table_net(&index, &net) == BOOL_TRUE; ++index)
	{
		if(is_net_addr_matched(packet, net) == BOOL_TRUE)
		{
			packet->connection = dmap_get_con(net->con_dmap_id);
			packet->con_id = net->con_dmap_id;
			dmap_rdunlock_table_net();
			dmap_rdunlock_table_con();
			//CMP_SEND(CMP_NAME_TUNHANDLER_CONASR, this->send2rout, packet);
			return;
		}
	}
	ERRORPRINT("No connection found for packet, new connection needs to be setup");
	dmap_rdunlock_table_net();
	dmap_rdunlock_table_con();
	//DEBUGPRINT("A packet is dropped");
}
