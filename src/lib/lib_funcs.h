/**
 * @file lib_funcs.h
 * @brief base functions for structures defined in the library
 * @author Bal�zs, Kreith; Debrecen, Hungary
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @date 2014.02.11
*/
#ifndef INCGUARD_NTRT_LIBRARY_FUNCTIONS_H_
#define INCGUARD_NTRT_LIBRARY_FUNCTIONS_H_

#include "inc_predefs.h"
#include "lib_defs.h"
#include "lib_descs.h"

/** \fn void execute_cback(callback_t*)
      \brief execute a callback
	  \param cback the subjected callback
  */
void	execute_cback(callback_t* cback);
void*	execute_rcback(rcallback_t* cback);

void datchain_add_item(datchain_t *chain, void *item);
void datchain_rem_item(datchain_t *item);

pathring_t* pathring_add(pathring_t** ring, path_t *path);
void *eventer(eventer_arg_t *eventer_arg);

command_t* cmdcat(command_t*, command_t*);
void ptrmov(void **dst, void **src);

#define get_array_item(ARRAYPTR, TYPE, ITEM_NUM) \
	(TYPE) ARRAYPTR->items[ITEM_NUM];

#define set_array_item(ARRAYPTR, ITEM_NUM, ITEM) \
	ARRAYPTR->items[ITEM_NUM] = ITEM;

#endif //INCGUARD_NTRT_LIBRARY_FUNCTIONS_H_
