#ifndef INCGUARD_NTRT_COMPONENT_GROUPCOUNTER_H_
#define INCGUARD_NTRT_COMPONENT_GROUPCOUNTER_H_
#include "lib_descs.h"
#include "lib_puffers.h"

#define CMP_PUFFER_CONASR_SIZE 2
#define CMP_NAME_GROUPCOUNTER "Groupcounter component"

typedef struct{
  slist_t*        groupcounters;
}groupcounters_holder_t;

typedef struct cmp_groupcounter_struct_t
{
	//void        (*send)(int32_t);
	void*                      (*start)();
	void*                      (*stop)();
	void*                      (*restart)();
        void                       (*sniff_receiver)(sniff_t*,int32_t listener_id);
        void                       (*record_requester)(int32_t listener_id, record_t* result);
	mutex_t*                     mutex;
	groupcounters_holder_t       holders[NTRT_MAX_PCAPLS_NUM];
	int32_t                      timeout_treshold;
}cmp_groupcounter_t;


void cmp_groupcounter_ctor();
void cmp_groupcounter_dtor();
cmp_groupcounter_t* get_cmp_groupcounter();

#endif //INCGUARD_NTRT_COMPONENT_ACCUMULATOR_H_
