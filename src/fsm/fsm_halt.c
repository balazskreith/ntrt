#include "fsm.h"
#include "fsm_actions.h"
#include "cmp_predefs.h"
#include "etc_iniparser.h"
#include "dmap.h"
#include "fsm_cmpactions.h"

//components:
#include "cmp_predefs.h"
#include "cmp_defs.h"
#include "cmp_conasr.h"
#include "cmp_cmdexetor.h"
#include "cmp_pthsch.h"
#include "cmp_precver.h"
#include "cmp_psender.h"
#include "cmp_cli.h"
#include "cmp_tunreader.h"
#include "cmp_screener.h"
#include "cmp_handshaker.h"
#include "cmp_tunwriter.h"

static void _sys_start();
static void _sys_shutdown();

static void _con_delete(char_t *filename);
static void _con_reload(char_t *filename);
//----------------------------------------------------------------------------------------------------
//------------------------- Transitions  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

fsm_states_t _fsm_halt_trans(int32_t event, void *arg)
{
	CMP_DEF_THIS(fsm_t, get_fsm());

	switch(event)
	{
	case NTRT_EVENT_START:
		_sys_start();
		return NTRT_STATE_RUN;
		break;
	case NTRT_EVENT_SHUTDOWN:
		_sys_shutdown();
		_fsm_mptsrv_dtor();
		return NTRT_STATE_EXIST;
		break;
	case NTRT_EVENT_CONNECTION_ADD:
		break;
	case NTRT_EVENT_CONNECTION_DELETE:
		_con_delete((char_t*)arg);
		break;
	case NTRT_EVENT_CONNECTION_RELOAD:
		_con_reload((char_t*)arg);
		break;
	default:
		WARNINGPRINT("The event (%s) you required doesn't have transition in the current state (%s)", this->event_str, this->state_str);
		break;
	}

	return this->current;
}



//----------------------------------------------------------------------------------------------------
//------------------------- Actions  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _sys_start()
{

	get_cmp_tunreader()->start();
	get_cmp_tunwriter()->start();

	get_cmp_pthsch()->start();
	get_cmp_psender()->start();

	get_cmp_conasr()->start();
	get_cmp_screener()->start();
	get_cmp_handshaker()->start();

	dmap_itr_do(dmap_itr_table_con, get_cmp_precver()->start);
}

void _sys_shutdown()
{

	dmap_itr_do(dmap_itr_table_con, dmap_rem_con_and_close);
	dmap_itr_do(dmap_itr_table_tun, dmap_rem_tun_and_close);
	dmap_itr_do(dmap_itr_table_net, dmap_rem_net);
	dmap_itr_do(dmap_itr_table_inf, dmap_rem_inf);
	dmap_itr_do(dmap_itr_table_pth, dmap_rem_pth);
	dmap_itr_do(dmap_itr_table_etc, dmap_rem_etc);
}

void _con_reload(char_t *filename)
{
	int32_t index;
	connection_t *conn;
	char_t path[255];

	//load config files
	if(filename != NULL){
		sprintf(path, "%s/%s/%s", sysio->config_dir, "connections", filename);
		con_load_from_file(path);
		return;
	}
	for(index = 0; dmap_itr_table_con(&index, &conn) == BOOL_TRUE; ++index){
		sprintf(path, "%s/%s/%s", sysio->config_dir, "connections", conn->filename);
		con_load_from_file(path);
	}

}

void _con_delete(char_t *filename)
{
	connection_t *conn;
	char_t oldname[255];
	char_t newname[255];
	int32_t conn_id;
	if(filename == NULL){
		ERRORPRINT("Can not delete a connection which has no filename.");
		return;
	}
	conn = dmap_get_con_byfilename(filename, &conn_id);
	if(conn == NULL){
		ERRORPRINT("Can not delete a connection which hasn't been registered.");
		return;
	}
	sprintf(oldname, "%s/%s/%s", sysio->config_dir, "connections", filename);
	sprintf(newname, "%s/%s/%s.disabled", sysio->config_dir, "connections", filename);
	dmap_wrlock_table_con();
	dmap_rem_con(conn);
	dmap_wrunlock_table_con();

	dmap_wrlock_table_pth();
	dmap_rem_pths_byconid(conn_id);
	dmap_wrunlock_table_pth();

	dmap_wrlock_table_net();
	dmap_rem_net_byconid(conn_id);
	dmap_wrunlock_table_net();

	rename(oldname, newname);
}



