/**
 * @file
 * @author Kelemen Tamas <kiskele@krc.hu>
 * @brief Authentication and encryption functions
 */

#include "etc_auth.h"
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include "lib_defs.h"


/**
 * Generate SHA-256 HASH
 *
 * @param[in]  buff          Generate hash from this string
 * @param[in]  size          Size of the buff parameter
 * @param[in]  key           Append this secret key to buff if larger then 0 byte
 * @param[in]  keysize       Size of the key parameter
 * @param[out] out           Put the generated hash key to this value
 * @param[out] outsize       Length of the generated hash
 *
 * @return This function always return true
 */
int sha256(char *buff, int size, char * key, int keysize, char * out, int * outsize) {
    EVP_MD_CTX * mdctx = NULL;
    const EVP_MD * md;

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("sha256");
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit(mdctx, md);
    EVP_DigestUpdate(mdctx, buff, size);
    if(keysize) EVP_DigestUpdate(mdctx, key, keysize);
    EVP_DigestFinal(mdctx, (unsigned char *)out, (unsigned int *)outsize);

    EVP_MD_CTX_destroy(mdctx);
    EVP_cleanup();
    return 0;
}


/**
 * Display a HASH value in HEX format on the standard output (useful for testing)
 *
 * @param buff     HASH value
 * @param size     Size of the buff parameter
 */
void printHash(char * buff, int size) {
    int i;

    for(i=0; i<size; i++) printf("%02X", (unsigned char)buff[i]);
    printf("\n");
}

/**
 * Returns how large authentication header used in the packets for a specified auth. type
 *
 * @param authType      Used authentication type. See @ref AuthTypes for the complete list
 * @return              The authentication header length
 */
int authSize(int authType) {
    switch(authType) {
        case AUTH_NONE:   return 0;
        case AUTH_RAND:   return 4;
        case AUTH_SHA256: return 36;    // authSize(AUTH_RAND) + keySize(AUTH_SHA256)

        default:          return 0;
    }
}


/**
 * Check the received packet whether the authentication data is correct
 *
 * @param conn  The connection to the actual peer
 * @param buff  Received data
 * @param size  Size of the data
 * @return False if the packet data
 *
 */
int authTest(connection_t *conn, byte_t* buff, int32_t size) {
    int sh = authSize(buff[2]);

    switch(buff[2]) {
        case AUTH_NONE:
            break;
        case AUTH_SHA256:
        case AUTH_RAND:
            if(buff[sh+4] == 2) {
                // save key when first response received
                memcpy(&conn->waitrand, &buff[4], authSize(AUTH_RAND));
            } else if(buff[sh+4] > 1 && memcmp(&conn->waitrand, &buff[4], authSize(AUTH_RAND)) != 0) {
                // auth mismatch
                return 0;
            }
            break;
        default:
            return 0;
    }

    if(buff[2] == AUTH_SHA256) {
        char key[32];
        int keysize = 32;

        // save key
        memcpy(key, &buff[8], 32);

        // clear memory
        memset(&buff[8], 0, 32);

        sha256(buff, size, conn->auth_key, keySize(conn->auth_type), &buff[8], &keysize);

        // check hash
        if(memcmp(key, &buff[8], 32)) {
            // restore key
            memcpy(&buff[8], key, 32);

            // return with incorrect key error
            return 0;
        }
    }

    return 1;
}


/**
 * Sets the correct authentication data in the packet before sending it
 *
 * @param conn  The connection to the actual peer
 * @param buff  The packet to send
 * @param size  Size of the packet
 *
 */
void authSet(connection_t *conn, char * buff, int size) {
    int sh = authSize(buff[2]);

    switch(buff[2]) {
        case AUTH_SHA256:
        case AUTH_RAND:
            if(buff[sh+4] == 1) {
                // Initialize waitrand
                memset(&conn->waitrand, 0, authSize(AUTH_RAND));
            } else if(buff[sh+4] == 2) { // incremented round number
                // Generate random number
                uint32_t random;
                srand(time(NULL));
                random = rand();
                memcpy(&conn->waitrand, &random, authSize(AUTH_RAND));
            }

            // set random number in packet
            memcpy(&buff[4], &conn->waitrand, authSize(AUTH_RAND));

            break;
        default:
            memset(&buff[4], 0, sh);
    }

    if(buff[2] == AUTH_SHA256) {
        int hashsize = 0;
        memset(&buff[8], 0, 32);
        sha256(buff, size, conn->auth_key, keySize(conn->auth_type), &buff[8], &hashsize);
    }
}

/**
 * Converts authentication key from HEX to binary and saves it in the connection struct.
 * @param conn  The connection to use
 * @param hex   The authentication key stored in HEX format
 * @pre The auth_type needed to be set before using this function
 */
void setKey(connection_t *conn, char * hex) {
    int i;
    for(i=0; i<keySize(conn->auth_type); i++) {
        conn->auth_key[i] = HEX2DEC(hex[2*i]) << 4 | HEX2DEC(hex[2*i+1]);
    }
}

/**
 * Save authentication key in HEX format to the configuration file
 *
 * @param fd    The FILE pointer to the configuration file
 * @param conn  The connection which contains the authentication datas
 */
void saveKey(FILE * fd, connection_t *conn) {
    int i;

    switch(conn->auth_type) {
        case AUTH_SHA256:
            for(i=0; i<keySize(conn->auth_type); i++) {
                fprintf(fd, "%02X", (unsigned char)conn->auth_key[i]);
            }
            break;
        default:
            fprintf(fd, "0");
    }
    fprintf(fd, "\t# AUTH. KEY\n");
}

/**
 * Returns how large unique key is used for the authentication
 * @return Size of the auth_key
 */
int keySize(int authType) {
    switch(authType) {
        case AUTH_NONE:   return 0;
        case AUTH_RAND:   return 0;
        case AUTH_SHA256: return 32;

        default:          return 0;
    }
}
