/*
 * sys_prints.h
 *
 *  Created on: Mar 11, 2014
 *      Author: balazs
 */

#ifndef INCGUARD_SYS_PRINTS_H_
#define INCGUARD_SYS_PRINTS_H_
#include "lib_descs.h"

void* sys_stdout_cmd(const char* format, ...);

void* sys_print_all();

void* sys_print_all_tun();
void* sys_print_tun(tunnel_t *tun);

void* sys_print_all_con();
void* sys_print_con(connection_t *con);

void* sys_print_all_inf();
void* sys_print_inf(interface_t *inf);

void* sys_print_all_net();
void* sys_print_net(network_t *net);

void* sys_print_all_pth();
void* sys_print_pth(path_t *pth);

#endif /* INCGUARD_SYS_PRINTS_H_ */
