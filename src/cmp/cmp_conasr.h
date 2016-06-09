#ifndef INCGUARD_MPT_COMPONENT_PACKET_CONASR_H_
#define INCGUARD_MPT_COMPONENT_PACKET_CONASR_H_
#include "lib_descs.h"

#define CMP_PUFFER_CONASR_SIZE 2
#define CMP_NAME_CONASR "Connection assigner component"
typedef struct cmp_conasr_struct_t
{
	//void       (*send)(int32_t);
	packet_t*  (*demand)();
	void*      (*start)();
	void*      (*stop)();
	void*      (*restart)();
	void       (*send)(packet_t*);
	//packet_t*   read_packets[32768];
	//barrier_t*  syncronizer;
}cmp_conasr_t;


void cmp_conasr_ctor();
void cmp_conasr_dtor();
cmp_conasr_t* get_cmp_conasr();

#endif //INCGUARD_MPT_COMPONENT_PACKET_CONASR_H_
