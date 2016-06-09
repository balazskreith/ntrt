#ifndef INCGUARD_CMP_PACKET_RECYCLE_H_
#define INCGUARD_CMP_PACKET_RECYCLE_H_
#include "lib_descs.h"

typedef struct _parkdat_struct_t
{
	packet_t *packets[32768];
	int32_t   limit;
}parkdat_t;


void cmplib_deinit();
void cmplib_init();
void clean_packet(packet_t*);
extern packet_t* (*prov_packet)();
extern void (*disp_packet)(packet_t*);

#endif /* INCGUARD_CMP_PACKET_RECYCLE_H_ */
