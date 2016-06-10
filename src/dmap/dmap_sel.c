#include "dmap_sel.h"
#include "dmap.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "inc_texts.h"
#include "lib_debuglog.h"

int32_t dmap_getid_bycon(connection_t *con)
{
	connection_t *itr;
	int32_t result;
	for(result = 0; dmap_itr_table_con(&result, &itr) == BOOL_TRUE && con != itr; ++result);
	if(result == NTRT_MAX_CONNECTION_NUM){
		ERRORPRINT("No id found in dmap for connection: %d", con->name);
		return DEVCLEGO_DMAP_ERROR_ITEM_NOT_FOUND;
	}
	return result;
}

connection_t* dmap_get_con_byname(char_t *name, int32_t *dmap_index)
{
	int32_t index;
	connection_t *result = NULL;
	for(index = 0; dmap_itr_table_con(&index, &result) == BOOL_TRUE; ++index)
	{
		if(strcmp(result->name, name) != 0){
			continue;
		}
		if(dmap_index != NULL){
			*dmap_index = index;
		}
		return result;
	}
	return NULL;
}


connection_t* dmap_get_con_byfilename(char_t* filename, int32_t *dmap_index)
{
	int32_t index;
	connection_t *result = NULL;
	for(index = 0; dmap_itr_table_con(&index, &result) == BOOL_TRUE; ++index)
	{
		if(strcmp(result->filename, filename) != 0){
			continue;
		}
		if(dmap_index != NULL){
			*dmap_index = index;
		}
		return result;
	}
	return NULL;
}

connection_t* dmap_get_con_byremoteip(uint32_t *ip_remote, int32_t *dmap_index)
{
	int32_t index;
	connection_t *result = NULL;
	for(index = 0; dmap_itr_table_con(&index, &result) == BOOL_TRUE; ++index)
	{
		if(memcmp(result->ip_remote, ip_remote, SIZE_IN6ADDR) != 0){
			continue;
		}
		if(dmap_index != NULL){
			*dmap_index = index;
		}
		return result;
	}
	return NULL;
}

tunnel_t* dmap_get_tun_byname(char_t* if_name, int32_t *dmap_index)
{
	int32_t index;
	tunnel_t *result = NULL;
	for(index = 0; dmap_itr_table_tun(&index, &result) == BOOL_TRUE; ++index)
	{
		if(strcmp(result->if_name, if_name) != 0){
			continue;
		}
		if(dmap_index != NULL){
			*dmap_index = index;
		}
		return result;
	}
	return NULL;
}

tunnel_t* dmap_get_tun_bycon(connection_t *con, int32_t *dmap_index)
{
	int32_t index;
	uint32_t ipv6[4];
	uint32_t ipv4;
	tunnel_t *result = NULL;
	for(index = 0; dmap_itr_table_tun(&index, &result) == BOOL_TRUE; ++index)
	{
		ipv4 = (result->ip4[3]<<24) | (result->ip4[2]<<16) | (result->ip4[1]<<8) | (result->ip4[0]);
		ipv6[3] = (result->ip6[3]<<24) | (result->ip6[2]<<16) | (result->ip6[1]<<8) | (result->ip6[0]);
		ipv6[2] = (result->ip6[7]<<24) | (result->ip6[6]<<16) | (result->ip6[5]<<8) | (result->ip6[4]);
		ipv6[1] = (result->ip6[11]<<24) | (result->ip6[10]<<16) | (result->ip6[9]<<8) | (result->ip6[8]);
		ipv6[0] = (result->ip6[15]<<24) | (result->ip6[14]<<16) | (result->ip6[13]<<8) | (result->ip6[12]);

		if((con->ip_version != 4 || memcmp(&(con->ip_local[3]), &ipv4, 4) != 0)
			&& (con->ip_version != 6 || memcmp(&(con->ip_local), &ipv6, 16) != 0)
		  )
		{
			continue;
		}
		if(dmap_index != NULL){
			*dmap_index = index;
		}
		return result;
	}
	return NULL;
}

interface_t* dmap_get_inf_byname(char_t* name, int32_t *dmap_index)
{
	int32_t index;
	interface_t *result = NULL;
	for(index = 0; dmap_itr_table_inf(&index, &result) == BOOL_TRUE; ++index)
	{
		if(strcmp(result->name, name) != 0){
			continue;
		}
		if(dmap_index != NULL){
			*dmap_index = index;
		}
		return result;
	}
	return NULL;
}

path_t* dmap_get_pth_byname(char_t* name, int32_t *dmap_index)
{
	int32_t index;
	path_t *result = NULL;
	for(index = 0; dmap_itr_table_pth(&index, &result) == BOOL_TRUE; ++index)
	{
		if(strcmp(result->interface_name, name) != 0){
			continue;
		}
		if(dmap_index != NULL){
			*dmap_index = index;
		}
		return result;
	}
	return NULL;
}

path_t* dmap_get_pth_byip(uint32_t* ip_local, uint32_t* ip_remote, int32_t* dmap_index)
{
	int32_t index;
	path_t *result = NULL;
	for(index = 0; dmap_itr_table_pth(&index, &result) == BOOL_TRUE; ++index)
	{
		//DEBUGPRINT("%p", result);
		//DEBUGPRINT("path->local: %X:%X:%X:%X || ip_local: %X:%X:%X:%X", result->ip_local[0], result->ip_local[1], result->ip_local[2], result->ip_local[3], ip_local[0], ip_local[1], ip_local[2], ip_local[3]);
		//DEBUGPRINT("path->remote: %X:%X:%X:%X || ip_remote: %X:%X:%X:%X", result->ip_remote[0], result->ip_remote[1], result->ip_remote[2], result->ip_remote[3], ip_remote[0], ip_remote[1], ip_remote[2], ip_remote[3]);
		if((memcmp(result->ip_local, ip_local, 16) != 0)
			|| memcmp(result->ip_remote, ip_remote, 16) != 0)
		{
			continue;
		}
		if(dmap_index != NULL){
			*dmap_index = index;
		}
		return result;
	}
	return NULL;
}


void dmap_rem_pths_byconid(int32_t con_dmap_id)
{
	int32_t    index;
	path_t    *path = NULL;
	for(index = 0; dmap_itr_table_pth(&index, &path) == BOOL_TRUE; ++index){
		if(path->con_dmap_id != con_dmap_id){
			continue;
		}
		dmap_rem_pth_byindex(index);
	}
}

void dmap_rem_net_byconid(int32_t con_dmap_id)
{
	int32_t          index;
	network_t       *net = NULL;
	for(index = 0; dmap_itr_table_net(&index, &net) == BOOL_TRUE; ++index){
		if(net->con_dmap_id != con_dmap_id){
			continue;
		}
		dmap_rem_net_byindex(index);
	}
}
