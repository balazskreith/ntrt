#include "sys_confs.h"
#include "lib_tors.h"
#include "inc_unistd.h"
#include "lib_descs.h"
#include <string.h>
#include <dirent.h>
#include "lib_debuglog.h"
#include "dmap.h"
#include "lib_makers.h"
#include "etc_sniffex.h"


//-------------------------FEATURE BEGIN------------------------
static uint32_t _ip_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  return sniff_is_ip_packet(sniff) ? 1 : 0;
}

feature_t *make_feature_ip_packets()
{
  feature_t* result;
  result = make_feature("TCP Packets", "IP_PACKETS", _ip_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _ip_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_ip_packet(sniff)){
    return 0;
  }
  return ntohs(sniff->ip_len);
}

feature_t *make_feature_ip_bytes()
{
  feature_t* result;
  result = make_feature("TCP Bytes", "IP_BYTES", _ip_bytes_evaluator);
  return result;
}

//-------------------------FEATURE BEGIN------------------------
static uint32_t _tcp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  return sniff_is_tcp_packet(sniff) ? 1 : 0;
}

feature_t *make_feature_tcp_packets()
{
  feature_t* result;
  result = make_feature("TCP Packets", "TCP_PACKETS", _tcp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _tcp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_tcp_packet(sniff)){
    return 0;
  }
  return ntohs(sniff->ip_len);
}

feature_t *make_feature_tcp_bytes()
{
  feature_t* result;
  result = make_feature("TCP Bytes", "TCP_BYTES", _tcp_bytes_evaluator);
  return result;
}

//-------------------------FEATURE BEGIN------------------------
static uint32_t _udp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  return sniff_is_udp_packet(sniff) ? 1 : 0;
}

feature_t *make_feature_udp_packets()
{
  feature_t* result;
  result = make_feature("UDP Packets", "UDP_PACKETS", _udp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _udp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_udp_packet(sniff)){
    return 0;
  }
  return ntohs(sniff->ip_len) * 8;
}

feature_t *make_feature_udp_bytes()
{
  feature_t* result;
  result = make_feature("UDP Bytes", "UDP_BYTES", _udp_bytes_evaluator);
  return result;
}



//-------------------------FEATURE BEGIN------------------------
static uint32_t _rtp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
    return 0;
  }
  return 1;
}

feature_t *make_feature_rtp_packets()
{
  feature_t* result;
  result = make_feature("RTP Packets", "RTP_PACKETS_X", _rtp_packets_evaluator);
  return result;
}

//-------------------------FEATURE BEGIN------------------------
static uint32_t _rtp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
      return 0;
  }
  return sniff->ip_len;
}

feature_t *make_feature_rtp_bytes()
{
  feature_t* result;
  result = make_feature("RTP Bytes", "RTP_BYTES_X", _rtp_bytes_evaluator);
  return result;
}




//-------------------------FEATURE BEGIN------------------------
static u_short
_u_short_diff (u_short start, u_short end)
{
  if (start <= end) {
    return end - start;
  }
  return ~((u_short) (start - end));
}

static u_short
_cmp_u_short (u_short x, u_short y)
{
  if(x == y) return 0;
  if(x < y && y - x < 32768) return -1;
  if(x > y && x - y > 32768) return -1;
  if(x < y && y - x > 32768) return 1;
  if(x > y && x - y < 32768) return 1;
  return 0;
}

static uint32_t _lost_rtp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  u_short rtph_seq_num, diff;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
    return 0;
  }
  rtph_seq_num = ntohs(rtph->seq_num);
  if(data->seq_num_initialized == BOOL_FALSE){
    data->seq_num = rtph_seq_num;
    data->seq_num_initialized = BOOL_TRUE;
    return 0;
  }
  if(_cmp_u_short(rtph_seq_num, data->seq_num) <= 0){
    return 0;
  }

  diff = _u_short_diff(data->seq_num, rtph_seq_num);
  data->seq_num = rtph_seq_num;
  return diff - 1;
}

feature_t *make_feature_lost_rtp_packets()
{
  feature_t* result;
  result = make_feature("Lost RTP Packets", "LOST_RTP_PACKETS_X", _lost_rtp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _src_udp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_udp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_src) != data->port_num){
    return 0;
  }
  return 1;
}

feature_t *make_feature_src_udp_packets()
{
  feature_t* result;
  result = make_feature("UDP Packets on src port", "SRC_UDP_PACKETS_X", _src_udp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _src_udp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_udp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_src) != data->port_num){
    return 0;
  }
  return sniff->ip_len;
}

feature_t *make_feature_src_udp_bytes()
{
  feature_t* result;
  result = make_feature("UDP Bytes on src port", "SRC_UDP_BYTES_X", _src_udp_bytes_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _src_tcp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_src) != data->port_num){
    return 0;
  }
  return 1;
}

feature_t *make_feature_src_tcp_packets()
{
  feature_t* result;
  result = make_feature("TCP Packets on src port", "SRC_TCP_PACKETS_X", _src_tcp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _src_tcp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_src) != data->port_num){
    return 0;
  }
  return sniff->ip_len;
}

feature_t *make_feature_src_tcp_bytes()
{
  feature_t* result;
  result = make_feature("TCP Bytes on src port", "SRC_TCP_BYTES_X", _src_tcp_bytes_evaluator);
  return result;
}

