#include "fsm.h"
#include "dmap.h"
#include "etc_utils.h"
#include "fsm_actions.h"
#include "inc_opcall.h"
#include "sys_prints.h"

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

static void _sys_stop();
static void _path_change(path_t *path, uint8_t status);
static void _path_add(path_t *path);
static void _network_add(network_t *network);
static void _interface_change(interface_t *interface, uint8_t status, char_t *op);
static void _con_save(char_t *filename);
//----------------------------------------------------------------------------------------------------
//------------------------- Transitions  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

fsm_states_t _fsm_run_trans(int32_t event, void *arg)
{
	CMP_DEF_THIS(fsm_t, get_fsm());

	switch(event)
	{
	case NTRT_EVENT_STOP:
		_sys_stop();
		return NTRT_STATE_HALT;
		break;
	case NTRT_EVENT_CONNECTION_SAVE:
		_con_save((char_t*)arg);
		break;
	case NTRT_EVENT_PATH_UP:
		_path_change((path_t*) arg, NTRT_REQ_STAT_OK);
		break;
	case NTRT_EVENT_PATH_ADD:
		_path_add((path_t*) arg);
		break;
	case NTRT_EVENT_NETWORK_ADD:
		_network_add((network_t*) arg);
		break;
	case NTRT_EVENT_PATH_DOWN:
		_path_change((path_t*) arg, NTRT_REQ_STAT_PATH_DOWN);
		break;
	case NTRT_EVENT_INTERFACE_UP:
		_interface_change((interface_t*) arg, NTRT_REQ_STAT_OK, "add");
		break;
	case NTRT_EVENT_INTERFACE_DOWN:
		_interface_change((interface_t*) arg, NTRT_REQ_STAT_IF_DOWN, "del");
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

void _sys_stop()
{
	get_cmp_pthsch()->stop();
	get_cmp_psender()->stop();

	get_cmp_tunreader()->stop();
	get_cmp_conasr()->stop();
	get_cmp_screener()->stop();
	get_cmp_handshaker()->stop();

	dmap_itr_do(dmap_itr_table_con, get_cmp_precver()->stop);
}

void _path_change(path_t *path, uint8_t status)
{
	char_t     pth_route[255];

	if(path == NULL){
		WARNINGPRINT("The path is null given as an argument");
		return;
	}
	set_pathstr(pth_route, path);
	if(path_has_already_changed(path, &status) == BOOL_TRUE){
		INFOPRINT("Path (%s) has already changed to %d", pth_route, status);
		return;
	}

	get_cmp_pthsch()->stop();
	get_cmp_psender()->stop();

	path_status_change(path, status);

	get_cmp_pthsch()->start();
	get_cmp_psender()->start();
	sys_print_all_pth();
}

void _interface_change(interface_t *interface, uint8_t status, char_t *op)
{
	int32_t index;
	path_t *path;
	if(interface == NULL){
		WARNINGPRINT("The interface is null given as an argument");
		return;
	}
	get_cmp_pthsch()->stop();
	get_cmp_psender()->stop();

	for(index = 0; dmap_itr_table_pth(&index, &path) == BOOL_TRUE; ++index){
		if(strcmp(path->interface_name, interface->name) != 0){
			continue;
		}
		if(path_has_already_changed(path, &status) == BOOL_TRUE){
			continue;
		}
		path_status_change(path, status);
		opcall_change_route(path, op);
	}

	get_cmp_pthsch()->start();
	get_cmp_psender()->start();
}

void _path_add(path_t *path)
{
	if(path == NULL){
		ERRORPRINT("Path requested to add is null");
		return;
	}
	get_cmp_pthsch()->stop();
	get_cmp_psender()->stop();

	dmap_wrlock_table_pth();
	dmap_add_pth(path);
	INFOPRINT("Path is added");
	dmap_wrunlock_table_pth();

	get_cmp_pthsch()->start();
	get_cmp_psender()->start();
}

void _network_add(network_t *network)
{
	if(network == NULL){
		ERRORPRINT("Network requested to add is null");
		return;
	}
	get_cmp_pthsch()->stop();
	get_cmp_psender()->stop();

	dmap_wrlock_table_net();
	dmap_add_net(network);
	INFOPRINT("Network is added");
	dmap_wrunlock_table_net();

	get_cmp_pthsch()->start();
	get_cmp_psender()->start();
}

void _con_save(char_t *filename)
{
	int32_t index;
	connection_t *conn;
	char_t path[255];

	//load config files
	if(filename != NULL){
		sprintf(path, "%s/%s/%s", sysio->config_dir, "connections", filename);
		con_save_to_file(path);
		return;
	}
	for(index = 0; dmap_itr_table_con(&index, &conn) == BOOL_TRUE; ++index){
		sprintf(path, "%s/%s/%s", sysio->config_dir, "connections", conn->filename);
		con_save_to_file(path);
	}

}


