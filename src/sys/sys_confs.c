#include "sys_confs.h"
#include "lib_tors.h"
#include "inc_unistd.h"
#include "lib_descs.h"
#include <string.h>
#include <dirent.h>
#include "lib_debuglog.h"

tunnel_t* tun_from_conf(dictionary *conf, char_t* section)
{
	char_t        *read_str;
	char_t        *lstr;
	char_t         conf_key[128];
	tunnel_t      *result;
	result = tunnel_ctor();

	sprintf(conf_key, "%s:name", section);
	read_str = iniparser_getstring(conf, conf_key, "tun0");
	strcpy(result->if_name, read_str);

	sprintf(conf_key, "%s:device", section);
	read_str = iniparser_getstring(conf, conf_key, "/dev/net/tun");
	strcpy(result->device, read_str);

	sprintf(conf_key, "%s:cmdport_local", section);
	result->cmd_port_rcv = iniparser_getint(conf, conf_key, 65050);

	sprintf(conf_key, "%s:playout_cut_size", section);
	result->playout_cut_size = iniparser_getint(conf, conf_key, 65050);

	sprintf(conf_key, "%s:playout_max_time", section);
	result->playout_max_time = iniparser_getint(conf, conf_key, 65050);

	sprintf(conf_key, "%s:cmdport_local", section);
	result->cmd_port_rcv = iniparser_getint(conf, conf_key, 65050);

	sprintf(conf_key, "%s:ipv4_addr", section);
	read_str = iniparser_getstring(conf, conf_key, "0.0.0.0/0");
	lstr = trim((char_t *)strtok(read_str, "/#\n"));
	inet_pton(AF_INET, lstr, &result->ip4);
	lstr = trim((char_t *)strtok(NULL, "/#\n"));
	result->ip4len = atoi(lstr);

	sprintf(conf_key, "%s:ipv6_addr", section);
	read_str = iniparser_getstring(conf, conf_key, "FE::1/0");
	lstr = trim((char_t *)strtok(read_str, "/#\n"));
	inet_pton(AF_INET6, lstr, &result->ip6);
	lstr = trim((char_t *)strtok(NULL, "/#\n"));
	result->ip6len = atoi(lstr);

	return result;
}

interface_t* inf_from_conf(dictionary *conf, char_t *section)
{
	char_t        conf_key[128];
	char_t       *read_str;
	char_t       *lstr;
	char_t        ipstr[255];
	char_t        cmdbuf[255];
	interface_t  *result;

	result = interface_ctor();

	sprintf(conf_key, "%s:name", section);
	read_str = iniparser_getstring(conf, conf_key, "unknown");
	strcpy(result->name, read_str);

	sprintf(conf_key, "%s:ipv4_addr", section);
	read_str = iniparser_getstring(conf, conf_key, "0.0.0.0/0");
	lstr = trim((char_t *)strtok(read_str, "/#\n"));
	inet_pton(AF_INET, lstr, &result->ip4);
	lstr = trim((char_t *)strtok(NULL, "/#\n"));
	result->ip4len = atoi(lstr);
	if (result->ip4len) {
		inet_ntop(AF_INET, &result->ip4, ipstr, 255);
		sprintf(cmdbuf,"%s/mpt_addr_adddel.sh add %s/%d %s", sysio->bash_dir, ipstr, result->ip4len, result->name );
		logging("running %s", cmdbuf);
		system(cmdbuf);
	}

	sprintf(conf_key, "%s:ipv6_addr", section);
	read_str = iniparser_getstring(conf, conf_key, "FE::1/0");
	lstr = trim((char_t *)strtok(read_str, "/#\n"));
	inet_pton(AF_INET, lstr, &result->ip6);
	lstr = trim((char_t *)strtok(NULL, "/#\n"));
	result->ip6len = atoi(lstr);
	if (result->ip6len) {
		inet_ntop(AF_INET6, &result->ip6, ipstr, 255);
		sprintf(cmdbuf,"%s/mpt_addr_adddel.sh change %s/%d %s", sysio->bash_dir, ipstr, result->ip6len, result->name);
		logging("running %s", cmdbuf);
		system(cmdbuf);
	}

	return result;
}



