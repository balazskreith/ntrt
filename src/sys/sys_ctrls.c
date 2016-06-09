#include "sys_ctrls.h"
#include "cmp_psender.h"
#include "cmp_pthsch.h"
#include "lib_makers.h"
#include "lib_funcs.h"
#include "etc_utils.h"
#include "sys_prints.h"
#include "dmap.h"
#include "cmp_conasr.h"
#include "cmp_pthsch.h"
#include "cmp_psender.h"
#include "lib_makers.h"
#include "lib_tors.h"
#include "dmap_sel.h"
#include "sys_confs.h"
#include "inc_opcall.h"
#include <dirent.h>
#include "fsm.h"

static request_t* _make_request_path_change(path_t *path, byte_t status);
static void* _cmd_rcall(void (*action)());

command_t* make_scall_cmd(char_t *cmd, void (*action)())
{
	command_t* result = make_command_pa(cmd, _cmd_rcall, action);
	return result;
}

command_t* make_event_cmd(int32_t event, void *arg)
{
	command_t *result;
	eventer_arg_t *eventer_arg = make_eventer_arg(event, arg);
	result = make_command_pa("Eventing", eventer, eventer_arg);
	return result;
}

command_t* make_cmd_opcall(char_t *cmd_name, char_t *system_cmd)
{
	command_t *result;
	result = make_command_pa(cmd_name, opcall_cmd_system, system_cmd);
	return result;
}

command_t* make_command_from_request(request_t *request)
{
	//CMP_DEF_THIS(_cmp_hs2cmd_t, _cmp_hs2cmd);
	command_t*  result = NULL;
	path_t*     path;
	uint32_t    ip_local[4], ip_remote[4];
	connection_t *conn;
	network_t *net;

	switch(request->command)
	{
		case MPT_REQ_CMD_P_STATUS_CHANGE:
			memcpy(ip_local,  &request->message[16], SIZE_IN6ADDR);
			memcpy(ip_remote, &request->message[0],  SIZE_IN6ADDR);
			path = dmap_get_pth_byip(ip_local, ip_remote, NULL);
			//DEBUGPRINT("path :%p", path);
			if(request->status == MPT_REQ_STAT_OK){
				result = make_event_cmd(MPT_EVENT_PATH_UP, path);
			}else if(request->status == MPT_REQ_STAT_PATH_DOWN){
				result = make_event_cmd(MPT_EVENT_PATH_DOWN, path);
			}
			//result->request = _make_request_path_change(path, request->status);
			break;
		case MPT_REQ_CMD_CONNECTION_SEND:
			conn = conn_ctor();
			memcpy(conn, request->message, request->size);
			result = make_event_cmd(MPT_EVENT_STOP, NULL);
			cmdcat(result, make_event_cmd(MPT_EVENT_CONNECTION_ADD, conn));
			cmdcat(result, make_event_cmd(MPT_EVENT_START, NULL));
			break;

		case MPT_REQ_CMD_NETWORK_SEND:
			net = network_ctor();
			memcpy(ip_local, request->message, SIZE_IN6ADDR);
			dmap_get_con_byremoteip(ip_local, &net->con_dmap_id);
			memcpy(net, &request->message[SIZE_IN6ADDR], request->size - SIZE_IN6ADDR);
			result = make_event_cmd(MPT_EVENT_NETWORK_ADD, net);
			break;

		case MPT_REQ_CMD_PATH_SEND:
			path = path_ctor();
			memcpy(ip_local, request->message, SIZE_IN6ADDR);
			dmap_get_con_byremoteip(ip_local, &path->con_dmap_id);
			memcpy(path, &request->message[SIZE_IN6ADDR], request->size - SIZE_IN6ADDR);
			result = make_event_cmd(MPT_EVENT_PATH_ADD, path);
			break;
	}

	return result;
}


request_t* make_request_path_up(path_t *path)
{
	return _make_request_path_change(path, MPT_REQ_STAT_OK);
}

request_t* make_request_path_down(path_t *path)
{
	return _make_request_path_change(path, MPT_REQ_STAT_PATH_DOWN);
}

request_t* make_request_con_send(connection_t *subject, connection_t *send)
{
	request_t *result;
	byte_t  message[MPT_PACKET_LENGTH];
	int32_t size;

	dmap_rdlock_table_con();
	size = sizeof(connection_t);
	memcpy(message, subject, size);
	strcpy(((connection_t*) message)->auth_key, "");
	((connection_t*) message)->auth_type = -3;
	((connection_t*) message)->sockd = -3;
	dmap_rdunlock_table_con();

	result = make_request(MPT_REQ_CMD_CONNECTION_SEND, 0, message, send, size);
	return result;
}

request_t* make_request_pth_send(path_t *subject, connection_t *send)
{
	request_t *result;
	byte_t  message[MPT_PACKET_LENGTH];
	connection_t *conn;
	int32_t size;

	dmap_rdlock_table_con();
	conn = dmap_get_con(subject->con_dmap_id);
	memcpy(message, conn->ip_remote, SIZE_IN6ADDR);
	dmap_rdunlock_table_con();

	dmap_rdlock_table_pth();
	size = sizeof(path_t);
	memcpy(&message[SIZE_IN6ADDR], subject, size);
	swap_ip6addr(((path_t*) message)->ip_local, ((path_t*) message)->ip_remote);
	dmap_rdunlock_table_pth();

	result = make_request(MPT_REQ_CMD_PATH_SEND, 0, message, send, size);
	return result;
}


request_t* make_request_net_send(network_t *subject, connection_t *send)
{
	request_t *result;
	byte_t  message[MPT_PACKET_LENGTH];
	connection_t *conn;
	int32_t size;

	dmap_rdlock_table_con();
	conn = dmap_get_con(subject->con_dmap_id);
	memcpy(message, conn->ip_remote, SIZE_IN6ADDR);
	dmap_rdunlock_table_con();

	dmap_rdlock_table_pth();
	size = sizeof(network_t);
	memcpy(&message[SIZE_IN6ADDR], subject, size);
	swap_nets((network_t*) &message[SIZE_IN6ADDR]);
	dmap_rdunlock_table_pth();

	result = make_request(MPT_REQ_CMD_NETWORK_SEND, 0, message, send, size);
	return result;
}
//_---------------------------------------------------------------------------------
request_t* _make_request_path_change(path_t *path, byte_t status)
{
	request_t *result;
	byte_t  message[MPT_PACKET_LENGTH];
	int32_t size;
	connection_t *con;

	dmap_rdlock_table_pth();

	dmap_rdlock_table_con();
	con = dmap_get_con(path->con_dmap_id);
	dmap_rdunlock_table_con();

	memcpy(&message[0],  path->ip_local,  SIZE_IN6ADDR);
	memcpy(&message[16], path->ip_remote, SIZE_IN6ADDR);
	size = SIZE_IN6ADDR << 1;
	dmap_rdunlock_table_pth();

	result = make_request(MPT_REQ_CMD_P_STATUS_CHANGE, status, message, con, size);
	return result;
}

void* _cmd_rcall(void (*action)())
{
	action();
	return NULL;
}
