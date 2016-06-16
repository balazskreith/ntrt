#include "etc_sniffex.h"
#include "lib_descs.h"
/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void
print_hex_ascii_line(const u_char *payload, int len, int offset)
{

	int i;
	int gap;
	const u_char *ch;

	/* offset */
	printf("%05d   ", offset);
	
	/* hex */
	ch = payload;
	for(i = 0; i < len; i++) {
		printf("%02x ", *ch);
		ch++;
		/* print extra space after 8th byte for visual aid */
		if (i == 7)
			printf(" ");
	}
	/* print space to handle line less than 8 bytes */
	if (len < 8)
		printf(" ");
	
	/* fill hex gap with spaces if not full line */
	if (len < 16) {
		gap = 16 - len;
		for (i = 0; i < gap; i++) {
			printf("   ");
		}
	}
	printf("   ");
	
	/* ascii (if printable) */
	ch = payload;
	for(i = 0; i < len; i++) {
		if (isprint(*ch))
			printf("%c", *ch);
		else
			printf(".");
		ch++;
	}

	printf("\n");

return;
}

/*
 * print packet payload data (avoid printing binary data)
 */
void
print_payload(const u_char *payload, int len)
{

	int len_rem = len;
	int line_width = 16;			/* number of bytes per line */
	int line_len;
	int offset = 0;					/* zero-based offset counter */
	const u_char *ch = payload;

	if (len <= 0)
		return;

	/* data fits on one line */
	if (len <= line_width) {
		print_hex_ascii_line(ch, len, offset);
		return;
	}

	/* data spans multiple lines */
	for ( ;; ) {
		/* compute current line length */
		line_len = line_width % len_rem;
		/* print line */
		print_hex_ascii_line(ch, line_len, offset);
		/* compute total remaining */
		len_rem = len_rem - line_len;
		/* shift pointer to remaining bytes to print */
		ch = ch + line_len;
		/* add offset */
		offset = offset + line_width;
		/* check if we have line width chars or less */
		if (len_rem <= line_width) {
			/* print last line and get out */
			print_hex_ascii_line(ch, len_rem, offset);
			break;
		}
	}

return;
}



/*
 * dissect/print packet
 */
bool_t
sniff_is_ip_packet(sniff_t *sniff)
{

        const struct pcap_pkthdr *header;
        const u_char *packet;

        const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
        const struct sniff_ip *ip;              /* The IP header */
        int size_ip;

        if(!sniff->packet){
          sniff->is_udp_packet = EXTENDED_BOOL_FALSE;
          return BOOL_FALSE;
        }

        header = sniff->header;
        packet = sniff->packet;

        if(sniff->is_ip_packet != EXTENDED_BOOL_UNDEFINED){
          return (bool_t) sniff->is_ip_packet;
        }
        /* define ethernet header */
        ethernet = (struct sniff_ethernet*)(packet);

        /* define/compute ip header offset */
        ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
        size_ip = IP_HL(ip)*4;
        if (size_ip < 20) {
            sniff->is_ip_packet = EXTENDED_BOOL_FALSE;
            return BOOL_FALSE;
        }

        sniff->ip_src  = ip->ip_src;
        sniff->ip_dst  = ip->ip_dst;
        sniff->ip_p    = ip->ip_p;
        sniff->ip_len  = ip->ip_len;
        sniff->size_ip = size_ip;
        sniff->is_ip_packet = EXTENDED_BOOL_TRUE;
        return BOOL_TRUE;
}

