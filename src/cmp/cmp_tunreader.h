#ifndef INCGUARD_CMP_TUNREADER_H_
#define INCGUARD_CMP_TUNREADER_H_
#include "lib_descs.h"

//--------------------------------------------------
//tunreader components begin (cmp_tunreader)
//----------------------------------------------------
#define CMP_NAME_TUNREADER "Tunnel reader component"
typedef struct cmp_tunreader_struct_t
{
	void*            (*start)();
	void*            (*stop)();
	packet_t*        (*supplier)();
	void             (*receiver)(packet_t*);
}cmp_tunreader_t;
//--------------------------------------------------
//tunreader components end (cmp_tunreader)
//----------------------------------------------------


void cmp_tunreader_ctor();
void cmp_tunreader_dtor();
cmp_tunreader_t* get_cmp_tunreader();



#endif /* INCGUARD_CMP_TUNREADER_H_ */
