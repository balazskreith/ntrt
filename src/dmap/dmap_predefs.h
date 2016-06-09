/*
 * dmap_predefs.h
 *
 *  Created on: Mar 8, 2014
 *      Author: balazs
 */

#ifndef INCGUARD_DMAP_PREDEFS_H_
#define INCGUARD_DMAP_PREDEFS_H_
#include "dmap_defs.h"
#include "lib_defs.h"
#include "lib_threading.h"


#define DMAP_DECL_GET_DATA_PROC(																	\
								 PROC_NAME,															\
								 RETURN_TYPE														\
								 ) 																	\
	RETURN_TYPE* PROC_NAME();				  														\

#define DMAP_DEF_GET_DATA_PROC(						  												\
								PROC_NAME, 			  												\
								RETURN_TYPE,		  												\
								DATA_PTR			  												\
								) 					  												\
	RETURN_TYPE* PROC_NAME()				 		  												\
	{												 												\
		return DATA_PTR;							  												\
	}																								\


#define DMAP_DECL_M_LOCK_PROC(																		\
								  PROC_NAME															\
								 )		  	 	   													\
	void PROC_NAME();   																			\


#define DMAP_DEF_M_LOCK_PROC(			 	   														\
									  PROC_NAME, 													\
									  MUTEX_PTR,													\
									  DATA_NAME														\
								)			 														\
									  	  	  	  													\
	void PROC_NAME() 																				\
	{																								\
		debug_lock(DATA_NAME);																	\
		mutex_lock(MUTEX_PTR);																\
	}																								\



#define DMAP_DECL_M_UNLOCK_PROC(PROC_NAME)  														\
	void PROC_NAME();   																			\


#define DMAP_DEF_M_UNLOCK_PROC(			 	    													\
									  PROC_NAME, 													\
									  MUTEX_PTR,													\
									  DATA_NAME														\
								)			 														\
									  	  	  	  													\
	void PROC_NAME() 																				\
	{																								\
		debug_unlock(DATA_NAME);																\
		mutex_unlock(MUTEX_PTR);															\
	}																								\



#define DMAP_DECL_R_LOCK_PROC(																		\
								  PROC_NAME															\
								 )		  	 	   													\
	void PROC_NAME();   																			\


#define DMAP_DEF_R_LOCK_PROC(			 	   														\
									  PROC_NAME, 													\
									  RWMUTEX_PTR,													\
									  DATA_NAME														\
								)			 														\
									  	  	  	  													\
	void PROC_NAME() 																				\
	{																								\
		debug_rdlock(DATA_NAME);																	\
		rwmutex_read_lock(RWMUTEX_PTR);																\
	}																								\



#define DMAP_DECL_R_UNLOCK_PROC(PROC_NAME)  														\
	void PROC_NAME();   																			\


#define DMAP_DEF_R_UNLOCK_PROC(			 	    													\
									  PROC_NAME, 													\
									  RWMUTEX_PTR,													\
									  DATA_NAME														\
								)			 														\
									  	  	  	  													\
	void PROC_NAME() 																				\
	{																								\
		debug_rdunlock(DATA_NAME);																\
		rwmutex_read_unlock(RWMUTEX_PTR);															\
	}																								\

#define DMAP_DECL_W_LOCK_PROC(																		\
								  PROC_NAME															\
								 )		  	 	   													\
	void PROC_NAME();   																			\


#define DMAP_DEF_W_LOCK_PROC(			 	   														\
									  PROC_NAME, 													\
									  RWMUTEX_PTR,													\
									  DATA_NAME														\
								)			 														\
									  	  	  	  													\
	void PROC_NAME() 																				\
	{																								\
		debug_wrlock(DATA_NAME);																	\
		rwmutex_write_lock(RWMUTEX_PTR);															\
	}																								\



#define DMAP_DECL_W_UNLOCK_PROC(PROC_NAME)  														\
	void PROC_NAME();   																			\


