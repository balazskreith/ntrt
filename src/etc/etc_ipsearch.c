#include "etc_ipsearch.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "dmap.h"
#include "etc_auth.h"

static bool_t _is_ipv4_network_matched(network_t *network, uint32_t *src, uint32_t *dst);
static bool_t _is_ipv6_network_matched(network_t *network, uint32_t *src, uint32_t *dst);

static uint32_t SYNC_ipv4_masks[33] = {
		0x00000000, //0.0.0.0
		0x00000080, //128.0.0.0
		0x000000C0, //192.0.0.0
		0x000000E0, //224.0.0.0
		0x000000F0, //240.0.0.0
		0x000000F8, //248.0.0.0
		0x000000FC, //252.0.0.0
		0x000000FE, //254.0.0.0
		0x000000FF, //255.0.0.0
		0x000080FF, //255.128.0.0
		0x0000C0FF, //255.192.0.0
		0x0000E0FF, //255.224.0.0
		0x0000F0FF, //255.240.0.0
		0x0000F8FF, //255.248.0.0
		0x0000FCFF, //255.252.0.0
		0x0000FEFF, //255.254.0.0
		0x0000FFFF, //255.255.0.0
		0x0080FFFF, //255.255.128.0
		0x00C0FFFF, //255.255.192.0
		0x00E0FFFF, //255.255.224.0
		0x00F0FFFF, //255.255.240.0
		0x00F8FFFF, //255.255.248.0
		0x00FCFFFF, //255.255.252.0
		0x00FEFFFF, //255.255.254.0
		0x00FFFFFF, //255.255.255.0
		0x80FFFFFF, //255.255.255.128
		0xC0FFFFFF, //255.255.255.192
		0xE0FFFFFF, //255.255.255.224
		0xF0FFFFFF, //255.255.255.240
		0xF8FFFFFF, //255.255.255.248
		0xFCFFFFFF, //255.255.255.252
		0xFEFFFFFF, //255.255.255.254
		0xFFFFFFFF //255.255.255.255
};


