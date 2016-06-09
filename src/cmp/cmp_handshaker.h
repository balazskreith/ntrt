/*
 * cmp_handshaker.h
 *
 *  Created on: Mar 29, 2014
 *      Author: balazs
 */

#ifndef INCGUARD_CMP_HANDSHAKER_H_
#define INCGUARD_CMP_HANDSHAKER_H_
#include "lib_defs.h"
#include "lib_descs.h"

typedef struct cmp_handshaker_struct_t
{
	void   (*pck_receiver)(packet_t*);
	//void   (*req_receiver)(byte_t, byte_t, connection_t*, ...);
	void   (*req_receiver)(request_t*);
	void   (*send_pck)(packet_t*);
	void   (*send_cmd)(command_t*);
	void*  (*start)();
	void*  (*stop)();
}cmp_handshaker_t;

void cmp_handshaker_ctor();
void cmp_handshaker_dtor();
cmp_handshaker_t* get_cmp_handshaker();


#endif /* INCGUARD_CMP_HANDSHAKER_H_ */