#define DMAP_DEF_W_UNLOCK_PROC(			 	    													\
									  PROC_NAME, 													\
									  RWMUTEX_PTR,													\
									  DATA_NAME														\
								)			 														\
									  	  	  	  													\
	void PROC_NAME() 																				\
	{																								\
		debug_wrunlock(DATA_NAME);																\
		rwmutex_write_unlock(RWMUTEX_PTR);															\
	}																								\




//---------------------------------------------------------------------------------------------------
//----------------------------- TABLE MACROS --------------------------------------------------------
//---------------------------------------------------------------------------------------------------

#define DMAP_DECL_TABLE_ADD_VOIDT_PROC(																\
										PROC_NAME													\
									  )  			    											\
	int32_t PROC_NAME(void *item, void (*item_dtor)(void*item))     								\
				__attribute__((nonnull (1,2)));   													\



#define DMAP_DEF_TABLE_ADD_VOIDT_PROC(																\
										PROC_NAME, 													\
										TABLE_PTR													\
									  )					  		    								\
	int32_t PROC_NAME(void *item, void (*dtor)(void*))   											\
	{																							    \
		int32_t result;																				\
		dmap_row_t *row;																			\
		row = _make_dmap_row(item, dtor);															\
		result = _dmap_add_row(TABLE_PTR, row);														\
		return result;																				\
	}												  												\


#define DMAP_DEF_TABLE_INIT_PROC(PROC_NAME, TABLE_PTR, TABLE_LENGTH, TABLE_NAME)					\
	static void PROC_NAME()     																	\
	{																								\
		_dmap_table_init(TABLE_PTR, TABLE_LENGTH, TABLE_NAME);										\
	}																								\



#define DMAP_DEF_TABLE_DEINIT_PROC(PROC_NAME, TABLE_PTR)											\
	static void PROC_NAME()     																	\
	{																								\
		_dmap_table_deinit(TABLE_PTR);																\
	}																								\



#define DMAP_DECL_TABLE_ADD_SPECT_PROC( 															\
									  PROC_NAME, 													\
									  TABLE_PTR, 													\
									  DATA_TYPE														\
									  ) 			  												\
	int32_t PROC_NAME(DATA_TYPE* item)       														\
				__attribute__((nonnull (1)));   													\


#define DMAP_DEF_TABLE_ADD_SPECT_PROC( 																\
									  PROC_NAME, 													\
									  TABLE_PTR, 													\
									  DATA_TYPE, 													\
									  DATA_DTOR														\
									 )			 	  												\
									  	  	  	  	 											    \
	int32_t PROC_NAME(DATA_TYPE* item)																\
	{																								\
		int32_t result;																				\
		dmap_row_t *row;																			\
		row = _make_dmap_row(item, DATA_DTOR);														\
		result = _dmap_add_row(TABLE_PTR, row);														\
		return result;																				\
	}												  												\


#define DMAP_DECL_TABLE_REM_INDEX_PROC(PROC_NAME)  												    \
	void PROC_NAME(int32_t dmap_index);   														    \



#define DMAP_DEF_TABLE_REM_INDEX_PROC( 	   															\
									  PROC_NAME, 													\
									  TABLE_PTR														\
									 ) 				  												\
									  	  	  	  	  											    \
	void PROC_NAME(int32_t dmap_index)																\
	{																								\
		_dmap_rem(TABLE_PTR, dmap_index);															\
	}												  												\


#define DMAP_DECL_TABLE_REM_ITEM_PROC(PROC_NAME, ITEM_TYPE)  										\
	void PROC_NAME(ITEM_TYPE* item);   																\


#define DMAP_DEF_TABLE_REM_ITEM_PROC(			 													\
									  PROC_NAME, 													\
									  ITEM_TYPE,													\
									  TABLE_PTR														\
									) 																\
									  	  	  	  													\
	void PROC_NAME(ITEM_TYPE* item) 																\
	{																								\
		_dmap_rem_item(TABLE_PTR, (void *) item);													\
	}																								\




#define DMAP_DECL_ITR_TABLE_PROC(				 													\
								 PROC_NAME,															\
								 RESULT_TYPE														\
								)			  														\
	bool_t PROC_NAME(int32_t *dmap_index, RESULT_TYPE **result)										\
								__attribute__((nonnull (1,2)));   									\


