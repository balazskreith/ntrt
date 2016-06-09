#ifndef INCGUARD_MPT_DMAP_DMAP_H_
#define INCGUARD_MPT_DMAP_DMAP_H_
#include "lib_defs.h"
#include "lib_descs.h"
#include "lib_threading.h"
#include "dmap_defs.h"

#define MPT_DMAP_TABLE_ETC_NUM 32
//errors
#define DEVCLEGO_ERROR_TABLE_IS_FULL -1
#define DEVCLEGO_DMAP_ERROR_ITEM_NOT_FOUND -1
#define DEVCLEGO_DMAP_ERROR_ADD -2

//dmap creation
void dmap_init();
void dmap_deinit();
void dmap_itr_do(bool_t (*dmap_iterator)(int32_t*, void*), void* (*action)(void*));

dmap_row_t* _dmap_row_ctor_();
void        _dmap_row_dtor(dmap_row_t *row);
dmap_row_t* _make_dmap_row(void *item, void (*dtor)(void*));
void        _dmap_table_init(dmap_table_t *table, int32_t size, char_t *name)      __attribute__((nonnull (1)));
void        _dmap_table_deinit(dmap_table_t *table);
bool_t      _dmap_itr_table(dmap_table_t *table, int32_t *index, dmap_row_t **row) __attribute__((nonnull (1,2,3)));
void*       _dmap_get_item(dmap_table_t *table, int32_t dmap_index)                __attribute__((nonnull (1)));
void        _dmap_rem_item(dmap_table_t *table, void *item)                        __attribute__((nonnull (1)));
void        _dmap_rem(dmap_table_t *table, int32_t dmap_index)                     __attribute__((nonnull (1)));
int32_t     _dmap_add_row(dmap_table_t *table, dmap_row_t *row)                    __attribute__((nonnull (1)));


//------------------------------------------------------------------------------------------------
//------------------------------------ Table declarations ----------------------------------------
//------------------------------------------------------------------------------------------------
void dmap_rem_con_and_close(connection_t*);
DMAP_DECL_SPECT_TABLE(
	connection_t,		   /*type of the items stored in the table*/
	dmap_get_table_con,	   /*name of the process gets the table*/
	dmap_get_con,		   /*name of the process gets an item from the table by index*/
	dmap_add_con,		   /*name of the process adds an item and returns with its index*/
	dmap_rem_con_byindex,  /*name of the process remove an item from the table by index*/
	dmap_rem_con, 		   /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_con,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_con,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_con,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_con,/*name of the process unlock the table from writing*/
	dmap_itr_table_con	   /*name of the process iterate the table*/
);

void dmap_rem_tun_and_close(tunnel_t*);
DMAP_DECL_SPECT_TABLE(
	tunnel_t,		  	   /*type of the items stored in the table*/
	dmap_get_table_tun,	   /*name of the process gets the table*/
	dmap_get_tun,		   /*name of the process gets an item from the table by index*/
	dmap_add_tun,		   /*name of the process adds an item and returns with its index*/
	dmap_rem_tun_byindex,  /*name of the process remove an item from the table by index*/
	dmap_rem_tun, 		   /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_tun,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_tun,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_tun,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_tun,/*name of the process unlock the table from writing*/
	dmap_itr_table_tun	   /*name of the process iterate the table*/
);

DMAP_DECL_SPECT_TABLE(
	interface_t,		   /*type of the items stored in the table*/
	dmap_get_table_inf,	   /*name of the process gets the table*/
	dmap_get_inf,		   /*name of the process gets an item from the table by index*/
	dmap_add_inf,		   /*name of the process adds an item and returns with its index*/
	dmap_rem_inf_byindex,  /*name of the process remove an item from the table by index*/
	dmap_rem_inf, 		   /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_inf,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_inf,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_inf,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_inf,/*name of the process unlock the table from writing*/
	dmap_itr_table_inf	   /*name of the process iterate the table*/
);

DMAP_DECL_SPECT_TABLE(
	network_t,		   	   /*type of the items stored in the table*/
	dmap_get_table_net,	   /*name of the process gets the table*/
	dmap_get_net,		   /*name of the process gets an item from the table by index*/
	dmap_add_net,		   /*name of the process adds an item and returns with its index*/
	dmap_rem_net_byindex,  /*name of the process remove an item from the table by index*/
	dmap_rem_net, 		   /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_net,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_net,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_net,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_net,/*name of the process unlock the table from writing*/
	dmap_itr_table_net	   /*name of the process iterate the table*/
);

DMAP_DECL_SPECT_TABLE(
	path_t,				   /*type of the items stored in the table*/
	dmap_get_table_pth,	   /*name of the process gets the table*/
	dmap_get_pth,		   /*name of the process gets an item from the table by index*/
	dmap_add_pth,		   /*name of the process adds an item and returns with its index*/
	dmap_rem_pth_byindex,  /*name of the process remove an item from the table by index*/
	dmap_rem_pth, 		   /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_pth,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_pth,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_pth,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_pth,/*name of the process unlock the table from writing*/
	dmap_itr_table_pth	   /*name of the process iterate the table*/
);

DMAP_DECL_SPECT_TABLE(
	thread_t,			   /*type of the items stored in the table*/
	dmap_get_table_thr,	   /*name of the process gets the table*/
	dmap_get_thr,		   /*name of the process gets an item from the table by index*/
	dmap_add_thr,		   /*name of the process adds an item and returns with its index*/
	dmap_rem_thr_byindex,  /*name of the process remove an item from the table by index*/
	dmap_rem_thr, 		   /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_thr,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_thr,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_thr,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_thr,/*name of the process unlock the table from writing*/
	dmap_itr_table_thr	   /*name of the process iterate the table*/
);

DMAP_DECL_VOIDT_TABLE(
	dmap_get_table_etc,	    /*name of the process gets the table*/
	dmap_get_etc,		    /*name of the process gets an item from the table by index*/
	dmap_add_etc,		    /*name of the process adds an item and returns with its index*/
	dmap_rem_etc_byindex,   /*name of the process remove an item from the table by index*/
	dmap_rem_etc, 		    /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_etc,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_etc,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_etc,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_etc,/*name of the process unlock the table from writing*/
	dmap_itr_table_etc	    /*name of the process iterate the table*/
);


DMAP_DECL_ITEM(
	sysdat_t,			   /*type of the item */
	dmap_get_sysdat,	   /*name of the process gets the table*/
	dmap_lock_sysdat,      /*name of the process lock the table*/
	dmap_unlock_sysdat 	   /*name of the process unlock the table*/
);


#endif //INCGUARD_MPT_DMAP_DMAP_H_
