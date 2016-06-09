#ifndef INCGUARD_MPT_DMAP_SELECTS_H_
#define INCGUARD_MPT_DMAP_SELECTS_H_
#include "lib_defs.h"
#include "lib_descs.h"
#include "dmap.h"

int32_t dmap_getid_bycon(connection_t *con);
connection_t *dmap_get_con_byname(char_t *name, int32_t *dmap_index);
connection_t* dmap_get_con_byfilename(char_t* filename, int32_t *dmap_index);
connection_t* dmap_get_con_byremoteip(uint32_t *ip_remote, int32_t *dmap_index);

tunnel_t* dmap_get_tun_byname(char_t* if_name, int32_t *dmap_index);
tunnel_t* dmap_get_tun_bycon(connection_t *con, int32_t *dmap_index);

interface_t* dmap_get_inf_byname(char_t* name, int32_t *dmap_index);

path_t* dmap_get_pth_byname(char_t* name, int32_t *dmap_index);
path_t* dmap_get_pth_byip(uint32_t* ip_local, uint32_t* ip_remote, int32_t* dmap_index);

void dmap_rem_pths_byconid(int32_t con_dmap_id);
void dmap_rem_net_byconid(int32_t con_dmap_id);

#endif //INCGUARD_MPT_DMAP_SELECTS_H_