#define DMAP_DEF_ITR_TABLE_PROC(			 	    												\
									  PROC_NAME, 													\
									  RESULT_TYPE,													\
									  TABLE_PTR														\
								   )			 													\
									  	  	  	  													\
	bool_t PROC_NAME(																				\
				   int32_t *dmap_index, 															\
				   RESULT_TYPE **result																\
				  )																					\
	{																								\
		bool_t exist;																				\
		dmap_row_t *row = NULL;																		\
		*result = NULL;																				\
		exist = _dmap_itr_table(TABLE_PTR, dmap_index, &row);										\
		if(exist == BOOL_FALSE){																	\
			return BOOL_FALSE;																		\
		}																							\
		*result = (RESULT_TYPE*) row->item;															\
		return exist;																				\
	}																								\


#define DMAP_DECL_GET_TABLE_ITEM_PROC(				 												\
								 PROC_NAME,															\
								 RESULT_TYPE														\
								)			  														\
	RESULT_TYPE* PROC_NAME(int32_t dmap_index);   													\


#define DMAP_DEF_GET_TABLE_ITEM_BYINDX_PROC(			  											\
									  PROC_NAME, 													\
									  RESULT_TYPE,													\
									  TABLE_PTR														\
								   )			 													\
									  	  	  	  													\
	RESULT_TYPE* PROC_NAME(																			\
				   int32_t dmap_index	 															\
				  )																					\
	{																								\
		return (RESULT_TYPE*) _dmap_get_item(TABLE_PTR, dmap_index);								\
	}																								\

#define DMAP_DECL_VOIDT_TABLE(																		\
						GET_TABLE_PROC_NAME,														\
						GET_ITEM_PROC_NAME,															\
						ADD_ITEM_PROC_NAME,															\
						REM_INDEX_PROC_NAME,														\
						REM_ITEM_PROC_NAME,															\
						R_LOCK_TABLE_PROC_NAME,														\
						R_UNLOCK_TABLE_PROC_NAME,													\
						W_LOCK_TABLE_PROC_NAME,														\
						W_UNLOCK_TABLE_PROC_NAME,													\
						ITR_TABLE_PROC_NAME															\
						)																			\
		DMAP_DECL_GET_DATA_PROC(GET_TABLE_PROC_NAME, dmap_table_t);									\
		DMAP_DECL_GET_TABLE_ITEM_PROC(GET_ITEM_PROC_NAME, void);									\
		DMAP_DECL_TABLE_ADD_VOIDT_PROC(ADD_ITEM_PROC_NAME);											\
		DMAP_DECL_TABLE_REM_INDEX_PROC(REM_INDEX_PROC_NAME);										\
		DMAP_DECL_TABLE_REM_ITEM_PROC(REM_ITEM_PROC_NAME, void);									\
		DMAP_DECL_R_LOCK_PROC(R_LOCK_TABLE_PROC_NAME);												\
		DMAP_DECL_R_UNLOCK_PROC(R_UNLOCK_TABLE_PROC_NAME);											\
		DMAP_DECL_W_LOCK_PROC(W_LOCK_TABLE_PROC_NAME);												\
		DMAP_DECL_W_UNLOCK_PROC(W_UNLOCK_TABLE_PROC_NAME);											\
		DMAP_DECL_ITR_TABLE_PROC(ITR_TABLE_PROC_NAME, void);										\


