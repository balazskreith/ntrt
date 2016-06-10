#ifndef INCGUARD_NTRT_COMPONENT_SCREENER_H_
#define INCGUARD_NTRT_COMPONENT_SCREENER_H_
#include "lib_descs.h"

typedef struct cmp_screener_struct_t
{
	void       (*receiver)(packet_t*);
	void       (*send_pck)(packet_t*);
	void       (*send_cmd)(command_t*);
	void       (*send_req)(request_t*);
	void*      (*start)();
	void*      (*stop)();
}cmp_screener_t;


void cmp_screener_ctor();
void cmp_screener_dtor();
cmp_screener_t* get_cmp_screener();


#endif //INCGUARD_NTRT_COMPONENT_SCREENER_H_
