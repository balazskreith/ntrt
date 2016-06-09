#include "dmap.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "lib_tors.h"

#include "lib_makers.h"
#include "lib_threading.h"
#include "inc_texts.h"
#include <stdlib.h>
#include "lib_debuglog.h"

//----------------------------- Static declarations ---------------------
/*
static dmap_row_t* _dmap_row_ctor();
static void        _dmap_row_dtor(dmap_row_t *row);
static dmap_row_t* _make_dmap_row(void *item, void (*dtor)(void*));
static void        _dmap_table_init(dmap_table_t *table, int32_t size, char_t *name)      __attribute__((nonnull (1)));
static void        _dmap_table_deinit(dmap_table_t *table);                               __attribute__((nonnull (1)))
static bool_t      _dmap_itr_table(dmap_table_t *table, int32_t *index, dmap_row_t **row) __attribute__((nonnull (1,2,3)));
static void*       _dmap_get_item(dmap_table_t *table, int32_t dmap_index)                __attribute__((nonnull (1)));
static void        _dmap_rem_item(dmap_table_t *table, void *item)                        __attribute__((nonnull (1)));
static void        _dmap_rem(dmap_table_t *table, int32_t dmap_index)                     __attribute__((nonnull (1)));
static int32_t     _dmap_add_row(dmap_table_t *table, dmap_row_t *row)                    __attribute__((nonnull (1)));
*/

//------------------------------------------------------------------------------------------------
//------------------------------------ Table definitions -----------------------------------------
//------------------------------------------------------------------------------------------------
DMAP_DEF_SPECT_TABLE(																	 		 \
	connection_t,		   /*name of the specific data type*/									 \
	conn_dtor,			   /*name of the destructor for the specified data*/					 \
	_dmap_table_con,	   /*name of the variable reference to the table*/						 \
	DMAP_NAME_TABLE_CON,   /*name of the table*/						 	     				 \
	MPT_MAX_CONNECTION_NUM,/*maximal number of item the table can contain. */                    \
	dmap_init_table_con,   /*name of the process initialize the table*/                          \
	dmap_deinit_table_con, /*name of the process deinitialze the table*/                         \
	dmap_get_table_con,	   /*name of the process gets the table*/								 \
	dmap_get_con,		   /*name of the process gets an item from the table by index*/			 \
	dmap_add_con,		   /*name of the process adds an item and returns with its index*/  	 \
	dmap_rem_con_byindex,  /*name of the process remove an item from the table by index*/   	 \
	dmap_rem_con, 		   /*name of the process remove an item from the table by pointer*/ 	 \
	dmap_rdlock_table_con,  /*name of the process lock the table for reading*/					 \
	dmap_rdunlock_table_con,/*name of the process unlock the table from reading*/				 \
	dmap_wrlock_table_con,  /*name of the process lock the table for writing*/					 \
	dmap_wrunlock_table_con,/*name of the process unlock the table from writing*/				 \
	dmap_itr_table_con	   /*name of the process iterate the table*/							 \
);

void dmap_rem_con_and_close(connection_t *conn)
{
	if(conn->sockd != 0){
		sysio->udpsock_close(conn->sockd);
	}
	dmap_rem_con(conn);
}


DMAP_DEF_SPECT_TABLE(																	 		 \
	tunnel_t,		 	   /*name of the specific data type*/									 \
	tunnel_dtor,		   /*name of the destructor for the specified data*/					 \
	_dmap_table_tun,	   /*name of the variable reference to the table*/						 \
	DMAP_NAME_TABLE_TUN,   /*name of the table*/						 	     				 \
	MPT_MAX_TUNNEL_NUM,    /*maximal number of item the table can contain. */                    \
	dmap_init_table_tun,   /*name of the process initialize the table*/                          \
	dmap_deinit_table_tun, /*name of the process deinitialze the table*/                         \
	dmap_get_table_tun,	   /*name of the process gets the table*/								 \
	dmap_get_tun,		   /*name of the process gets an item from the table by index*/			 \
	dmap_add_tun,		   /*name of the process adds an item and returns with its index*/  	 \
	dmap_rem_tun_byindex,  /*name of the process remove an item from the table by index*/   	 \
	dmap_rem_tun, 		   /*name of the process remove an item from the table by pointer*/ 	 \
	dmap_rdlock_table_tun,  /*name of the process lock the table for reading*/					 \
	dmap_rdunlock_table_tun,/*name of the process unlock the table from reading*/				 \
	dmap_wrlock_table_tun,  /*name of the process lock the table for writing*/					 \
	dmap_wrunlock_table_tun,/*name of the process unlock the table from writing*/				 \
	dmap_itr_table_tun	   /*name of the process iterate the table*/							 \
);

