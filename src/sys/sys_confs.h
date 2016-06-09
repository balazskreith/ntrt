#ifndef INCGUARD_SYS_CONFS_H_
#define INCGUARD_SYS_CONFS_H_
#include "lib_descs.h"
#include "etc_iniparser.h"



tunnel_t*     tun_from_conf(dictionary *conf, char_t* section);
interface_t*  inf_from_conf(dictionary *conf, char_t *section);
connection_t* con_from_conf(dictionary *conf);
path_t*       pth_from_conf(dictionary *conf, char_t *section, int32_t conn_id);
network_t*    net_from_conf(dictionary *conf, char_t *section, int32_t conn_id);


#endif /* INCGUARD_SYS_CONFS_H_ */