//-------------------------FEATURE BEGIN------------------------
static uint32_t _src_rtp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_src) != data->port_num){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
      return 0;
  }
  return 1;
}

feature_t *make_feature_src_rtp_packets()
{
  feature_t* result;
  result = make_feature("RTP Packets on src port", "SRC_RTP_PACKETS_X_Y", _src_rtp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _src_rtp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_src) != data->port_num){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
      return 0;
  }
  return sniff->ip_len;
}

feature_t *make_feature_src_rtp_bytes()
{
  feature_t* result;
  result = make_feature("RTP Bytes on src port", "SRC_RTP_BYTES_X_Y", _src_rtp_bytes_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _src_lost_rtp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  u_short rtph_seq_num, diff;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_src) != data->port_num){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
    return 0;
  }
  rtph_seq_num = ntohs(rtph->seq_num);
  if(data->seq_num_initialized == BOOL_FALSE){
    data->seq_num = rtph_seq_num;
    data->seq_num_initialized = BOOL_TRUE;
    return 0;
  }
  if(_cmp_u_short(rtph_seq_num, data->seq_num) <= 0){
    return 0;
  }

  diff = _u_short_diff(data->seq_num, rtph_seq_num);
  data->seq_num = rtph_seq_num;
  return diff - 1;
}

feature_t *make_feature_src_lost_rtp_packets()
{
  feature_t* result;
  result = make_feature("Lost RTP Packets on src port", "SRC_LOST_RTP_PACKETS_X_Y", _src_lost_rtp_packets_evaluator);
  return result;
}














//-------------------------FEATURE BEGIN------------------------
static uint32_t _dst_udp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_udp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_dst) != data->port_num){
    return 0;
  }
  return 1;
}

feature_t *make_feature_dst_udp_packets()
{
  feature_t* result;
  result = make_feature("UDP Packets on dst port", "DST_UDP_PACKETS_X", _dst_udp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _dst_udp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_udp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_dst) != data->port_num){
    return 0;
  }
  return sniff->ip_len;
}

feature_t *make_feature_dst_udp_bytes()
{
  feature_t* result;
  result = make_feature("UDP Bytes on dst port", "DST_UDP_BYTES_X", _dst_udp_bytes_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _dst_tcp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_dst) != data->port_num){
    return 0;
  }
  return 1;
}

feature_t *make_feature_dst_tcp_packets()
{
  feature_t* result;
  result = make_feature("TCP Packets on dst port", "DST_TCP_PACKETS_X", _dst_tcp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _dst_tcp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  if(!sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_dst) != data->port_num){
    return 0;
  }
  return sniff->ip_len;
}

feature_t *make_feature_dst_tcp_bytes()
{
  feature_t* result;
  result = make_feature("TCP Bytes on dst port", "DST_TCP_BYTES_X", _dst_tcp_bytes_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _dst_rtp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_dst) != data->port_num){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
      return 0;
  }
  return 1;
}

feature_t *make_feature_dst_rtp_packets()
{
  feature_t* result;
  result = make_feature("RTP Packets on dst port", "DST_RTP_PACKETS_X_Y", _dst_rtp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _dst_rtp_bytes_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_dst) != data->port_num){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
      return 0;
  }
  return sniff->ip_len;
}

feature_t *make_feature_dst_rtp_bytes()
{
  feature_t* result;
  result = make_feature("RTP Bytes on dst port", "DST_RTP_BYTES_X_Y", _dst_rtp_bytes_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _dst_lost_rtp_packets_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  sniff_rtp_t *rtph;
  u_short rtph_seq_num, diff;
  if(!sniff_is_udp_packet(sniff) && !sniff_is_tcp_packet(sniff)){
    return 0;
  }
  if(ntohs(sniff->port_dst) != data->port_num){
    return 0;
  }
  rtph = (sniff_rtp_t *) sniff_get_payload(sniff);
  if(rtph->PT != data->payload_type){
    return 0;
  }
  rtph_seq_num = ntohs(rtph->seq_num);
  if(data->seq_num_initialized == BOOL_FALSE){
    data->seq_num = rtph_seq_num;
    data->seq_num_initialized = BOOL_TRUE;
    return 0;
  }
  if(_cmp_u_short(rtph_seq_num, data->seq_num) <= 0){
    return 0;
  }

  diff = _u_short_diff(data->seq_num, rtph_seq_num);
  data->seq_num = rtph_seq_num;
  return diff - 1;
}

feature_t *make_feature_dst_lost_rtp_packets()
{
  feature_t* result;
  result = make_feature("Lost RTP Packets on dst port", "DST_LOST_RTP_PACKETS_X_Y", _dst_lost_rtp_packets_evaluator);
  return result;
}


//-------------------------FEATURE BEGIN------------------------
static uint32_t _mapped_var_evaluator(sniff_t *sniff, evaluator_container_t* data)
{
  mapped_var_t *mapped_var;
  uint32_t result;
  dmap_rdlock_table_mapped_var();
  mapped_var = dmap_get_mapped_var_by_var_id(data->mapped_var_id);
  if(!mapped_var){
    WARNINGPRINT("mapped var with id %d not found", data->mapped_var_id);
    goto done;
  }
  result = mapped_var->value;
done:
  dmap_rdunlock_table_mapped_var();
  return result;
}

feature_t *make_feature_mapped_var()
{
  feature_t* result;
  result = make_feature("Mapped variable", "MAPPED_VAR_X", _mapped_var_evaluator);
  return result;
}