#define DMAP_DEF_VOIDT_TABLE(																		\
						TABLE_VARIABLE,																\
						TABLE_NAME,																	\
						TABLE_LENGTH,																\
						INIT_TABLE_PROC_NAME,														\
						DEINIT_TABLE_PROC_NAME,														\
						GET_TABLE_PROC_NAME,														\
						GET_ITEM_PROC_NAME,															\
						ADD_ITEM_PROC_NAME,															\
						REM_INDEX_PROC_NAME,														\
						REM_ITEM_PROC_NAME,															\
						R_LOCK_TABLE_PROC_NAME,														\
						R_UNLOCK_TABLE_PROC_NAME,													\
						W_LOCK_TABLE_PROC_NAME,														\
						W_UNLOCK_TABLE_PROC_NAME,													\
						ITR_TABLE_PROC_NAME															\
						)																			\
		static dmap_table_t TABLE_VARIABLE;															\
		DMAP_DEF_TABLE_INIT_PROC(INIT_TABLE_PROC_NAME, &TABLE_VARIABLE, TABLE_LENGTH, TABLE_NAME);  \
		DMAP_DEF_TABLE_DEINIT_PROC(DEINIT_TABLE_PROC_NAME, &TABLE_VARIABLE);						\
		DMAP_DEF_GET_DATA_PROC(GET_TABLE_PROC_NAME, dmap_table_t, &TABLE_VARIABLE);					\
		DMAP_DEF_GET_TABLE_ITEM_BYINDX_PROC(GET_ITEM_PROC_NAME, void, &TABLE_VARIABLE);					\
		DMAP_DEF_TABLE_ADD_VOIDT_PROC(ADD_ITEM_PROC_NAME, &TABLE_VARIABLE);							\
		DMAP_DEF_TABLE_REM_INDEX_PROC(REM_INDEX_PROC_NAME, &TABLE_VARIABLE);						\
		DMAP_DEF_TABLE_REM_ITEM_PROC(REM_ITEM_PROC_NAME, void, &TABLE_VARIABLE); 					\
		DMAP_DEF_R_LOCK_PROC(R_LOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);			\
		DMAP_DEF_R_UNLOCK_PROC(R_UNLOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);		\
		DMAP_DEF_W_LOCK_PROC(W_LOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);			\
		DMAP_DEF_W_UNLOCK_PROC(W_UNLOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);		\
		DMAP_DEF_ITR_TABLE_PROC(ITR_TABLE_PROC_NAME, void, &TABLE_VARIABLE);						\



#define DMAP_DECL_SPECT_TABLE(																		\
						DATA_TYPE,																	\
						GET_TABLE_PROC_NAME,														\
						GET_ITEM_PROC_NAME,															\
						ADD_ITEM_PROC_NAME,															\
						REM_INDEX_PROC_NAME,														\
						REM_ITEM_PROC_NAME,															\
						R_LOCK_TABLE_PROC_NAME,														\
						R_UNLOCK_TABLE_PROC_NAME,													\
						W_LOCK_TABLE_PROC_NAME,														\
						W_UNLOCK_TABLE_PROC_NAME,													\
						ITR_TABLE_PROC_NAME															\
						)																			\
		DMAP_DECL_GET_DATA_PROC(GET_TABLE_PROC_NAME, dmap_table_t);									\
		DMAP_DECL_GET_TABLE_ITEM_PROC(GET_ITEM_PROC_NAME, DATA_TYPE);								\
		DMAP_DECL_TABLE_ADD_SPECT_PROC(ADD_ITEM_PROC_NAME, TABLE_PTR, DATA_TYPE);					\
		DMAP_DECL_TABLE_REM_INDEX_PROC(REM_INDEX_PROC_NAME);										\
		DMAP_DECL_TABLE_REM_ITEM_PROC(REM_ITEM_PROC_NAME, DATA_TYPE);								\
		DMAP_DECL_R_LOCK_PROC(R_LOCK_TABLE_PROC_NAME);												\
		DMAP_DECL_R_UNLOCK_PROC(R_UNLOCK_TABLE_PROC_NAME);											\
		DMAP_DECL_W_LOCK_PROC(W_LOCK_TABLE_PROC_NAME);												\
		DMAP_DECL_W_UNLOCK_PROC(W_UNLOCK_TABLE_PROC_NAME);											\
		DMAP_DECL_ITR_TABLE_PROC(ITR_TABLE_PROC_NAME, DATA_TYPE);									\