void dmap_rem_tun_and_close(tunnel_t *tun)
{
	if(tun->sockd != 0){
		sysio->udpsock_close(tun->sockd);
	}
	if(tun->fd != 0){
		sysio->tunnel_stop(tun);
	}
	dmap_rem_tun(tun);
}

DMAP_DEF_SPECT_TABLE(																	 		 \
	interface_t,		   /*name of the specific data type*/									 \
	interface_dtor,		   /*name of the destructor for the specified data*/					 \
	_dmap_table_inf,	   /*name of the variable reference to the table*/						 \
	DMAP_NAME_TABLE_INF,   /*name of the table*/						 	     				 \
	MPT_MAX_INTERFACE_NUM,   /*maximal number of item the table can contain. */                  \
	dmap_init_table_inf,   /*name of the process initialize the table*/                          \
	dmap_deinit_table_inf, /*name of the process deinitialze the table*/                         \
	dmap_get_table_inf,	   /*name of the process gets the table*/								 \
	dmap_get_inf,		   /*name of the process gets an item from the table by index*/			 \
	dmap_add_inf,		   /*name of the process adds an item and returns with its index*/  	 \
	dmap_rem_inf_byindex,  /*name of the process remove an item from the table by index*/   	 \
	dmap_rem_inf, 		   /*name of the process remove an item from the table by pointer*/ 	 \
	dmap_rdlock_table_inf,  /*name of the process lock the table for reading*/					 \
	dmap_rdunlock_table_inf,/*name of the process unlock the table from reading*/				 \
	dmap_wrlock_table_inf,  /*name of the process lock the table for writing*/					 \
	dmap_wrunlock_table_inf,/*name of the process unlock the table from writing*/				 \
	dmap_itr_table_inf	   /*name of the process iterate the table*/							 \
);


DMAP_DEF_SPECT_TABLE(																	 		 \
	network_t,		  	   /*name of the specific data type*/									 \
	network_dtor,		   /*name of the destructor for the specified data*/					 \
	_dmap_table_net,	   /*name of the variable reference to the table*/						 \
	DMAP_NAME_TABLE_NET,   /*name of the table*/						 	     				 \
	MPT_MAX_NETWORK_NUM,   /*maximal number of item the table can contain. */                    \
	dmap_init_table_net,   /*name of the process initialize the table*/                          \
	dmap_deinit_table_net, /*name of the process deinitialze the table*/                         \
	dmap_get_table_net,	   /*name of the process gets the table*/								 \
	dmap_get_net,		   /*name of the process gets an item from the table by index*/			 \
	dmap_add_net,		   /*name of the process adds an item and returns with its index*/  	 \
	dmap_rem_net_byindex,  /*name of the process remove an item from the table by index*/   	 \
	dmap_rem_net, 		   /*name of the process remove an item from the table by pointer*/ 	 \
	dmap_rdlock_table_net,  /*name of the process lock the table for reading*/					 \
	dmap_rdunlock_table_net,/*name of the process unlock the table from reading*/				 \
	dmap_wrlock_table_net,  /*name of the process lock the table for writing*/					 \
	dmap_wrunlock_table_net,/*name of the process unlock the table from writing*/				 \
	dmap_itr_table_net	   /*name of the process iterate the table*/							 \
);

DMAP_DEF_SPECT_TABLE(																	 		 \
	path_t,				   /*name of the specific data type*/									 \
	path_dtor,			   /*name of the destructor for the specified data*/					 \
	_dmap_table_pth,	   /*name of the variable reference to the table*/						 \
	DMAP_NAME_TABLE_PTH,   /*name of the table*/						 	     				 \
	MPT_MAX_PATH_NUM,      /*maximal number of item the table can contain. */                    \
	dmap_init_table_pth,   /*name of the process initialize the table*/                          \
	dmap_deinit_table_pth, /*name of the process deinitialze the table*/                         \
	dmap_get_table_pth,	   /*name of the process gets the table*/								 \
	dmap_get_pth,		   /*name of the process gets an item from the table by index*/			 \
	dmap_add_pth,		   /*name of the process adds an item and returns with its index*/  	 \
	dmap_rem_pth_byindex,  /*name of the process remove an item from the table by index*/   	 \
	dmap_rem_pth, 		   /*name of the process remove an item from the table by pointer*/ 	 \
	dmap_rdlock_table_pth,  /*name of the process lock the table for reading*/					 \
	dmap_rdunlock_table_pth,/*name of the process unlock the table from reading*/				 \
	dmap_wrlock_table_pth,  /*name of the process lock the table for writing*/					 \
	dmap_wrunlock_table_pth,/*name of the process unlock the table from writing*/				 \
	dmap_itr_table_pth	   /*name of the process iterate the table*/							 \
);

