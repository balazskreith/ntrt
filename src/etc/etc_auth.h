/**
 * @file
 * @author Kelemen Tamas <kiskele@krc.hu>
 * @brief Macro and function definitons for auth.c
 */

#ifndef NTRT_ETC_AUTH_H_
#define NTRT_ETC_AUTH_H_
#include "lib_defs.h"
#include "lib_descs.h"

#define AUTH_NONE       0       ///< Don't use any authentication
#define AUTH_RAND       1       ///< Use random number to detect malicious packets
#define AUTH_SHA256     2       ///< Use random number and crypt the content of the whole packet with the secret key to an SHA hash

/**
 * Simple macro to convert a HEX digit (0-9a-fA-F) into decimal value
 */
#define HEX2DEC(x) (x>='0' && x<='9' ? x-'0' : (x>='a' && x<='f' ? x-'a'+10 : (x>='A' && x<='F' ? x-'A'+10 : 0)))

int sha256(char * buff, int size, char * key, int keysize, char * out, int * outsize);
void printHash(char * buff, int size);
int authSize(int authType);
int authTest(connection_t *conn, byte_t *buff, int32_t size);
void authSet(connection_t *conn, char * buff, int size);

void setKey(connection_t *conn, char * hex);
void saveKey(FILE * fd, connection_t *conn);
int keySize(int authType);

#endif //NTRT_ETC_AUTH_H_
