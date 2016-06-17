#ifndef INCGUARD_CMP_COMPONENT_RECORDER_H_
#define INCGUARD_CMP_COMPONENT_RECORDER_H_
#include "lib_descs.h"
#include "cmp_defs.h"

#define CMP_PUFFER_DATSENDER_SIZE 128

#define CMP_NAME_RECORDER "Recorder component"
typedef struct cmp_recorder_struct_t
{
        void         (*requester)(int32_t, record_t*);
	void*        (*start)();
	void*        (*restart)();
	void*        (*stop)();
}cmp_recorder_t;

void cmp_recorder_ctor();
void cmp_recorder_dtor();
cmp_recorder_t* get_cmp_recorder();

#endif /* INCGUARD_CMP_RECORDER_H_ */
