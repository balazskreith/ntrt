#ifndef INCGUARD_NTRT_COMPONENT_EXECUTOR_H_
#define INCGUARD_NTRT_COMPONENT_EXECUTOR_H_
#include "lib_descs.h"

#define CMP_PUFFER_CONASR_SIZE 2
#define CMP_NAME_CMDEXECUTOR "Command File Executor component"

typedef struct cmp_cmdexecutor_struct_t
{
	void*      (*start)();
	void*      (*stop)();
	void*      (*restart)();
}cmp_cmdexecutor_t;


void cmp_cmdexecutor_ctor();
void cmp_cmdexecutor_dtor();
cmp_cmdexecutor_t* get_cmp_cmdexecutor();

#endif //INCGUARD_NTRT_COMPONENT_EXECUTOR_H_
