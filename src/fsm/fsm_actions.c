#include "fsm_actions.h"
#include "sys_confs.h"
#include "lib_tors.h"
#include "inc_unistd.h"
#include "lib_descs.h"
#include <dirent.h>
#include "etc_iniparser.h"
#include "dmap.h"
#include "dmap_sel.h"
#include "etc_utils.h"
#include <time.h>

static void _load_dir(char_t* dir, void (*loader)(char_t*));
static void _change_route_bynet(char_t *op, network_t *network, uint32_t *gw);
static void _del_route_bynet(network_t *network, uint32_t *gateway);
static void _add_route_bynet(network_t *network, uint32_t *gateway);
static void _add_route_bypath(path_t *path);
static void _del_route_bypath(path_t *path);
static void _change_route_bypath(char_t* op, path_t *path);

void con_load_from_dir(char_t *dir)
{
	_load_dir(dir, con_load_from_file);
}

void inf_load_from_dir(char_t *dir)
{
	_load_dir(dir, inf_load_from_file);
}

void inf_load_from_file(char_t *filename)
{
	dictionary    *conf ;
	int32_t        if_num;
	int32_t        tun_num;
	char_t         line[255];
	char_t         section[255];
	char_t         conf_key[255];
	char_t        *read_str;
	FILE          *fd;
	int32_t        index;
	tunnel_t      *tun, *tun_rem;
	interface_t   *inf, *inf_rem;

	fd = fopen(filename, "r");
	if (fd == NULL)
	{   sprintf(line , "\nFile read open error: %s\n",filename);
		EXERROR(line, 3);
	}
	fclose(fd);


	conf = iniparser_load( filename);
	if (conf==NULL) {
		fprintf(stdout, "cannot parse file: %s\n", filename);
		EXERROR("Iniparser failed", 3);
		return;
	}
	//iniparser_dump(conf, stderr);

	if_num = iniparser_getint(conf, "general:interface_num", 0);
	tun_num = iniparser_getint(conf, "general:tunnel_num", 0);
	if(if_num < 1){
		EXERROR("Interface number is too small.\n", 4);
	}

	if(tun_num < 1){
		EXERROR("Tunnel number is too small.\n", 4);
	}

	dmap_lock_sysdat();
	dmap_wrlock_table_tun();
	dmap_wrlock_table_inf();

	dmap_get_sysdat()->mp_global_server = iniparser_getint(conf, "general:accept_remote", 0);
	for(index = 0; index < tun_num; ++index)
	{
		sprintf(section, "tun_%d", index);
		tun = tun_from_conf(conf, section);
		tun_rem = dmap_get_tun_byname(tun->if_name, NULL);
		if(tun_rem != NULL){
			dmap_rem_tun_and_close(tun_rem);
		}
		dmap_add_tun(tun);

		tun->sockd = sysio->udpsock_open_and_bind(tun->cmd_port_rcv);
		tun->fd = sysio->tunnel_start(tun);
	}

	for(index = 0; index < if_num; ++index)
	{
		sprintf(section, "interface_%d", index);
		inf = inf_from_conf(conf, section);
		inf_rem = dmap_get_inf_byname(inf->name, NULL);
		if(inf_rem != NULL){
			dmap_rem_inf(inf_rem);
		}
		dmap_add_inf(inf);
	}

	dmap_unlock_sysdat();
	dmap_wrunlock_table_tun();
	dmap_wrunlock_table_inf();

	iniparser_freedict(conf);
}

void con_load_from_file(char_t *filename)
{
	dictionary    *conf ;
	int32_t        index;
	char_t         section[128];
	char_t        *read_str;
	FILE          *fd;
	char_t         line[128];
	connection_t  *conn, *conn_rem;
	path_t        *path;
	network_t     *net;
	int32_t        conn_id;

	fd = fopen(filename, "r");
	if (fd == NULL)
	{   sprintf(line , "File read open error: %s\n",filename);
		EXERROR(line, 3);
	}
	fclose(fd);

	conf = iniparser_load( filename);
	if (conf==NULL) {
		fprintf(stdout, "cannot parse file: %s\n", filename);
		EXERROR("Critical error.", 3);
	}
	//iniparser_dump(conf, stderr);

	dmap_lock_sysdat();
	dmap_wrlock_table_con();
	dmap_wrlock_table_pth();
	dmap_wrlock_table_net();

	conn = con_from_conf(conf);

	conn_rem = dmap_get_con_byfilename(filename, NULL);
	if(conn_rem != NULL){
		conn_id = dmap_getid_bycon(conn_rem);
		for(index = 0; dmap_itr_table_pth(&index, &path) == BOOL_TRUE; ++index){
			if(path->con_dmap_id != conn_id){
				continue;
			}
			_del_route_bypath(path);
		}
		for(index = 0; dmap_itr_table_net(&index, &net) == BOOL_TRUE; ++index){
			if(net->con_dmap_id != conn_id){
				continue;
			}
			_del_route_bynet(net, conn_rem->ip_remote);
		}
		dmap_rem_pths_byconid(conn_id);
		dmap_rem_net_byconid(conn_id);
		dmap_rem_con_and_close(conn_rem);
	}
	strcpy(conn->filename, filename);
	conn_id = dmap_add_con(conn);

	conn->sockd = sysio->udpsock_open_and_bind(conn->port_local);

	dmap_rem_pths_byconid(conn_id);
	for(index=0; index < conn->path_count; ++index ){
		sprintf(section, "path_%d", index);
		path = pth_from_conf(conf, section, conn_id);
		dmap_add_pth(path);
		_add_route_bypath(path);
	}

	dmap_rem_net_byconid(conn_id);
	for(index=0; index < conn->network_count; ++index){
		sprintf(section, "net_%d", index);
		net = net_from_conf(conf, section, conn_id);
		dmap_add_net(net);
		_add_route_bynet(net, conn->ip_remote);
	}

	iniparser_freedict(conf);
	dmap_get_sysdat()->mp_global_con = conn;

	dmap_wrunlock_table_net();
	dmap_wrunlock_table_pth();
	dmap_wrunlock_table_con();
	dmap_unlock_sysdat();
}

