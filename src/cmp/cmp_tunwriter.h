#ifndef INCGUARD_CMP_TUNWRITER_H_
#define INCGUARD_CMP_TUNWRITER_H_
#include "lib_descs.h"

//--------------------------------------------------
//tunwriter components begin (cmp_tunwriter)
//----------------------------------------------------
#define CMP_NAME_TUNWRITER "Tunnel writer component"
typedef struct cmp_tunwriter_struct_t
{
	void             (*send)(packet_t*);
	void             (*receiver)(packet_t*);
	void             (*start)();
}cmp_tunwriter_t;
//--------------------------------------------------
//tunwriter components end (cmp_tunwriter)
//----------------------------------------------------


void cmp_tunwriter_ctor();
void cmp_tunwriter_dtor();
cmp_tunwriter_t* get_cmp_tunwriter();



#endif /* INCGUARD_CMP_TUNWRITER_H_ */
