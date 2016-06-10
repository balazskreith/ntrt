#include "cmp_psender.h"
#include "dmap_sel.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "cmp_predefs.h"
#include "lib_descs.h"
#include "lib_puffers.h"
#include "lib_threading.h"
#include <time.h>
#include <stdlib.h>
#include "inc_mtime.h"
#include "etc_utils.h"
#include <stdio.h>
#include "inc_texts.h"
#include "inc_unistd.h"
#include "inc_inet.h"
#include "inc_predefs.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "inc_io.h"
#include "lib_tors.h"
#include "etc_endian.h"
#include "lib_interrupting.h"


//----------------------------------------------------------------------------------------------------
//---------------------------- Public function definitions -------------------------------------------
//----------------------------------------------------------------------------------------------------
static void  _cmp_psender_init();
static void  _cmp_psender_deinit();
CMP_DEF(, 			      		/*type of definitions*/ 										   \
		cmp_psender_t,       /*type of component*/ 										  	   \
		 CMP_NAME_PSENDER,    /*name of the component*/ 										   \
		 _cmp_psender,        /*variable name of the component*/  							   \
		 cmp_psender_ctor,    /*name of the constructor function implemented automatically*/     \
		 cmp_psender_dtor,    /*name of the destructor function implemented automatically*/      \
		 _cmp_psender_init,   /*name of the external function called after construction*/        \
		 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
		 _cmp_psender_deinit  /*name of the external function called before destruction*/        \
		);																						   \

CMP_DEF_GET_PROC(get_cmp_psender, cmp_psender_t, _cmp_psender);


static void* _sender_start();
static void* _sender_stop();
static void* _sender_restart();
static void _refresh_pinfos();

static void _cmp_datsender_sender(packet_t*);
static void _cmp_cmdsender_process(packet_t*);

typedef struct _pinfo_struct_t
{
	int32_t              dat_sockd;
	struct sockaddr_in6  peer;
	bool_t               available;
}pinfo_t;

static pinfo_t _pinfos[NTRT_MAX_PATH_NUM];

void  _cmp_psender_init()
{
	int32_t index;
	//constructions

	//Binding
	CMP_BIND(_cmp_psender->start, _sender_start);
	CMP_BIND(_cmp_psender->stop, _sender_stop);
	CMP_BIND(_cmp_psender->restart, _sender_restart);
	CMP_BIND(_cmp_psender->datpck_receiver, _cmp_datsender_sender);
	CMP_BIND(_cmp_psender->cmdpck_receiver, _cmp_cmdsender_process);

	for(index = 0; index < NTRT_MAX_PATH_NUM; ++index){
		_pinfos[index].available = BOOL_FALSE;
	}

}

void _cmp_psender_deinit()
{

}


void* _sender_start()
{
	_refresh_pinfos();
	PRINTING_SG_IS_STARTED(CMP_NAME_PSENDER);
	return NULL;
}

void* _sender_restart()
{
	_sender_stop();
	_sender_start();
	return NULL;
}

void* _sender_stop()
{

	PRINTING_SG_IS_STOPPED(CMP_NAME_PSENDER);
	return NULL;
}


//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _cmp_datsender_sender(packet_t *packet)
{
	CMP_DEF_THIS(cmp_psender_t, _cmp_psender);
	//DEBUGPRINT("Path out: %d", packet->path_out);
	//send datapacket
	if(_pinfos[packet->path_out].available == BOOL_FALSE){
		int32_t index = 0;
		WARNINGPRINT("Desired path is not available for sending", 0);
		for(packet->path_out = 0; packet->path_out < NTRT_MAX_PATH_NUM && _pinfos[packet->path_out].available == BOOL_FALSE; ++packet->path_out);
		if( index == NTRT_MAX_PATH_NUM){
			WARNINGPRINT("No path available for sending.");
			this->send(packet);
			return;
		}
	}

	sysio->udpsock_send(_pinfos[packet->path_out].dat_sockd, (const char*)packet->bytes, packet->length, 0,
		(struct sockaddr*) &(_pinfos[packet->path_out].peer), sizeof(struct sockaddr_in6));
	/*
	DEBUGPRINT("Packet sent out to: %X via: %X:%d",
			packet->header->ipv4.dst_addr.value,
			_pinfos[packet->path_out].peer.sin6_addr.__in6_u.__u6_addr32[3],
			_pinfos[packet->path_out].peer.sin6_port);
	/**/
	this->send(packet);
}

void _cmp_cmdsender_process(packet_t *packet)
{
	CMP_DEF_THIS(cmp_psender_t, _cmp_psender);
	int32_t              index;
	path_t               *path;
	connection_t         *con;
	struct sockaddr_in6   peer;
	tunnel_t             *tun;

	dmap_rdlock_table_con();
	dmap_rdlock_table_pth();
	con = dmap_get_con(packet->con_id);
	for(index = 0; dmap_itr_table_pth(&index, &path) == BOOL_TRUE; ++index)
	{
		if( path->con_dmap_id == packet->con_id && path->status == NTRT_REQ_STAT_OK)
		{
			break;
		}
	}
	dmap_rdunlock_table_pth();
	if(index == NTRT_MAX_PATH_NUM){
		ERRORPRINT("Packet sender: No available path for sending command at con: %s", con->name);
		dmap_rdunlock_table_con();
		return;
	}
	dmap_rdunlock_table_con();

	dmap_rdlock_table_tun();
	tun = dmap_get_tun_bycon(con, NULL);
	if(tun == NULL){
		ERRORPRINT("Tunnel is not exists.");
		dmap_rdunlock_table_tun();
		return;
	}

	setup_peer(&peer, (byte_t*)path->ip_remote, con->cmd_port_remote);
	set_packet_for_sending(packet);
	sysio->udpsock_send(tun->sockd, (const char*)packet->bytes, packet->length, 0,
		(struct sockaddr*) &peer, sizeof(struct sockaddr_in6));
	dmap_rdunlock_table_tun();
	this->send(packet);
}



void _refresh_pinfos()
{
	path_t       *path;
	connection_t *con;
	int32_t       index;

	dmap_rdlock_table_pth();
	dmap_rdlock_table_con();
	for(index = 0; dmap_itr_table_pth(&index, &path) == BOOL_TRUE; ++index)
	{
		if(path->status != NTRT_REQ_STAT_OK){
			_pinfos[index].available = BOOL_FALSE;
			continue;
		}
		con = dmap_get_con(path->con_dmap_id);
		_pinfos[index].dat_sockd = con->sockd;
		_pinfos[index].available = BOOL_TRUE;
		setup_peer_for_path(&_pinfos[index].peer, path);
		INFOPRINT("Path %s is available for sending", path->interface_name);
	}
	dmap_rdunlock_table_con();
	dmap_rdunlock_table_pth();
}