connection_t * con_from_conf(dictionary *conf)
{
	char_t   *read_str;
	int32_t   af, start;
	connection_t *result;

	result = conn_ctor();

	read_str = iniparser_getstring(conf, "connection:name", NTRT_CONNECTION_STRUCT_DEFAULT_NAME);
	strcpy(result->name, read_str);

	result->permission =
				iniparser_getint(conf, "connection:permission",NTRT_CONNECTION_STRUCT_DEFAULT_PERMISSION);

	result->ip_version =
			iniparser_getint(conf, "connection:ip_ver", NTRT_CONNECTION_STRUCT_DEFAULT_IP_VERSION);
	if (result->ip_version == 6) {
		af = AF_INET6;
		start = 0;
	}
	else {
		af = AF_INET;
		result->ip_version = 4;
		start = 3;
		result->ip_local[2]  = 0xFFFF0000;
		result->ip_remote[2] = 0xFFFF0000;
	}
	read_str = iniparser_getstring(conf,
			"connection:ip_local",
			NTRT_CONNECTION_STRUCT_DEFAULT_SOURCE_IP
			);

	inet_pton(af, read_str, &(result->ip_local[start]));

	result->port_local = iniparser_getint(conf,
			"connection:local_port",
			NTRT_CONNECTION_STRUCT_DEFAULT_DATA_LOCAL_PORT
			);

	read_str = iniparser_getstring(conf,
			"connection:ip_remote",
			NTRT_CONNECTION_STRUCT_DEFAULT_DEST_IP
			);
	inet_pton(af, read_str, &(result->ip_remote[start]));

	result->port_remote =
			iniparser_getint(conf,
					"connection:remote_port",
					NTRT_CONNECTION_STRUCT_DEFAULT_DATA_REMOTE_PORT
					);

	result->cmd_port_remote =
			iniparser_getint(conf,
					"connection:remote_cmd_port",
					NTRT_CONNECTION_STRUCT_DEFAULT_CMD_PORT);

	result->path_count =
			iniparser_getint(conf, "connection:path_count", 0);

	result->network_count =
			iniparser_getint(conf, "connection:network_count", 0);

	result->keepalive =
			iniparser_getint(conf, "connection:keepalive_time", 5);

	result->deadtimer =
			iniparser_getint(conf, "connection:dead_time", 20);

	result->status = iniparser_getint(conf,
						"connection:status",
						NTRT_CONNECTION_STRUCT_DEFAULT_STATUS
						);

	read_str = iniparser_getstring(conf,
			"connection:scheduling_type",
			NTRT_CONNECTION_STRUCT_DEFAULT_SCHEDULING_TYPE
			);

	if(strcmp("balanced", read_str) == 0){
		result->scheduling_type = PACKET_SCHEDULING_BALANCED;
	}else{
		result->scheduling_type = PACKET_SCHEDULING_WINDOWED;
	}

	read_str = iniparser_getstring(conf,
			"connection:auth_key",
			NTRT_CONNECTION_STRUCT_DEFAULT_AUTH_KEY
			);

	strcpy(result->auth_key, read_str);

	result->auth_type =
			iniparser_getint(conf,
					"connection:auth_type",
					NTRT_CONNECTION_STRUCT_DEFAULT_AUTH_TYPE
					);

	return result;
}

path_t* pth_from_conf(dictionary *conf, char_t *section, int32_t conn_id)
{
	char_t  conf_key[255];
	char_t *read_str;
	int32_t af;
	int32_t start;
	path_t *result;

	result = path_ctor();

	result->con_dmap_id = conn_id;
	sprintf(conf_key, "%s:interface_name", section);
	read_str = iniparser_getstring(conf, conf_key, "unnamed_result");
	strcpy(result->interface_name, read_str);

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:ip_ver", section);
	result->ip_version = iniparser_getint(conf, conf_key, 0);

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:local_macaddr", section);
	read_str= iniparser_getstring(conf, conf_key, "00:00:00:00:00:00");
	mac_pton(read_str, (char *)result->mac_local);

	if (result->ip_version == 6) {
		af = AF_INET6;
		start = 0;
	}
	else {
		af = AF_INET;
		result->ip_version = 4;
		start = 3;
		result->ip_local[2]  = 0xFFFF0000;
		result->ip_remote[2] = 0xFFFF0000;
		result->ip_gw[2]     = 0xFFFF0000;
	}

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:local_ipaddr", section);
	read_str = iniparser_getstring(conf, conf_key, "0.0.0.0");
	inet_pton(af, read_str, &(result->ip_local[start]));

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:gw_macaddr", section);
	read_str= iniparser_getstring(conf, conf_key, "00:00:00:00:00:00");
	mac_pton(read_str, (char *)result->mac_gw);

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:gw_ipaddr", section);
	read_str = iniparser_getstring(conf, conf_key, "0.0.0.0");
	inet_pton(af, read_str, &(result->ip_gw[start]));

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:remote_ipaddr", section);
	read_str = iniparser_getstring(conf, conf_key, "0.0.0.0");
	inet_pton(af, read_str, &(result->ip_remote[start]));

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:weight_in", section);
	result->weight_in = iniparser_getint(conf, conf_key, 0);

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:weight_out", section);
	result->weight_out = iniparser_getint(conf, conf_key, 0);

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:window_size", section);
	result->packet_max = iniparser_getint(conf, conf_key, 0);

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:status", section);
	result->status = iniparser_getint(conf, conf_key, 0);

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:bandwidth", section);
	result->bandwidth = iniparser_getint(conf, conf_key, 0);

	memset(conf_key, 0, strlen(conf_key));
	sprintf(conf_key, "%s:latency", section);
	result->latency = iniparser_getint(conf, conf_key, 0);

	return result;
}

network_t* net_from_conf(dictionary *conf, char_t *section, int32_t conn_id)
{
	char_t  conf_key[255];
	char_t *read_str;
	int32_t  version;
	int32_t  offset;
	network_t *result;

	result = network_ctor();

	result->con_dmap_id = conn_id;

	sprintf(conf_key, "%s:ip_ver", section);
	result->version = iniparser_getint(conf, conf_key, 6);
	version = (result->version == 4) ? AF_INET : AF_INET6;
	offset = 0;
	if (result->version == 4)  {
		offset = 3;
		result->source[2]  = 0xFFFF0000;
		result->destination[2] = 0xFFFF0000;
	}

	sprintf(conf_key, "%s:src_addr", section);
	read_str = iniparser_getstring(conf, conf_key, "0.0.0.0");
	inet_pton(version, read_str, &result->source[offset]);

	sprintf(conf_key, "%s:src_prefix", section);
	result->source_prefix_length =
			iniparser_getint(conf, conf_key, 0);

	sprintf(conf_key, "%s:dst_addr", section);
	read_str = iniparser_getstring(conf, conf_key, "0.0.0.0");
	inet_pton(version, read_str, &result->destination[offset]);

	sprintf(conf_key, "%s:dst_prefix", section);
	result->destination_prefix_length =
		iniparser_getint(conf, conf_key, 0);

	return result;
}



void _load_dir(char_t* dir, void*(*loader)(char_t*))
{
	DIR *dp;
	char_t file[255];
	char_t ext[8];
	char_t path[255];
	struct dirent *ep;
	dp = opendir (dir);
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
