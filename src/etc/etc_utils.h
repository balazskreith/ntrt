#ifndef INCGUARD_NTRT_ETC_UTILS_H_
#define INCGUARD_NTRT_ETC_UTILS_H_

#include "lib_defs.h"
#include "lib_descs.h"

char_t* ltrim(char_t* s);
char_t* rtrim(char_t* s);
char_t* trim(char_t* s);
char_t* basename(char_t * string);
void mac_pton(char_t *src, char_t *dst);
void mac_ntop(char_t *src, char_t *dst);
int32_t check_loopback (char_t *address);
void set_ipstr(char_t* result, int32_t result_size, uint32_t* ip, uint8_t version);
void swap_ip6addr(byte_t* ip1, byte_t* ip2);
void str_repeat(char_t *result, char_t *str, int32_t num);
void cpystr_with_def(char_t *dest, char_t *src, char_t *def);
void bytes_ntoh(byte_t* bytes, int32_t length);
void bytes_hton(byte_t* bytes, int32_t length);
void cpy_bytes_ntoh(byte_t *dst, byte_t *src, int32_t length);
void cpy_bytes_hton(byte_t *dst, byte_t *src, int32_t length);
char_t** stumpy_strtbl(char_t **table, int32_t table_size, int32_t start_row, int32_t stumpy_length);

void setup_peer(struct sockaddr_in6 *peer, byte_t* ip_remote, int16_t port);

int32_t open_ipv6_dgram_socket();

#endif //INCGUARD_NTRT_ETC_UTILS_H_