DMAP_DEF_SPECT_TABLE(																	 		 \
	thread_t,			   /*name of the specific data type*/									 \
	thread_dtor,		   /*name of the destructor for the specified data*/					 \
	_dmap_table_thr,	   /*name of the variable reference to the table*/						 \
	DMAP_NAME_TABLE_THR,   /*name of the table*/						 	     				 \
	MPT_MAX_THREAD_NUM,    /*maximal number of item the table can contain. */                    \
	dmap_init_table_thr,   /*name of the process initialize the table*/                          \
	dmap_deinit_table_thr, /*name of the process deinitialze the table*/                         \
	dmap_get_table_thr,	   /*name of the process gets the table*/								 \
	dmap_get_thr,		   /*name of the process gets an item from the table by index*/			 \
	dmap_add_thr,		   /*name of the process adds an item and returns with its index*/  	 \
	dmap_rem_thr_byindex,  /*name of the process remove an item from the table by index*/   	 \
	dmap_rem_thr, 		   /*name of the process remove an item from the table by pointer*/ 	 \
	dmap_rdlock_table_thr,  /*name of the process lock the table for reading*/					 \
	dmap_rdunlock_table_thr,/*name of the process unlock the table from reading*/				 \
	dmap_wrlock_table_thr,  /*name of the process lock the table for writing*/					 \
	dmap_wrunlock_table_thr,/*name of the process unlock the table from writing*/				 \
	dmap_itr_table_thr	   /*name of the process iterate the table*/							 \
);

DMAP_DEF_VOIDT_TABLE(																	 		 \
	_dmap_table_etc,	   /*name of the variable reference to the table*/						 \
	DMAP_NAME_TABLE_CON,   /*name of the table*/						 	     				 \
	MPT_DMAP_TABLE_ETC_NUM,/*maximal number of item the table can contain. */                    \
	dmap_init_table_etc,   /*name of the process initialize the table*/                          \
	dmap_deinit_table_etc, /*name of the process deinitialze the table*/                         \
	dmap_get_table_etc,	   /*name of the process gets the table*/								 \
	dmap_get_etc,		   /*name of the process gets an item from the table by index*/			 \
	dmap_add_etc,		   /*name of the process adds an item and returns with its index*/  	 \
	dmap_rem_etc_byindex,  /*name of the process remove an item from the table by index*/   	 \
	dmap_rem_etc, 		   /*name of the process remove an item from the table by pointer*/ 	 \
	dmap_rdlock_table_etc,  /*name of the process lock the table for reading*/					 \
	dmap_rdunlock_table_etc,/*name of the process unlock the table from reading*/				 \
	dmap_wrlock_table_etc,  /*name of the process lock the table for writing*/					 \
	dmap_wrunlock_table_etc,/*name of the process unlock the table from writing*/				 \
	dmap_itr_table_etc	   /*name of the process iterate the table*/							 \
);




DMAP_DEF_ITEM(																				 	 \
		sysdat_t,			   /*name of the specific data type*/								 \
		_dmap_sysdat,		   /*name of the variable reference to the data*/					 \
		_dmap_sysdat_mutex,	   /*name of the variable reference to the mutex*/					 \
		DMAP_NAME_SYSDAT,	   /*name of the item*/												 \
		dmap_get_sysdat,	   /*name of the process gets the table*/							 \
		dmap_lock_sysdat,      /*name of the process lock the table*/							 \
		dmap_unlock_sysdat 	   /*name of the process unlock the table*/							 \
	);




//------------------------------------------------------------------------------------------------
//------------------------------------ Dmap init -------------------------------------------------
//------------------------------------------------------------------------------------------------

void dmap_init()
{
	PRINTING_CONSTRUCTING_SG(DMAP_NAME);
	dmap_init_table_con();
	dmap_init_table_etc();
	dmap_init_table_inf();
	dmap_init_table_net();
	dmap_init_table_pth();
	dmap_init_table_thr();
	dmap_init_table_tun();
	_dmap_sysdat_mutex = mutex_ctor();
	PRINTING_SG_IS_CONSTRUCTED(DMAP_NAME);
}

void dmap_deinit()
{
	PRINTING_DESTRUCTING_SG(DMAP_NAME);
	mutex_dtor(_dmap_sysdat_mutex);
	dmap_deinit_table_con();
	dmap_deinit_table_etc();
	dmap_deinit_table_inf();
	dmap_deinit_table_net();
	dmap_deinit_table_pth();
	dmap_deinit_table_thr();
	dmap_deinit_table_tun();
	PRINTING_SG_IS_DESTRUCTED(DMAP_NAME);
}
//------------------------------------------------------------------------------------------------
//------------------------------------ Custom functions -------------------------------------------------
//------------------------------------------------------------------------------------------------

void dmap_itr_do(bool_t (*dmap_iterator)(int32_t*, void*), void* (*action)(void*))
{
	int32_t    index;
	void      *item = NULL;
	for(index = 0; dmap_iterator(&index, &item) == BOOL_TRUE; ++index){
		action(item);
	}
}