static uint32_t SYNC_ipv6_masks[516] = {  //516 = 129*4
		0x00000000, 0x00000000, 0x00000000, 0x00000000, //0
		0x00000000, 0x00000000, 0x00000000, 0x00000080, //1
		0x00000000, 0x00000000, 0x00000000, 0x000000C0, //2
		0x00000000, 0x00000000, 0x00000000, 0x000000E0, //3
		0x00000000, 0x00000000, 0x00000000, 0x000000F0, //4
		0x00000000, 0x00000000, 0x00000000, 0x000000F8, //5
		0x00000000, 0x00000000, 0x00000000, 0x000000FC, //6
		0x00000000, 0x00000000, 0x00000000, 0x000000FE, //7
		0x00000000, 0x00000000, 0x00000000, 0x000000FF, //8
		0x00000000, 0x00000000, 0x00000000, 0x000080FF, //9
		0x00000000, 0x00000000, 0x00000000, 0x0000C0FF, //10
		0x00000000, 0x00000000, 0x00000000, 0x0000E0FF, //11
		0x00000000, 0x00000000, 0x00000000, 0x0000F0FF, //12
		0x00000000, 0x00000000, 0x00000000, 0x0000F8FF, //13
		0x00000000, 0x00000000, 0x00000000, 0x0000FCFF, //14
		0x00000000, 0x00000000, 0x00000000, 0x0000FEFF, //15
		0x00000000, 0x00000000, 0x00000000, 0x0000FFFF, //16
		0x00000000, 0x00000000, 0x00000000, 0x0080FFFF, //17
		0x00000000, 0x00000000, 0x00000000, 0x00C0FFFF, //18
		0x00000000, 0x00000000, 0x00000000, 0x00E0FFFF, //19
		0x00000000, 0x00000000, 0x00000000, 0x00F0FFFF, //20
		0x00000000, 0x00000000, 0x00000000, 0x00F8FFFF, //21
		0x00000000, 0x00000000, 0x00000000, 0x00FCFFFF, //22
		0x00000000, 0x00000000, 0x00000000, 0x00FEFFFF, //23
		0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, //24
		0x00000000, 0x00000000, 0x00000000, 0x80FFFFFF, //25
		0x00000000, 0x00000000, 0x00000000, 0xC0FFFFFF, //26
		0x00000000, 0x00000000, 0x00000000, 0xE0FFFFFF, //27
		0x00000000, 0x00000000, 0x00000000, 0xF0FFFFFF, //28
		0x00000000, 0x00000000, 0x00000000, 0xF8FFFFFF, //29
		0x00000000, 0x00000000, 0x00000000, 0xFCFFFFFF, //30
		0x00000000, 0x00000000, 0x00000000, 0xFEFFFFFF, //31
		0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, //32
		0x00000000, 0x00000000, 0x00000080, 0xFFFFFFFF, //33
		0x00000000, 0x00000000, 0x000000C0, 0xFFFFFFFF, //34
		0x00000000, 0x00000000, 0x000000E0, 0xFFFFFFFF, //35
		0x00000000, 0x00000000, 0x000000F0, 0xFFFFFFFF, //36
		0x00000000, 0x00000000, 0x000000F8, 0xFFFFFFFF, //37
		0x00000000, 0x00000000, 0x000000FC, 0xFFFFFFFF, //38
		0x00000000, 0x00000000, 0x000000FE, 0xFFFFFFFF, //39
		0x00000000, 0x00000000, 0x000000FF, 0xFFFFFFFF, //40
		0x00000000, 0x00000000, 0x000080FF, 0xFFFFFFFF, //41
		0x00000000, 0x00000000, 0x0000C0FF, 0xFFFFFFFF, //42
		0x00000000, 0x00000000, 0x0000E0FF, 0xFFFFFFFF, //43
		0x00000000, 0x00000000, 0x0000F0FF, 0xFFFFFFFF, //44
		0x00000000, 0x00000000, 0x0000F8FF, 0xFFFFFFFF, //45
		0x00000000, 0x00000000, 0x0000FCFF, 0xFFFFFFFF, //46
		0x00000000, 0x00000000, 0x0000FEFF, 0xFFFFFFFF, //47
		0x00000000, 0x00000000, 0x0000FFFF, 0xFFFFFFFF, //48
		0x00000000, 0x00000000, 0x0080FFFF, 0xFFFFFFFF, //49
		0x00000000, 0x00000000, 0x00C0FFFF, 0xFFFFFFFF, //50
		0x00000000, 0x00000000, 0x00E0FFFF, 0xFFFFFFFF, //51
		0x00000000, 0x00000000, 0x00F0FFFF, 0xFFFFFFFF, //52
		0x00000000, 0x00000000, 0x00F8FFFF, 0xFFFFFFFF, //53
		0x00000000, 0x00000000, 0x00FCFFFF, 0xFFFFFFFF, //54
		0x00000000, 0x00000000, 0x00FEFFFF, 0xFFFFFFFF, //55
		0x00000000, 0x00000000, 0x00FFFFFF, 0xFFFFFFFF, //56
		0x00000000, 0x00000000, 0x80FFFFFF, 0xFFFFFFFF, //57
		0x00000000, 0x00000000, 0xC0FFFFFF, 0xFFFFFFFF, //58
		0x00000000, 0x00000000, 0xE0FFFFFF, 0xFFFFFFFF, //59
		0x00000000, 0x00000000, 0xF0FFFFFF, 0xFFFFFFFF, //60
		0x00000000, 0x00000000, 0xF8FFFFFF, 0xFFFFFFFF, //61
		0x00000000, 0x00000000, 0xFCFFFFFF, 0xFFFFFFFF, //62
		0x00000000, 0x00000000, 0xFEFFFFFF, 0xFFFFFFFF, //63
		0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, //64
		0x00000000, 0x00000080, 0xFFFFFFFF, 0xFFFFFFFF, //65
		0x00000000, 0x000000C0, 0xFFFFFFFF, 0xFFFFFFFF, //66
		0x00000000, 0x000000E0, 0xFFFFFFFF, 0xFFFFFFFF, //67
		0x00000000, 0x000000F0, 0xFFFFFFFF, 0xFFFFFFFF, //68
		0x00000000, 0x000000F8, 0xFFFFFFFF, 0xFFFFFFFF, //69
		0x00000000, 0x000000FC, 0xFFFFFFFF, 0xFFFFFFFF, //70
		0x00000000, 0x000000FE, 0xFFFFFFFF, 0xFFFFFFFF, //71
		0x00000000, 0x000000FF, 0xFFFFFFFF, 0xFFFFFFFF, //72
		0x00000000, 0x000080FF, 0xFFFFFFFF, 0xFFFFFFFF, //73
		0x00000000, 0x0000C0FF, 0xFFFFFFFF, 0xFFFFFFFF, //74
		0x00000000, 0x0000E0FF, 0xFFFFFFFF, 0xFFFFFFFF, //75
		0x00000000, 0x0000F0FF, 0xFFFFFFFF, 0xFFFFFFFF, //76
		0x00000000, 0x0000F8FF, 0xFFFFFFFF, 0xFFFFFFFF, //77
		0x00000000, 0x0000FCFF, 0xFFFFFFFF, 0xFFFFFFFF, //78
		0x00000000, 0x0000FEFF, 0xFFFFFFFF, 0xFFFFFFFF, //79
		0x00000000, 0x0000FFFF, 0xFFFFFFFF, 0xFFFFFFFF, //80
		0x00000000, 0x0080FFFF, 0xFFFFFFFF, 0xFFFFFFFF, //81
		0x00000000, 0x00C0FFFF, 0xFFFFFFFF, 0xFFFFFFFF, //82
		0x00000000, 0x00E0FFFF, 0xFFFFFFFF, 0xFFFFFFFF, //83
		0x00000000, 0x00F0FFFF, 0xFFFFFFFF, 0xFFFFFFFF, //84
		0x00000000, 0x00F8FFFF, 0xFFFFFFFF, 0xFFFFFFFF, //85
		0x00000000, 0x00FCFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //86
		0x00000000, 0x00FEFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //87
		0x00000000, 0x00FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //88
		0x00000000, 0x80FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //89
		0x00000000, 0xC0FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //90
		0x00000000, 0xE0FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //91
		0x00000000, 0xF0FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //92
		0x00000000, 0xF8FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //93
		0x00000000, 0xFCFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //94
		0x00000000, 0xFEFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //95
		0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //96
		0x00000080, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //97
		0x000000C0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //98
		0x000000E0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //99
		0x000000F0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //100
		0x000000F8, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //101
		0x000000FC, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //102
		0x000000FE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //103
		0x000000FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //104
		0x000080FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //105
		0x0000C0FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //106
		0x0000E0FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //107
		0x0000F0FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //108
		0x0000F8FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //109
		0x0000FCFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //110
		0x0000FEFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //111
		0x0000FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //112
		0x0080FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //113
		0x00C0FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //114
		0x00E0FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //115
		0x00F0FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //116
		0x00F8FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //117
		0x00FCFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //118
		0x00FEFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //119
		0x00FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //120
		0x80FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //121
		0xC0FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //122
		0xE0FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //123
		0xF0FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //124
		0xF8FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //125
		0xFCFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //126
		0xFEFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, //127
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF  //128
};