void _load_dir(char_t* dir, void (*loader)(char_t*))
{
	DIR *dp;
	char_t ext[8];
	char_t path[255];
	struct dirent *ep;

	dp = opendir(dir);
	if (dp == NULL){
		ERRORPRINT("Couldn't open the directory %s", dir);
		return;
	}

	while ((ep = readdir(dp))){
		strcpy(ext, &ep->d_name[strlen(ep->d_name) - 5]);
		if(strcmp(ext, "conf2") != 0){
			continue;
		}
		sprintf(path, "%s/%s", dir, ep->d_name);
		loader(path);
	}

	(void) closedir (dp);
}

void inf_save_to_file(interface_t *interface, char_t *filename)
{

}

void con_save_to_file(char_t *filename)
{
	FILE *fd;
	char lipstr[128], ripstr[128], gwstr[128];
	unsigned char *mac;
	int i, ver, af, start, num;
	int32_t  conn_id;
	connection_t *conn;
	path_t *path;
	network_t *network;
	dictionary *conf;
	char_t path_route[255];
	time_t rawtime;
	struct tm * timeinfo;

	char conf_key[255], conf_value[255], section[128];
	int path_count, network_count, index, checkp_index = 0;

	//chdir("conf/connections");

	conn = dmap_get_con_byfilename(filename, &conn_id);
	if(conn == NULL){
		WARNINGPRINT("The connection is not registered with this filename %s", filename);
		return;
	}
	//sprintf(path_route, "%s/%s/%s", sysio->config_dir, "connections", filename);
	sprintf(path_route, "%s", filename);
	fd = fopen(path_route, "w");
	//chdir("../..");
	if (fd == NULL){
		ERRORPRINT("File write open error: %s\n",path_route);
		return;
	}

  //save here
	fprintf(fd, "###################### Multipath Connection Information: ###############\n");
	fprintf(fd, "\n[connection]\n");

	fprintf(fd, "%-16s = %s\n", "name", conn->name);
	fprintf(fd, "%-16s = %d\n", "permission", conn->permission);
	fprintf(fd, "%-16s = %d\n", "ip_ver", conn->ip_version);

	set_ipstr(conf_value, 255, conn->ip_local, conn->ip_version);
	fprintf(fd, "%-16s = %s\n", "ip_local", conf_value);
	fprintf(fd, "%-16s = %d\n", "local_port", conn->port_local);

	set_ipstr(conf_value, 255, conn->ip_remote, conn->ip_version);
	fprintf(fd, "%-16s = %s\n", "ip_remote", conf_value);
	fprintf(fd, "%-16s = %d\n", "remote_port", conn->port_remote);
	fprintf(fd, "%-16s = %d\n", "remote_cmd_port", conn->cmd_port_remote);

	path_count = conn->path_count;
	fprintf(fd, "%-16s = %d\n", "path_count", conn->path_count);

	network_count =  conn->network_count;
	fprintf(fd, "%-16s = %d\n", "network_count", conn->network_count);
	fprintf(fd, "%-16s = %d\n", "keepalive_time", conn->keepalive);
	fprintf(fd, "%-16s = %d\n", "dead_time", conn->deadtimer);
	fprintf(fd, "%-16s = %d\n", "status", conn->status);
	fprintf(fd, "%-16s = %d\n", "auth_type", conn->auth_type);
	fprintf(fd, "%-16s = %s\n", "auth_key", conn->auth_key);


	//p = conn->mpath;
	fprintf(fd, "\n########################## PATHS ############################\n");
	for(index = 0; dmap_itr_table_pth(&index, &path) == BOOL_TRUE; ++index)
	//for(index = 0; index < path_count; ++index, ++p)
	{
	  if(path->con_dmap_id != conn_id){
		  continue;
	  }
	  fprintf(fd, "\n[path_%d]\n", index);
	  fprintf(fd, "%-16s = %s\n", "interface_name", path->interface_name);
	  fprintf(fd, "%-16s = %d\n", "ip_ver", path->ip_version);
	  mac_ntop((char*)path->mac_gw, conf_value);
	  fprintf(fd, "%-16s = %s\n", "local_macaddr", conf_value);
	  set_ipstr(conf_value, 255, path->ip_local, path->ip_version);
	  fprintf(fd, "%-16s = %s\n", "local_ipaddr", conf_value);
	  mac_ntop((char*)path->mac_gw, conf_value);
	  fprintf(fd, "%-16s = %s\n", "gw_macaddr", conf_value);
	  set_ipstr(conf_value, 255, path->ip_gw, path->ip_version);
	  fprintf(fd, "%-16s = %s\n", "gw_ipaddr", conf_value);
	  set_ipstr(conf_value, 255, path->ip_remote, path->ip_version);
	  fprintf(fd, "%-16s = %s\n", "remote_ipaddr", conf_value);
	  fprintf(fd, "%-16s = %d\n", "weight_in", path->weight_out);
	  fprintf(fd, "%-16s = %d\n", "weight_out", path->weight_out);
	  fprintf(fd, "%-16s = %d\n", "window_size", path->packet_max);
	  fprintf(fd, "%-16s = %d\n", "status", path->status);

	}

	//n = conn->networks;
	fprintf(fd, "\n####################### NETWORKS #####################################\n");
	for(index = 0; dmap_itr_table_net(&index, &network) == BOOL_TRUE; ++index)
	//for(index = 0; index < network_count; ++index, ++n)
	{
	  if(network->con_dmap_id != conn_id){
		  continue;
	  }
	  fprintf(fd, "\n[net_%d]\n", index);
	  fprintf(fd, "%-16s = %d\n", "ip_ver", network->version);
	  set_ipstr(conf_value, 255, network->source, network->version);
	  fprintf(fd, "%-16s = %s\n", "src_addr", conf_value);
	  fprintf(fd, "%-16s = %d\n", "src_prefix", network->source_prefix_length);
	  set_ipstr(conf_value, 255, network->destination, network->version);
	  fprintf(fd, "%-16s = %s\n", "dst_addr", conf_value);
	  fprintf(fd, "%-16s = %d\n", "dst_prefix", network->destination_prefix_length);
	}

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	fprintf(fd, "\n\n\n#Modified by MPT application at %s\n",  asctime (timeinfo) );
	fclose(fd);
	INFOPRINT("Connection config is saved into %s", filename);
	return;
}

