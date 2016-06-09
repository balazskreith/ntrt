#ifndef INCGUARD_CMP_COMMAND_LINE_INTERFACE_H_
#define INCGUARD_CMP_COMMAND_LINE_INTERFACE_H_
#include "lib_descs.h"

//--------------------------------------------------
//cli components begin (cmp_cli)
//----------------------------------------------------
#define CMP_NAME_TUNREADER "Tunnel reader component"
typedef struct cmp_cli_struct_t
{
	void             (*send)(command_t*);
	void             (*pck_receiver)(packet_t*);
	void             (*str_receiver)(char_t*);
}cmp_cli_t;
//--------------------------------------------------
//cli components end (cmp_cli)
//----------------------------------------------------


void cmp_cli_ctor();
void cmp_cli_dtor();
cmp_cli_t* get_cmp_cli();



#endif /* INCGUARD_CMP_COMMAND_LINE_INTERFACE_H_ */
