#ifndef INCGUARD_NTRT_DMAP_DMAP_H_
#define INCGUARD_NTRT_DMAP_DMAP_H_
#include "lib_defs.h"
#include "lib_descs.h"
#include "lib_threading.h"
#include "dmap_defs.h"

#define NTRT_DMAP_TABLE_ETC_NUM 32
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
DMAP_DECL_SPECT_TABLE(
	feature_t,		   /*type of the items stored in the table*/
	dmap_get_table_features,	   /*name of the process gets the table*/
	dmap_get_feature,		   /*name of the process gets an item from the table by index*/
	dmap_add_feature,		   /*name of the process adds an item and returns with its index*/
	dmap_rem_feature_byindex,  /*name of the process remove an item from the table by index*/
	dmap_rem_feature, 		   /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_features,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_features,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_features,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_features,/*name of the process unlock the table from writing*/
	dmap_itr_table_features	   /*name of the process iterate the table*/
);

DMAP_DECL_SPECT_TABLE(
        pcap_listener_t,		  	   /*type of the items stored in the table*/
	dmap_get_table_pcapls,	   /*name of the process gets the table*/
	dmap_get_pcapls,		   /*name of the process gets an item from the table by index*/
	dmap_add_pcapls,		   /*name of the process adds an item and returns with its index*/
	dmap_rem_pcapls_byindex,  /*name of the process remove an item from the table by index*/
	dmap_rem_pcapls, 		   /*name of the process remove an item from the table by pointer*/
	dmap_rdlock_table_pcapls,  /*name of the process lock the table for reading*/
	dmap_rdunlock_table_pcapls,/*name of the process unlock the table from reading*/
	dmap_wrlock_table_pcapls,  /*name of the process lock the table for writing*/
	dmap_wrunlock_table_pcapls,/*name of the process unlock the table from writing*/
	dmap_itr_table_pcapls	   /*name of the process iterate the table*/
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


DMAP_DECL_ITEM(
	sysdat_t,			   /*type of the item */
	dmap_get_sysdat,	   /*name of the process gets the table*/
	dmap_lock_sysdat,      /*name of the process lock the table*/
	dmap_unlock_sysdat 	   /*name of the process unlock the table*/
);


#endif //INCGUARD_NTRT_DMAP_DMAP_H_
