/**
 * @file
 * @brief Public domain implementations of in-place string trim functions
 * @author Michael Burr michael.burr@nth-element.com
 * @date 2010
*/
#include "etc_utils.h"
#include <ctype.h>
#include <string.h>
#include "lib_defs.h"
#include "inc_inet.h"
#include "inc_predefs.h"
#include "inc_texts.h"
#include "dmap.h"

/**
 * Strip whitespace from the beginning of a string
 */
char_t* ltrim(char_t* s)
{
    char* newstart = s;

    while (isspace( *newstart)) {
        ++newstart;
    }

    // newstart points to first non-whitespace char (which might be '\0')
    memmove( s, newstart, strlen( newstart) + 1); // don't forget to move the end '\0'

    return s;
}

/**
 * Strip whitespace from the end of a string
 */
char_t* rtrim( char_t* s)
{
    char* end = s + strlen( s);

    // find the last non-whitespace character
    while ((end != s) && isspace( *(end-1))) {
            --end;
    }

    // at this point either (end == s) and s is either empty or all whitespace
    //      so it needs to be made empty, or
    //      end points just past the last non-whitespace character (it might point
    //      at the '\0' terminator, in which case there's no problem writing
    //      another there).
    *end = '\0';

    return s;
}

/**
 * Strip whitespace from the beginning and end of a string
 */
char_t*  trim( char_t* s)
{
    return rtrim( ltrim( s));
}


/**
 * Strip down path and get filename
 * @author Kelemen Tamas
 */
char_t * basename(char_t* string)
{
    char * p = string;
    char * ret = NULL;
    while(*(p++)) {
        if(*p == '/') ret = p+1;
    }
    return ret;
}

void swap_ip6addr(byte_t* ip1, byte_t* ip2)
{
	byte_t ipt[SIZE_IN6ADDR];
	memcpy(ipt, ip1, SIZE_IN6ADDR);
	memcpy(ip1, ip2, SIZE_IN6ADDR);
	memcpy(ip2, ipt, SIZE_IN6ADDR);
}

void swap_nets(network_t *net)
{
	byte_t temp[SIZE_IN6ADDR];
	uint32_t temp_int;
	memcpy(temp, net->destination, SIZE_IN6ADDR);
	temp_int = net->destination_prefix_length;
	memcpy(net->destination, net->source, SIZE_IN6ADDR);
	net->destination_prefix_length = net->source_prefix_length;
	memcpy(net->source, temp, SIZE_IN6ADDR);
	net->source_prefix_length = temp_int;
}

/**
 * Convert the MAC address string (':' or '-' byte delimiter) to binary form
 */
void mac_pton(char_t *src, char_t *dst)
{
    int32_t slen;
    uint8_t *p, *d, c;
    memset(dst, 0, 6);

    if ((slen=strlen(src)) != 17) return; // Bad format input MAC string
    p = (uint8_t *)src; d=(uint8_t *)dst;
    while(p<p+slen) {
        c = toupper(*p);
        if (c >= 'A' && c<='F') {
            *d =(c -'A'+10)<<4;
        }
        else *d = ( c - '0')<<4;
        p++;
        c = toupper(*p);
        if (c >= 'A' && c<='F') {
            *d |= (c -'A'+10);
        }
        else *d |= (c - '0');

        p++;
        if ((*p == ':')||(*p == '-')) {p++ ;} else return;
        d++;
    }
    return;
}

void mac_ntop(char_t *src, char_t *dst)
{
    sprintf(dst, "%02X:%02X:%02X:%02X:%02X:%02X", src[0], src[1],src[2],src[3],src[4],src[5]);
}


/**
 * Check if the address is loopback (Ret. 1=yes)
 */
int32_t check_loopback (char_t *address)
{
    uint8_t   lo4[SIZE_IN6ADDR], lo6[SIZE_IN6ADDR];
    inet_pton(AF_INET6, "::FFFF:127.0.0.1", lo4);
    inet_pton(AF_INET6, "::1", lo6);
    if (!memcmp(lo4, address, SIZE_IN6ADDR)) return(4);
    if (!memcmp(lo6, address, SIZE_IN6ADDR)) return(6);
    return(0);
}

