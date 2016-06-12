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

void con_load_from_dir(char_t *dir)
{
	_load_dir(dir, con_load_from_file);
}

void inf_load_from_dir(char_t *dir)
{
	_load_dir(dir, inf_load_from_file);
}

void con_load_from_file(char_t *filename)
{
	dictionary    *conf ;
	int32_t        index;
	char_t         section[128];
	char_t        *read_str;
	FILE          *fd;
	char_t         line[128];
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
		dmap_rem_feature_and_close(conn_rem);
	}
	strcpy(conn->filename, filename);
	conn_id = dmap_add_feature(conn);

	conn->sockd = sysio->udpsock_open_and_bind(conn->port_local);

	dmap_rem_pths_byconid(conn_id);
	for(index=0; index < conn->path_count; ++index ){
		sprintf(section, "path_%d", index);
		path = pcap_listener_from_conf(conf, section, conn_id);
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

