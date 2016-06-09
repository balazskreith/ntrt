#ifndef INCGUARD_MPT_INC_IP_SEARCH_H_
#define INCGUARD_MPT_INC_IP_SEARCH_H_
#include "lib_defs.h"
#include "lib_descs.h"
bool_t is_con_addr_matched(packet_t *packet, connection_t *con);
bool_t is_net_addr_matched(packet_t *packet, network_t *net);


#endif //INCGUARD_MPT_ETC_IP_SEARCH_H_