__always_inline
bool_t is_con_addr_matched(packet_t *packet, connection_t *con)
{
	ip_packet_header_t* header = packet->header;
	if(con->ip_version != header->version && packet->bytes[0] < 0xA0){
		return BOOL_FALSE;
	}
	//DEBUGPRINT("%X:%X:%X:%X", con->ip_local[0], con->ip_local[1], con->ip_local[2], con->ip_local[3]);
	//DEBUGPRINT("%X:%X:%X:%X", packet->bytes[24], packet->bytes[25], packet->bytes[26], packet->bytes[27]);

	if(header->version == 4 &&
		con->ip_local[3] == header->ipv4.src_addr.value &&
		con->ip_remote[3] == header->ipv4.dst_addr.value)
	{
		return BOOL_TRUE;

	}
	else if(header->version == 6 &&
		memcmp(con->ip_local, header->ipv6.src_addr.value, SIZE_IN6ADDR) == 0 &&
		memcmp(con->ip_remote, header->ipv6.dst_addr.value, SIZE_IN6ADDR) == 0)
	{
		return BOOL_TRUE;

	}
	else if(0xA0 <= packet->bytes[0] &&
		memcmp(con->ip_local, &packet->bytes[ authSize(packet->bytes[2]) + 24 ] , SIZE_IN6ADDR) == 0 &&
		memcmp(con->ip_remote, &packet->bytes[ authSize(packet->bytes[2]) + 8 ] , SIZE_IN6ADDR) == 0)
	{
		return BOOL_TRUE;
	}
	//original version of the mptsrv send back
	//the packet with round 2 without set its local and remote ip.
	else if(0xA0 <= packet->bytes[0] &&
		(packet->bytes[ authSize(packet->bytes[2]) + 4 ] == 2 || packet->bytes[ authSize(packet->bytes[2]) + 4 ] == 4)&&
		memcmp(con->ip_local, &packet->bytes[ authSize(packet->bytes[2]) + 8 ] , SIZE_IN6ADDR) == 0 &&
		memcmp(con->ip_remote, &packet->bytes[ authSize(packet->bytes[2]) + 24 ] , SIZE_IN6ADDR) == 0)
	{
		return BOOL_TRUE;
	}
	return BOOL_FALSE;
}

