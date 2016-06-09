#include "inc_opcall.h"
#include "lib_descs.h"
#include <stdio.h>



void* opcall_change_route(path_t *path, char_t* op)
{
	char command[255], destination_network[128], gateway[128];
	set_ipstr(destination_network, 128, path->ip_remote, path->ip_version);
	set_ipstr(gateway, 128, path->ip_gw, path->ip_version);
	sprintf(command, "%s/mpt_peer_routes.sh %s %d %s %s %s", sysio->bash_dir, op, path->ip_version, destination_network, gateway, path->interface_name);
	sysio->opcall(command);
	return NULL;
}

void* opcall_cmd_system(char_t *str)
{
	sysio->opcall(str);
	free(str);
	return NULL;
}
