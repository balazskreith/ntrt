#ifndef INCGUARD_OPCALL_H_
#define INCGUARD_OPCALL_H_
#include "lib_descs.h"

//void opcall_create_tunnel(tunnel_t*);
//void* opcall_add_route(path_t *path);
//void* opcall_del_route(path_t *path);
void* opcall_change_route(path_t *path, char_t* op);
void* opcall_cmd_system(char_t*);

#endif /* INCGUARD_OPCALL_H_ */
