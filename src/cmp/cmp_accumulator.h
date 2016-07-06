#ifndef INCGUARD_NTRT_COMPONENT_ACCUMULATOR_H_
#define INCGUARD_NTRT_COMPONENT_ACCUMULATOR_H_
#include "lib_descs.h"
#include "lib_puffers.h"

#define CMP_PUFFER_CONASR_SIZE 2
#define CMP_NAME_ACCUMULATOR "Record Accumulator component"

typedef struct cmp_accumulator_item_struct_t{
  datapuffer_t* records;
  record_t      result;
  int32_t       length;
  record_t      total;
}accumulator_t;

typedef struct cmp_accumulator_struct_t
{
	//void        (*send)(int32_t);
	void*       (*start)();
	void*       (*stop)();
	void*       (*restart)();
        void        (*record_receiver)(record_t*);
        void        (*features_requester)(int32_t listener_id, record_t* result, record_t* total);
        record_t*   (*record)();
	void        (*send_record)(record_t*);
	mutex_t*      mutex;
	accumulator_t accumulators[NTRT_MAX_PCAPLS_NUM];
	int32_t       accumulation_time;
}cmp_accumulator_t;


void cmp_accumulator_ctor();
void cmp_accumulator_dtor();
cmp_accumulator_t* get_cmp_accumulator();

#endif //INCGUARD_NTRT_COMPONENT_ACCUMULATOR_H_
