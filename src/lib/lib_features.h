#ifndef INCGUARD_LIB_FEATURES_CONFS_H_
#define INCGUARD_LIB_FEATURES_CONFS_H_
#include "lib_descs.h"
#include "lib_makers.h"

feature_t *make_feature_ip_packets();
feature_t *make_feature_ip_bytes();
feature_t *make_feature_tcp_packets();
feature_t *make_feature_tcp_bytes();
feature_t *make_feature_udp_packets();
feature_t *make_feature_udp_bytes();

feature_t *make_feature_rtp_packets();
feature_t *make_feature_rtp_bytes();
feature_t *make_feature_lost_rtp_packets();

feature_t *make_feature_src_udp_packets();
feature_t *make_feature_src_udp_bytes();
feature_t *make_feature_src_tcp_packets();
feature_t *make_feature_src_tcp_bytes();

feature_t *make_feature_src_rtp_packets();
feature_t *make_feature_src_rtp_bytes();
feature_t *make_feature_src_lost_rtp_packets();

feature_t *make_feature_dst_udp_packets();
feature_t *make_feature_dst_udp_bytes();
feature_t *make_feature_dst_tcp_packets();
feature_t *make_feature_dst_tcp_bytes();
feature_t *make_feature_dst_rtp_packets();
feature_t *make_feature_dst_rtp_bytes();
feature_t *make_feature_dst_lost_rtp_packets();


#endif /* INCGUARD_LIB_FEATURES_CONFS_H_ */