void set_ipstr(char_t* result, int32_t result_size, uint32_t* ip, uint8_t version)
{
	 if (version == 6) {
		inet_ntop(AF_INET6, ip, result, result_size);
	} else if(version == 4) {
		inet_ntop(AF_INET, &ip[3], result, result_size);
	}
}

void set_pathstr(char_t *result, path_t *path)
{
	if(path == NULL){
		sprintf(result, "Not exists");
		return;
	}
	char_t     ip_local[255], ip_remote[255];
	set_ipstr(ip_local,  128, path->ip_local, path->ip_version);
	set_ipstr(ip_remote, 128, path->ip_remote, path->ip_version);
	sprintf(result, "%s -> %s", ip_local, ip_remote);
}

void str_repeat(char_t *result, char_t *str, int32_t num)
{
	int i;
	for (i = 0; i < num; i++) {
		strcat(result, str);
	}
}


void cpystr_with_def(char_t *dest, char_t *src, char_t *def)
{
	if(dest == NULL){
		return;
	}
	if(src == NULL && def == NULL){
		return;
	}

	if(src != NULL){
		strcpy(dest, src);
	}else{
		strcpy(dest, def);
	}
}

void  bytes_ntoh(byte_t* bytes, int32_t length)
{
	int32_t index;
	int32_t integer;
	/*byte htonl*/
	//*length must by dividable by 32
	for(index = 0; index < length; index += 4)
	{
		memcpy(&integer, &bytes[index], 4);
		integer = ntohl(integer);
		memcpy(&bytes[index], &integer, 4);
	}
}



void  bytes_hton(byte_t* bytes, int32_t length)
{
	int32_t index;
	int32_t integer;
	/*byte htonl*/
	//*length must by dividable by 32
	for(index = 0; index < length; index += 4)
	{
		memcpy(&integer, &bytes[index], 4);
		integer = htonl(integer);
		memcpy(&bytes[index], &integer, 4);
	}
}

void  cpy_bytes_ntoh(byte_t *dst, byte_t *src, int32_t length)
{
	int32_t index;
	int32_t integer;
	for(index = 0; index < length; index += 4)
	{
		integer = ntohl(*((uint32_t*)(src + index)));
		memcpy(dst + index, &integer , 4);
	}

}

void  cpy_bytes_hton(byte_t *dst, byte_t *src, int32_t length)
{
	int32_t index;
	int32_t integer;
	for(index = 0; index < length; index += 4)
	{
		integer = htonl(*((uint32_t*)(src + index)));
		memcpy(dst + index, &integer , 4);
	}
}

char_t** stumpy_strtbl(char_t **table, int32_t table_size, int32_t start_row, int32_t stumpy_length)
{
	char_t **result;
	int32_t i,j, actual_row = 0, end_row = start_row + stumpy_length;

	result = (char_t**) malloc(sizeof(char_t) * table_size - stumpy_length);
	for(i = 0; i < table_size; ++i){
		if(start_row <= i && i <= end_row){
			continue;
		}
		for(j = 0; table[i][j] != '\0'; ++j);

		for(j = 0; table[i][j] != '\0'; ++j);{

		}
	}
}

void setup_peer_for_path(struct sockaddr_in6 *peer, path_t* path)
{
	connection_t *con;
	dmap_rdlock_table_con();
	con = dmap_get_con(path->con_dmap_id);
	setup_peer(peer, (byte_t*) path->ip_remote, con->port_remote);
	dmap_rdunlock_table_con();
}

void setup_peer(struct sockaddr_in6 *peer, byte_t* ip_remote, int16_t port)
{
	peer->sin6_port   = htons(port);
	peer->sin6_family = AF_INET6;
	memcpy(&(peer->sin6_addr), ip_remote, SIZE_IN6ADDR);
}

int32_t open_ipv6_dgram_socket()
{

	int32_t result;
	if ((result = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){
		EXERROR("Socket creation error.", EXIT_FAILURE);
	}

#ifdef MPT_COM_IPV6_ONLY
	setsockopt(cmp->sockid, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof off);
#endif

	return result;
}