//------------------------------------------------------------------------------------------------
//------------------------------------ Functions ------------------------------------------
//------------------------------------------------------------------------------------------------

dmap_row_t* _dmap_row_ctor()
{
	dmap_row_t *result;
	result = (dmap_row_t*) malloc(sizeof(dmap_row_t));
	BZERO(result, sizeof(dmap_row_t));
	return result;
}

void _dmap_row_dtor(dmap_row_t *row)
{
	if(row->item == NULL){
		free(row);
		return;
	}
	if(row->dtor == NULL){
		runtime_warning("_dmap_row_dtor: Possible memory leak at %p.",row);
		free(row);
		return;
	}
	row->dtor(row->item);
	free(row);
}

dmap_row_t* _make_dmap_row(void *item, void (*dtor)(void*))
{
	dmap_row_t *result;
	result = _dmap_row_ctor();
	result->item = item;
	result->dtor = dtor;
	return result;
}

void _dmap_table_init(dmap_table_t *table, int32_t size, char_t *name)
{
	int32_t index;
	logging("%s: %s", GET_TEXT_INIT_SG("dmap_table"), name);
	BZERO(table, sizeof(dmap_table_t));
	table->rwmutex = rwmutex_ctor();
	table->size = size;
	table->rows = (dmap_row_t**) malloc(sizeof(dmap_row_t*) * table->size);
	for(index = 0; index < table->size; index++)
	{
		table->rows[index] = NULL;
	}
	strcpy(table->name, name);
	PRINTING_INIT_COMPLETED;
}

void _dmap_table_deinit(dmap_table_t *table)
{
	int32_t index;
	dmap_row_t *row;
	logging("%s: %s", GET_TEXT_DEINIT_SG("dmap_table"), table->name);
	for(index = 0; _dmap_itr_table(table, &index, &row) == BOOL_TRUE; ++index)
	{
		_dmap_rem(table, index);
	}
	if(table->rwmutex != NULL){
		rwmutex_dtor(table->rwmutex);
		table->rwmutex = NULL;
	}
	PRINTING_DEINIT_COMPLETED;
}

bool_t _dmap_itr_table(dmap_table_t *table, int32_t *dmap_index, dmap_row_t **row)
{
	*row = NULL;
	for(;*dmap_index < table->size && table->rows[*dmap_index] == NULL; ++(*dmap_index));
	if(table->size <= *dmap_index){
		return BOOL_FALSE;
	}
	*row = table->rows[*dmap_index];
	return BOOL_TRUE;
}

void* _dmap_get_item(dmap_table_t *table, int32_t dmap_index)
{
	void       *result = NULL;
	dmap_row_t *row;
	if(dmap_index < 0 || table->size <= dmap_index){
		runtime_warning("_dmap_get_item: the requested index (%d) is out of the borders in %s", dmap_index, table->name);
		return NULL;
	}
	row = table->rows[dmap_index];
	if(row == NULL){
		runtime_warning("_dmap_get_item: row is not found at %d in %s ", dmap_index, table->name);
		return NULL;
	}
	result = row->item;
	return result;
}

void _dmap_rem_item(dmap_table_t *table, void *item)
{
	int32_t     index;
	dmap_row_t *row = NULL;
	for(index = 0;
		_dmap_itr_table(table, &index, &row) == BOOL_TRUE
		&& item == row->item;
		++index);
	if(index == table->size){
		runtime_warning("_dmap_rem: row is not found to %p in %s ", item, table->name);
		return;
	}
	_dmap_row_dtor(row);
}

void _dmap_rem(dmap_table_t *table, int32_t dmap_index)
{
	dmap_row_t *row;
	if(dmap_index < 0 || table->size <= dmap_index){
		runtime_warning("_dmap_rem: the requested index (%d) is out of the borders in %s", dmap_index, table->name);
		return;
	}
	row = table->rows[dmap_index];
	if(row == NULL){
		runtime_warning("_dmap_rem: row is not exists at %d in %s ", dmap_index, table->name);
		return;
	}
	_dmap_row_dtor(row);
	table->rows[dmap_index] = NULL;
}

int32_t _dmap_add_row(dmap_table_t *table, dmap_row_t *row)
{
	int32_t result;
	if(row == NULL){
		runtime_warning("_dmap_add_row: tried to add row with NULL into %s", table->name);
		return DEVCLEGO_DMAP_ERROR_ADD;
	}
	for(result = 0; table->rows[result] != NULL && result < table->size; result++);
	if(result == table->size){
		runtime_warning("_dmap_add_row: %s is full", table->name);
		return DEVCLEGO_ERROR_TABLE_IS_FULL;
	}
	table->rows[result] = row;
	return result;
}

