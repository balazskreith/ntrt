#ifndef INCGUARD_SYS_CTRLS_H_
#define INCGUARD_SYS_CTRLS_H_
#include "lib_descs.h"
#include "fsm.h"

//Inner controls
command_t* make_scall_cmd(char_t *cmd, void (*)());
command_t* make_event_cmd(int32_t event, void *arg);
command_t* make_cmd_opcall(char_t *cmd_name, char_t *opcall_cmd);
command_t* make_command_from_request(request_t *request);

//Outer controls
request_t* make_request_path_up(path_t *path);
request_t* make_request_path_down(path_t *path);

#endif /* INCGUARD_SYS_CMDS_H_ */


