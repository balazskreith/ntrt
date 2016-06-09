#ifndef CMP_FRAME_PLAYOUT_SEQUENCE_SENDER_H_
#define CMP_FRAME_PLAYOUT_SEQUENCE_SENDER_H_
#include "cmp_defs.h"

//--------------------------------------------------
//mprtpsender component begins (cmp_foo)
//----------------------------------------------------
#define CMP_MPRTP_SENDER "MPRTP sender component"
typedef struct cmp_mprtpsender_struct_t
{
	void             (*receiver)(packet_t*);
	void             (*send)(packet_t*);
}cmp_mprtpsender_t;
//--------------------------------------------------
//foo component ends (cmp_foo)
//----------------------------------------------------


void cmp_mprtpsender_ctor();
void cmp_mprtpsender_dtor();
cmp_mprtpsender_t* get_cmp_mprtpsender();

#endif /* CMP_FRAME_PLAYOUT_SEQUENCE_SENDER_H_ */
