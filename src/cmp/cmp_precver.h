#ifndef INCGUARD_MPT_COMPONENT_precver_H_
#define INCGUARD_MPT_COMPONENT_precver_H_
#include "cmp_predefs.h"
#include "lib_descs.h"

#define CMP_PUFFER_TUNWRITER_SIZE 128
#define CMP_PUFFER_PCKROUTER_SIZE 32
//--------------------------------------------------
//precver components begin (cmp_precver)
//----------------------------------------------------
#define CMP_NAME_PRECVER "Packet receiver component"
typedef struct cmp_precver_struct_t
{
	void      (*receiver)(packet_t*);
	void 	  (*send_diagpck)(packet_t*);
	void 	  (*send_cmdpck)(packet_t*);
	void      (*send_locpck)(packet_t*);
	void      (*send_datpck)(packet_t*);
	void*     (*start)(connection_t*);
	void*     (*stop)(connection_t*);
}cmp_precver_t;
//--------------------------------------------------
//precver components end (cmp_precver)
//----------------------------------------------------

void cmp_precver_ctor();
void cmp_precver_dtor();
cmp_precver_t* get_cmp_precver();



#endif //INCGUARD_MPT_COMPONENT_precver_H_
