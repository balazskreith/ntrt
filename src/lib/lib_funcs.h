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

datchain_t* datchain_prepend(datchain_t *chain, ptr_t item);
datchain_t* datchain_append(datchain_t *chain, ptr_t item);
void datchain_foreach(datchain_t* chain, void (*process)(datchain_t*, ptr_t), ptr_t data);

slist_t* slist_append(slist_t *slist, ptr_t item);
slist_t* slist_prepend(slist_t* slist, ptr_t item);
void slist_foreach(slist_t* slist, void (*process)(slist_t*, ptr_t), ptr_t data);

dlist_t* dlist_append(dlist_t *dlist, ptr_t item);
dlist_t* dlist_prepend(dlist_t* dlist, ptr_t item);
void dlist_foreach(dlist_t* dlist, void (*process)(dlist_t*, ptr_t), ptr_t data);

void *eventer(eventer_arg_t *eventer_arg);

void ptrmov(void **dst, void **src);

#define get_array_item(ARRAYPTR, TYPE, ITEM_NUM) \
	(TYPE) ARRAYPTR->items[ITEM_NUM];

#define set_array_item(ARRAYPTR, ITEM_NUM, ITEM) \
	ARRAYPTR->items[ITEM_NUM] = ITEM;

#endif //INCGUARD_NTRT_LIBRARY_FUNCTIONS_H_