bool_t
sniff_is_tcp_packet(sniff_t *sniff)
{

        const struct pcap_pkthdr *header;
        const u_char *packet;
        const struct sniff_tcp *tcp;            /* The TCP header */
        const char *payload;                    /* Packet payload */

        int size_tcp;
        int size_payload;

        if(!sniff->packet){
          sniff->is_udp_packet = EXTENDED_BOOL_FALSE;
          return BOOL_FALSE;
        }

        header = sniff->header;
        packet = sniff->packet;

        if(sniff->is_tcp_packet != EXTENDED_BOOL_UNDEFINED){
          return (bool_t) sniff->is_tcp_packet;
        }

        if(sniff_is_ip_packet(sniff) == BOOL_FALSE || sniff->ip_p != IPPROTO_TCP){
          sniff->is_tcp_packet = EXTENDED_BOOL_FALSE;
          return BOOL_FALSE;
        }

        /* define/compute tcp header offset */
         tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + sniff->size_ip);
         size_tcp = TH_OFF(tcp)*4;
         if (size_tcp < 20) {
             ERRORPRINT("Invalid TCP header length: %u bytes", size_tcp);
             sniff->is_tcp_packet = EXTENDED_BOOL_FALSE;
             return BOOL_FALSE;
         }
         sniff->port_src = tcp->th_sport;
         sniff->port_dst = tcp->th_dport;

         /* define/compute tcp payload (segment) offset */
         sniff->payload = (u_char *)(packet + SIZE_ETHERNET + sniff->size_ip + size_tcp);
         /* compute tcp payload (segment) size */
         sniff->size_payload = ntohs(sniff->ip_len) - (sniff->size_ip + size_tcp);
         sniff->is_tcp_packet = EXTENDED_BOOL_TRUE;
         return BOOL_TRUE;
}


bool_t
sniff_is_udp_packet(sniff_t *sniff)
{

        const struct pcap_pkthdr *header;
        const u_char *packet;
        const struct sniff_udp *udp;            /* The UDP header */
        const char *payload;                    /* Packet payload */

        int size_udp;
        int size_payload;

        if(!sniff->packet){
          sniff->is_udp_packet = EXTENDED_BOOL_FALSE;
          return BOOL_FALSE;
        }

        header = sniff->header;
        packet = sniff->packet;

        if(sniff->is_udp_packet != EXTENDED_BOOL_UNDEFINED){
          return (bool_t) sniff->is_udp_packet;
        }

        if(sniff_is_ip_packet(sniff) == BOOL_FALSE || sniff->ip_p != IPPROTO_UDP){
          sniff->is_udp_packet = EXTENDED_BOOL_FALSE;
          return BOOL_FALSE;
        }

        /* define/compute tcp header offset */
        udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + sniff->size_ip);
        size_udp = 8;
        sniff->port_src = udp->src_port;
        sniff->port_dst = udp->dst_port;

        /* define/compute tcp payload (segment) offset */
        sniff->payload = (u_char *)(packet + SIZE_ETHERNET + sniff->size_ip + size_udp);

        /* compute tcp payload (segment) size */
        sniff->size_payload = ntohs(sniff->ip_len) - (sniff->size_ip + size_udp);
        sniff->is_udp_packet = EXTENDED_BOOL_TRUE;
        return BOOL_TRUE;
}

u_char* sniff_get_payload(sniff_t* sniff)
{
  if(sniff_is_ip_packet(sniff) == BOOL_FALSE){
    return NULL;
  }
  if(sniff_is_udp_packet(sniff) == BOOL_TRUE || sniff_is_tcp_packet(sniff) == BOOL_TRUE){
    return sniff->payload;
  }
  return NULL;
}


void
sniff_rtp_packet(sniff_t* sniff)
{
  u_char* payload;
  sniff_rtp_t* rtph;
  if(sniff->rtp.analyzed != EXTENDED_BOOL_UNDEFINED){
    return;
  }

  rtph = payload = sniff_get_payload(sniff);
  if(payload == NULL){
    sniff->rtp.analyzed = EXTENDED_BOOL_FALSE;
    ERRORPRINT("Can not analyze RTP Packet, Payload is NULL");
    return;
  }

  sniff->rtp.analyzed     = EXTENDED_BOOL_TRUE;
  sniff->rtp.payload_type = rtph->PT;
  sniff->rtp.seq_num      = rtph->seq_num;
}


