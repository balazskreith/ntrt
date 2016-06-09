#ifndef CMP_FRAME_PLAYOUT_SEQUENCE_RECEIVER_H_
#define CMP_FRAME_PLAYOUT_SEQUENCE_RECEIVER_H_
#include "cmp_defs.h"

//--------------------------------------------------
//fpsqrecver component begins (cmp_foo)
//----------------------------------------------------
#define CMP_MPRTP_RECEIVER "MPRTP receiver component"
typedef struct cmp_fpsqrecver_struct_t
{
	void             (*receiver)(packet_t*);
	void             (*send)(packet_t*);
}cmp_mprtprecver_t;
//--------------------------------------------------
//foo component ends (cmp_foo)
//----------------------------------------------------


void cmp_mprtprecver_ctor();
void cmp_fpsqrecver_dtor();
cmp_mprtprecver_t* get_cmp_mprtprecver();

#endif /* CMP_FRAME_PLAYOUT_SEQUENCE_RECEIVER_H_ */
