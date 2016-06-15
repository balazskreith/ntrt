#ifndef INCGUARD_SYS_FSM_H_
#define INCGUARD_SYS_FSM_H_
#include "lib_descs.h"
#include "lib_threading.h"

typedef enum
{
	NTRT_STATE_RUN,
	NTRT_STATE_HALT,
}fsm_states_t;

typedef enum
{
	NTRT_EVENT_START             = 64,
	NTRT_EVENT_STOP              = 68,
	NTRT_EVENT_SETUP             = 72,
	NTRT_EVENT_SHUTDOWN          = 76,
}fsm_events_t;

typedef struct fsm_mpt_struct_t
{
	fsm_states_t         current;
	char_t               state_str[255];
	char_t               event_str[255];
	void               (*fire)(int32_t, void*);
	signal_t            *signal;
}fsm_t;

void fsm_ctor();
void fsm_dtor();
fsm_t* get_fsm();
void fsm_kill();

//private tools
void fsm_str_state(int32_t state, char_t *dst);
void fsm_str_event(int32_t event, char_t *dst);

#endif /* INCGUARD_SYS_FSM_H_ */