#define DMAP_DEF_SPECT_TABLE(																		\
						DATA_TYPE,																	\
						DATA_DTOR,																	\
						TABLE_VARIABLE,																\
						TABLE_NAME,																	\
						TABLE_LENGTH,																\
						INIT_TABLE_PROC_NAME,														\
						DEINIT_TABLE_PROC_NAME,														\
						GET_TABLE_PROC_NAME,														\
						GET_ITEM_PROC_NAME,															\
						ADD_ITEM_PROC_NAME,															\
						REM_INDEX_PROC_NAME,														\
						REM_ITEM_PROC_NAME,															\
						R_LOCK_TABLE_PROC_NAME,														\
						R_UNLOCK_TABLE_PROC_NAME,													\
						W_LOCK_TABLE_PROC_NAME,														\
						W_UNLOCK_TABLE_PROC_NAME,													\
						ITR_TABLE_PROC_NAME															\
						)																			\
		static dmap_table_t TABLE_VARIABLE;															\
		DMAP_DEF_TABLE_INIT_PROC(INIT_TABLE_PROC_NAME, &TABLE_VARIABLE, TABLE_LENGTH, TABLE_NAME);  \
		DMAP_DEF_TABLE_DEINIT_PROC(DEINIT_TABLE_PROC_NAME, &TABLE_VARIABLE);						\
		DMAP_DEF_GET_DATA_PROC(GET_TABLE_PROC_NAME, dmap_table_t, &TABLE_VARIABLE);					\
		DMAP_DEF_GET_TABLE_ITEM_BYINDX_PROC(GET_ITEM_PROC_NAME, DATA_TYPE, &TABLE_VARIABLE); 				\
		DMAP_DEF_TABLE_ADD_SPECT_PROC(ADD_ITEM_PROC_NAME, &TABLE_VARIABLE, DATA_TYPE, DATA_DTOR);	\
		DMAP_DEF_TABLE_REM_INDEX_PROC(REM_INDEX_PROC_NAME, &TABLE_VARIABLE);						\
		DMAP_DEF_TABLE_REM_ITEM_PROC(REM_ITEM_PROC_NAME, DATA_TYPE, &TABLE_VARIABLE); 				\
		DMAP_DEF_R_LOCK_PROC(R_LOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);			\
		DMAP_DEF_R_UNLOCK_PROC(R_UNLOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);		\
		DMAP_DEF_W_LOCK_PROC(W_LOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);			\
		DMAP_DEF_W_UNLOCK_PROC(W_UNLOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);		\
		DMAP_DEF_ITR_TABLE_PROC(ITR_TABLE_PROC_NAME, DATA_TYPE, &TABLE_VARIABLE);					\

