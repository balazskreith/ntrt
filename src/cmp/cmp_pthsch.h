#ifndef INCGUARD_MPT_COMPONENT_PACKET_PTHSCH_H_
#define INCGUARD_MPT_COMPONENT_PACKET_PTHSCH_H_
#include "lib_descs.h"

#define CMP_PUFFER_PTHSCH_SIZE 128
#define CMP_NAME_PTHSCH "Path scheduler component"
typedef struct cmp_pthsch_struct_t
{
	void       (*receiver)(packet_t*);
	void       (*send)(packet_t*);
	void*      (*start)();
	void*      (*stop)();
	void*      (*restart)();
	volatile bool_t is_started;
}cmp_pthsch_t;
//----------------------------------------------------
//packet scheduler procedure components end (cmp_ptrascv)
//----------------------------------------------------
void cmp_pthsch_ctor();
void cmp_pthsch_dtor();
cmp_pthsch_t* get_cmp_pthsch();

#endif //INCGUARD_MPT_COMPONENT_PACKET_PTHSCH_H_
