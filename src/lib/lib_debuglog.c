#ifndef NDEBUG
#include "lib_debuglog.h"
#include <stdio.h>
#include <string.h>
#include "lib_defs.h"
#include "lib_descs.h"
#include "etc_utils.h"

#define setline(strout, line, retraction, format ,...)  \
	str_repeat(line, "\t", retraction);                 \
	strcat(strout, line);								\
	sprintf(line, format"\n", __VA_ARGS__);             \
	strcat(strout, line)

static void _d_pheader_print(char_t *result, ip_packet_header_t *header, int32_t retraction);
static void _d_pheaderv4_print(char_t* result, ipv4_packet_header_t *header, int32_t retraction);
static void _d_pheaderv6_print(char_t* result, ipv6_packet_header_t *header, int32_t retraction);
static void _d_ipaddrv4_print(char_t* result, ipv4_addr_t *addr, int32_t retraction);
static void _d_ipaddrv6_print(char_t* result, ipv6_addr_t *addr, int32_t retraction);
static void _d_state_print(char_t *result, int32_t retraction, packet_state_t state);
static void _d_pprint(packet_t *packet);

void debug_packet(packet_t *packet)
{
	char_t message[MPT_PACKET_LENGTH];
	int32_t index;
	for(index = 0; index < 64; ++index){
		sprintf(&message[index], "%X", packet->bytes[index]);
	}
	INFOPRINT("Packet message: %s", message);
}

void _d_pflow(packet_t *packet,  packet_state_t new_state)
{
	char_t strout[2048];
	_d_pprint(packet);
	packet->state = new_state;
	_d_state_print(strout, 1, packet->state);
}

void _d_pprint(packet_t *packet)
{
	char_t strout[2048];
	char_t line[128];
	char_t *state;

	setline(strout, line, 1, "packet ptr:   %p", packet);
	setline(strout, line, 1, "%s", "Header");
	_d_pheader_print(strout, packet->header, 2);
	setline(strout, line, 1, "header ptr:   %p", packet->header);
	setline(strout, line, 1, "length:       %d", packet->length);
	setline(strout, line, 1, "con_in   ptr: %p", packet->connection);
	setline(strout, line, 1, "path_out ptr: %p", packet->path_out);
	_d_state_print(strout, 1, packet->state);

}

void _d_state_print(char_t *result, int32_t retraction, packet_state_t state)
{
	char_t       line[128];
	char_t       state_str[128];
	switch(state)
	{
		case PACKET_STATE_CONSTRUCTED:
			strcpy(state_str, "Constructed");
		break;
		default:
			strcpy(state_str, "Unknown");
	}
	setline(result, line, retraction, "%s", state_str);
}

void _d_pheader_print(char_t* result, ip_packet_header_t *header, int32_t retraction)
{
	char_t   line[128];
	if(header == NULL){
		setline(result, line, retraction, "%s", "HEADER IS NOT EXISTS");
		return;
	}
	if(header->version == 4) {
		_d_pheaderv4_print(result, &(header->ipv4), retraction);
	}
	else if(header->version == 6){
		_d_pheaderv6_print(result, &(header->ipv6), retraction);
	}
	else{
		setline(result, line, retraction, "Version: %d", header->version);
	}
}

void _d_pheaderv4_print(char_t* result, ipv4_packet_header_t *header, int32_t retraction)
{
	char_t   line[128];
	setline(result, line, retraction, "IPv4 header ptr:  %p", header);
	setline(result, line, retraction, "IHL:              %d", header->IHL);
	setline(result, line, retraction, "DSCP:             %d", header->DSCP);
	setline(result, line, retraction, "ECN:              %d", header->ECN);
	setline(result, line, retraction, "Flags:            %d", header->flags);
	setline(result, line, retraction, "Checksum:         %d", header->header_checksum);
	setline(result, line, retraction, "Identification:   %d", header->identification);
	setline(result, line, retraction, "Protocol:         %d", header->protocol);
	setline(result, line, retraction, "TTL:              %d", header->time_to_live);
	setline(result, line, retraction, "Length:           %d", header->total_length);

	setline(result, line, retraction, "%s", "Source address      ");
	_d_ipaddrv4_print(result, &(header->src_addr), retraction + 1);
	setline(result, line, retraction, "%s", "Destination address ");
	_d_ipaddrv4_print(result, &(header->dst_addr), retraction + 1);

}

void _d_ipaddrv4_print(char_t* result, ipv4_addr_t *addr, int32_t retraction)
{
	char_t   line[128];
	setline(result, line, retraction, "IPv4 address ptr:    %p", addr);
	setline(result, line, retraction, "Value:               %x", addr->value);
	setline(result, line, retraction, "Value:               %d.%d.%d.%d", addr->bytes[0],addr->bytes[1],addr->bytes[2],addr->bytes[3]);
}

void _d_pheaderv6_print(char_t* result, ipv6_packet_header_t *header, int32_t retraction)
{
	char_t   line[128];
	setline(result, line, retraction, "IPv6 header ptr:  %p", header);
	setline(result, line, retraction, "Flow label:       %d", header->flow_label);
	setline(result, line, retraction, "Hop limit:        %d", header->hop_limit);
	setline(result, line, retraction, "Next header:      %d", header->next_header);
	setline(result, line, retraction, "Payload length:   %d", header->payload_length);
	setline(result, line, retraction, "Traffic class1:   %d", header->traffic_class1);
	setline(result, line, retraction, "Traffic class2:   %d", header->traffic_class2);

	setline(result, line, retraction, "%s", "Source address      ");
	_d_ipaddrv6_print(result, &(header->src_addr), retraction + 1);
	setline(result, line, retraction, "%s", "Destination address ");
	_d_ipaddrv6_print(result, &(header->dst_addr), retraction + 1);
}

void _d_ipaddrv6_print(char_t* result, ipv6_addr_t *addr, int32_t retraction)
{
	char_t   line[128];
	setline(result, line, retraction, "IPv6 address ptr:    %p", addr);
	setline(result, line, retraction, "Value:               %x:%x:%x:%x", addr->value[0],addr->value[1],addr->value[2],addr->value[3]);
}

#endif //NDEBUG