__always_inline
bool_t is_net_addr_matched(packet_t *packet, network_t *net)
{
	ip_packet_header_t* header = packet->header;
	if(net->version != header->version && packet->bytes[0] < 0xA0){
		return BOOL_FALSE;
	}
	if(header->version == 4
		&& _is_ipv4_network_matched(net, &header->ipv4.src_addr.value, &header->ipv4.dst_addr.value) == BOOL_TRUE)
	{
		return BOOL_TRUE;
	}
	else if(header->version == 6
		&& _is_ipv6_network_matched(net, header->ipv6.src_addr.value, header->ipv6.dst_addr.value) == BOOL_TRUE)
	{
		return BOOL_TRUE;
	}
	else if(0xA0 <= packet->bytes[0]
	    && _is_ipv6_network_matched(net, (uint32_t*) &packet->bytes[ authSize(packet->bytes[2]) + 24 ], (uint32_t*) &packet->bytes[ authSize(packet->bytes[2]) + 8 ]) == BOOL_TRUE)
	{
		return BOOL_TRUE;
	}
	return BOOL_FALSE;
}


bool_t _is_ipv4_network_matched(network_t *network, uint32_t *src, uint32_t *dst)
{

  uint32_t    *src_mask;
  uint32_t    *dst_mask;
  uint32_t     src_net;
  uint32_t     dst_net;
  bool_t       result = BOOL_FALSE;

  src_mask = SYNC_ipv4_masks + network->source_prefix_length;
  src_net = *src & *src_mask;

  //if(memcmp(&src_net, &network->source[3], 4) != 0){
  if(src_net != network->source[3] != 0){
	  return result;
  }

  dst_mask = SYNC_ipv4_masks + network->destination_prefix_length;
  dst_net = *dst & *dst_mask;

  //result = memcmp(&dst_net, &network->source[3], 4) == 0 ? BOOL_TRUE : BOOL_FALSE;
  result = src_net == network->source[3] ? BOOL_TRUE : BOOL_FALSE;
  return result;
}

bool_t _is_ipv6_network_matched(network_t *network, uint32_t *src, uint32_t *dst)
{

  uint32_t    *src_mask;
  uint32_t    *dst_mask;
  uint32_t     src_net[4];
  uint32_t     dst_net[4];
  bool_t       result = BOOL_FALSE;

  src_mask = (SYNC_ipv6_masks + network->source_prefix_length * 4);
  src_net[0] = src[0] & src_mask[0];
  src_net[1] = src[1] & src_mask[1];
  src_net[2] = src[2] & src_mask[2];
  src_net[3] = src[3] & src_mask[3];

  if(memcmp(&src_net, &network->source, 16) != 0){
	  return result;
  }

  dst_mask = (SYNC_ipv6_masks + network->source_prefix_length * 4);
  dst_net[0] = dst[0] & dst_mask[0];
  dst_net[1] = dst[1] & dst_mask[1];
  dst_net[2] = dst[2] & dst_mask[2];
  dst_net[3] = dst[3] & dst_mask[3];

  result = memcmp(&dst_net, &network->source, 16) == 0 ? BOOL_TRUE : BOOL_FALSE;
  return result;
}