#define DMAP_DEF_SPECT_LOCAL_TABLE(																	\
									DATA_TYPE,														\
									DATA_DTOR,														\
									TABLE_VARIABLE,													\
									TABLE_NAME,														\
									TABLE_LENGTH,													\
									INIT_TABLE_PROC_NAME,											\
									DEINIT_TABLE_PROC_NAME,											\
									GET_TABLE_PROC_NAME,											\
									GET_ITEM_PROC_NAME,												\
									ADD_ITEM_PROC_NAME,												\
									REM_INDEX_PROC_NAME,											\
									REM_ITEM_PROC_NAME,												\
									R_LOCK_TABLE_PROC_NAME,											\
									R_UNLOCK_TABLE_PROC_NAME,										\
									W_LOCK_TABLE_PROC_NAME,											\
									W_UNLOCK_TABLE_PROC_NAME,										\
									ITR_TABLE_PROC_NAME												\
								   )																\
		static dmap_table_t TABLE_VARIABLE;															\
		static DMAP_DECL_GET_DATA_PROC(GET_TABLE_PROC_NAME, dmap_table_t);							\
		static DMAP_DECL_GET_TABLE_ITEM_PROC(GET_ITEM_PROC_NAME, DATA_TYPE);						\
		static DMAP_DECL_TABLE_ADD_SPECT_PROC(ADD_ITEM_PROC_NAME, TABLE_PTR, DATA_TYPE);			\
		static DMAP_DECL_TABLE_REM_INDEX_PROC(REM_INDEX_PROC_NAME);									\
		static DMAP_DECL_TABLE_REM_ITEM_PROC(REM_ITEM_PROC_NAME, DATA_TYPE);						\
		static DMAP_DECL_R_LOCK_PROC(R_LOCK_TABLE_PROC_NAME);										\
		static DMAP_DECL_R_UNLOCK_PROC(R_UNLOCK_TABLE_PROC_NAME);									\
		static DMAP_DECL_W_LOCK_PROC(W_LOCK_TABLE_PROC_NAME);										\
		static DMAP_DECL_W_UNLOCK_PROC(W_UNLOCK_TABLE_PROC_NAME);									\
		static DMAP_DECL_ITR_TABLE_PROC(ITR_TABLE_PROC_NAME, DATA_TYPE);							\
																									\
		DMAP_DEF_TABLE_INIT_PROC(INIT_TABLE_PROC_NAME, &TABLE_VARIABLE, TABLE_LENGTH, TABLE_NAME);  \
		DMAP_DEF_TABLE_DEINIT_PROC(DEINIT_TABLE_PROC_NAME, &TABLE_VARIABLE);						\
		DMAP_DEF_GET_DATA_PROC(GET_TABLE_PROC_NAME, dmap_table_t, &TABLE_VARIABLE);					\
		DMAP_DEF_GET_TABLE_ITEM_BYINDX_PROC(GET_ITEM_PROC_NAME, DATA_TYPE, &TABLE_VARIABLE);				\
		DMAP_DEF_TABLE_ADD_SPECT_PROC(ADD_ITEM_PROC_NAME, &TABLE_VARIABLE, DATA_TYPE, DATA_DTOR);	\
		DMAP_DEF_TABLE_REM_INDEX_PROC(REM_INDEX_PROC_NAME, &TABLE_VARIABLE);						\
		DMAP_DEF_TABLE_REM_ITEM_PROC(REM_ITEM_PROC_NAME, DATA_TYPE, &TABLE_VARIABLE); 				\
		DMAP_DEF_R_LOCK_PROC(R_LOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);			\
		DMAP_DEF_R_UNLOCK_PROC(R_UNLOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);		\
		DMAP_DEF_W_LOCK_PROC(W_LOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);			\
		DMAP_DEF_W_UNLOCK_PROC(W_UNLOCK_TABLE_PROC_NAME, TABLE_VARIABLE.rwmutex, TABLE_NAME);		\
		DMAP_DEF_ITR_TABLE_PROC(ITR_TABLE_PROC_NAME, DATA_TYPE, &TABLE_VARIABLE);					\

//---------------------------------------------------------------------------------------------------
//------------------------- DATA ITEM MACROS --------------------------------------------------------
//---------------------------------------------------------------------------------------------------

#define DMAP_DECL_ITEM(																				\
						ITEM_TYPE,																	\
						GET_ITEM_PROC_NAME,															\
						LOCK_ITEM_PROC_NAME,														\
						UNLOCK_ITEM_PROC_NAME														\
						)																			\
		DMAP_DECL_GET_DATA_PROC(GET_ITEM_PROC_NAME, ITEM_TYPE)	 									\
		DMAP_DECL_R_LOCK_PROC(LOCK_ITEM_PROC_NAME)													\
		DMAP_DECL_R_UNLOCK_PROC(UNLOCK_ITEM_PROC_NAME)												\


#define DMAP_DEF_ITEM(																				\
						ITEM_TYPE,																	\
						ITEM_VARIABLE,																\
						MUTEX_VARIABLE,																\
						ITEM_NAME,																	\
						GET_ITEM_PROC_NAME,															\
						LOCK_ITEM_PROC_NAME,														\
						UNLOCK_ITEM_PROC_NAME														\
						)																			\
		static ITEM_TYPE ITEM_VARIABLE;																\
		static mutex_t *MUTEX_VARIABLE;																\
		DMAP_DEF_GET_DATA_PROC(GET_ITEM_PROC_NAME, ITEM_TYPE, &ITEM_VARIABLE) 						\
		DMAP_DEF_M_LOCK_PROC(LOCK_ITEM_PROC_NAME, MUTEX_VARIABLE, ITEM_NAME)						\
		DMAP_DEF_M_UNLOCK_PROC(UNLOCK_ITEM_PROC_NAME, MUTEX_VARIABLE, ITEM_NAME)					\


#endif /* INCGUARD_DMAP_PREDEFS_H_ */
