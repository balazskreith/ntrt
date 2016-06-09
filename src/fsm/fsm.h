#ifndef INCGUARD_SYS_FSM_H_
#define INCGUARD_SYS_FSM_H_
#include "lib_descs.h"
#include "lib_threading.h"

typedef enum
{
	MPT_STATE_EXIST,
	MPT_STATE_RUN,
	MPT_STATE_HALT,
	MPT_STATE_CLIENT,
}fsm_states_t;

typedef enum
{
	MPT_EVENT_START             = 64,
	MPT_EVENT_STOP              = 68,
	MPT_EVENT_SETUP             = 72,
	MPT_EVENT_SHUTDOWN          = 76,

	MPT_EVENT_INTERFACE_UP      = 4,
	MPT_EVENT_INTERFACE_DOWN    = 8,
	MPT_EVENT_PATH_UP           = 12,
	MPT_EVENT_PATH_ADD          = 14,
	MPT_EVENT_PATH_DOWN         = 16,
	MPT_EVENT_CONNECTION_ADD    = 20,
	MPT_EVENT_CONNECTION_SAVE   = 28,
	MPT_EVENT_CONNECTION_RELOAD = 32,
	MPT_EVENT_CONNECTION_DELETE = 36,
	MPT_EVENT_NETWORK_ADD       = 40,

	MPT_EVENT_CREATE_CLIENT     = 128,
	MPT_EVENT_SET_CLIENT_SERVER_ADDR = 132,
	MPT_EVENT_SET_CLIENT_KEY    = 136,
	MPT_EVENT_SET_CLIENT_AUTH   = 140,
	MPT_EVENT_SET_CLIENT_PORT   = 144,
	MPT_EVENT_CLIENT_DO         = 148,
	MPT_EVENT_SET_CLIENT_SERVER_IP6 = 152,
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

//private tools
void fsm_str_state(int32_t state, char_t *dst);
void fsm_str_event(int32_t event, char_t *dst);

#endif /* INCGUARD_SYS_FSM_H_ */
