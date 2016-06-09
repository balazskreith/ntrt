/*
 * sys_actions.h
 *
 *  Created on: May 1, 2014
 *      Author: balazs
 */

#ifndef INCGUARD_FSM_ACTIONS_H_
#define INCGUARD_FSM_ACTIONS_H_
#include "lib_descs.h"


void con_load_from_dir(char_t *dir);
void inf_load_from_dir(char_t *dir);
void con_load_from_file(char_t *filename);
void inf_load_from_file(char_t *filename);

void con_save_to_file(char_t *filename);

void path_status_change(path_t *path, uint8_t status);
bool_t path_has_already_changed(path_t *path, uint8_t *status);
void update_schtable();

#endif /* INCGUARD_FSM_ACTIONS_H_ */