void path_status_change(path_t *path, uint8_t status)
{
	char_t     pth_route[255];

	dmap_wrlock_table_pth();

	set_pathstr(pth_route, path);
	path->status = status;
	INFOPRINT("Path (%s) has changed to %d", pth_route, path->status);
	dmap_wrunlock_table_pth();

}

bool_t path_has_already_changed(path_t *path, uint8_t *status)
{
	switch(*status)
	{
		case MPT_REQ_STAT_OK:
			if(path->status  == MPT_REQ_STAT_OK){
				return BOOL_TRUE;
			}
			break;

		default:
			*status = path->status | MPT_REQ_STAT_PATH_DOWN;
			if((path->status  & MPT_REQ_STAT_PATH_DOWN) > 0){
				return BOOL_TRUE;
			}
			break;
	}
	return BOOL_FALSE;
}

void _add_route_bypath(path_t *path)
{
	_change_route_bypath("add", path);
}

void _del_route_bypath(path_t *path)
{
	_change_route_bypath("del", path);
}

void _change_route_bypath(char_t* op, path_t *path)
{
	char_t command[255], destination_network[128], gateway[128];
	int32_t af, start;
	strcpy(destination_network, "");
	strcpy(gateway, "");
	if (path->ip_version == 4) {
		af=AF_INET;
		start = 3;
	} else {
		af = AF_INET6;
		start = 0;
	}
	inet_ntop(af, &path->ip_remote[start], destination_network, 128);
	inet_ntop(af, &path->ip_gw[start], gateway, 128);
	sprintf(command, "%s/mpt_peer_routes.sh %s %d %s %s %s",
			sysio->bash_dir, op, path->ip_version, destination_network, gateway, path->interface_name);
	system(command);
}

void _add_route_bynet(network_t *network, uint32_t *gateway)
{
	_change_route_bynet("add", network, gateway);
}

void _del_route_bynet(network_t *network, uint32_t *gateway)
{
	_change_route_bynet("del", network, gateway);
}

void _change_route_bynet(char_t *op, network_t *network, uint32_t *gw)
{
	char_t command[255], destination_network[128], gateway[128];
	int32_t af, start;

	strcpy(destination_network, "");
	strcpy(gateway, "");

	if (network->version == 6) {
		af = AF_INET6;
		start = 0;
	}
	else {
		af = AF_INET;
		start = 3;
	}

	inet_ntop(af, &network->destination[start], destination_network, 128);
	inet_ntop(af, &gw[start], gateway, 128);
	sprintf(command, "%s/mpt_routes.sh %s %d %s %d %s", sysio->bash_dir, op, network->version, destination_network, network->destination_prefix_length, gateway);
	system(command);
}

