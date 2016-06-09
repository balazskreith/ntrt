#ifndef INCGUARD_CMP_PSENDER_ADAPTIVE_H_
#define INCGUARD_CMP_PSENDER_ADAPTIVE_H_
#include "lib_descs.h"
#include "cmp_defs.h"

#define CMP_PUFFER_DATSENDER_SIZE 128

#define CMP_NAME_PSENDER "Packet sender component"
typedef struct cmp_psender_struct_t
{
	void         (*datpck_receiver)(packet_t*);
	void         (*cmdpck_receiver)(packet_t*);
	void*        (*start)();
	void*        (*restart)();
	void*        (*stop)();
	void         (*send)(packet_t*);
}cmp_psender_t;

void cmp_psender_ctor();
void cmp_psender_dtor();
cmp_psender_t* get_cmp_psender();

#endif /* INCGUARD_CMP_PSENDER_ADAPTIVE_H_ */
