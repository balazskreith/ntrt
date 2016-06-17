#ifndef INCGUARD_NTRT_COMPONENT_EVALUATOR_H_
#define INCGUARD_NTRT_COMPONENT_EVALUATOR_H_
#include "lib_descs.h"

#define CMP_PUFFER_CONASR_SIZE 2
#define CMP_NAME_EVALUATOR "Sniff Evaluator component"

typedef struct cmp_evaluator_item_struct_t{
  slist_t*  evaluators;
  int32_t   feature_num;
}cmp_evaluator_item_t;

typedef struct cmp_evaluator_struct_t
{
	//void       (*send)(int32_t);
	void*      (*start)();
	void*      (*stop)();
	void*      (*restart)();
        void       (*sniff_receiver)(sniff_t*);
        void       (*record_receiver)(record_t*);
        record_t*  (*demand_record)();
	void       (*send)(record_t*);
	cmp_evaluator_item_t evaluators[NTRT_MAX_FEATURES_NUM];
}cmp_evaluator_t;


void cmp_evaluator_ctor();
void cmp_evaluator_dtor();
cmp_evaluator_t* get_cmp_evaluator();

#endif //INCGUARD_NTRT_COMPONENT_EVALUATOR_H_
