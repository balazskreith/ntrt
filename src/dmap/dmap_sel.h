#ifndef INCGUARD_NTRT_DMAP_SELECTS_H_
#define INCGUARD_NTRT_DMAP_SELECTS_H_
#include "lib_defs.h"
#include "lib_descs.h"
#include "dmap.h"

feature_t* dmap_get_feature_by_identifier(char_t* identifier);
int32_t dmap_get_id_by_pcapls(pcap_listener_t* pcap_listener);
mapped_var_t* dmap_get_mapped_var_by_var_id(int32_t map_id);
#endif //INCGUARD_NTRT_DMAP_SELECTS_H_
