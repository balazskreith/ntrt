#ifndef INCGUARD_MPT_COMPONENT_COMMAND_EXECUTION_H_
#define INCGUARD_MPT_COMPONENT_COMMAND_EXECUTION_H_
#include "cmp_predefs.h"
#include "lib_descs.h"

#define MPT_CMP_CMDPUFFFER_LENGTH 32
#define MPT_CMP_CMDHANDSHAKE_TIMEOUT_IN_MS 200
#define MPT_CMP_CMDHANDSHAKE_MAX_RETRY 5
#define MPT_COMMAND_TIMEOUT_IN_SECOND 60

typedef struct cmp_cmdexetor_struct_t
{
	void   (*pck_receiver)(packet_t*);
	void   (*receiver)(command_t*);
	void   (*send)(request_t*);
	void*  (*start)();
	void*  (*stop)();
}cmp_cmdexetor_t;

void cmp_cmdexetor_ctor();
void cmp_cmdexetor_dtor();
cmp_cmdexetor_t* get_cmp_cmdexetor();

#endif //INCGUARD_MPT_CCOMPONENT_COMMAND_EXECUTION_H_
