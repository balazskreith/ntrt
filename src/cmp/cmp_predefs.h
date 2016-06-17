#ifndef INCGUARD_CMP_PREDEFS_H_
#define INCGUARD_CMP_PREDEFS_H_
#include "lib_predefs.h"
#include "lib_debuglog.h"
#include "lib_descs.h"
#include "lib_funcs.h"
#include "inc_texts.h"
#include "lib_puffers.h"

/*
void cmp_tors_start();
void cmp_tors_stop();
packet_t* cmp_packet_ctor();
void cmp_packet_dtor(packet_t*);
command_t* cmp_command_ctor();
void cmp_command_dtor(command_t*);
handshake_t* cmp_handshake_ctor();
void cmp_handshake_dtor(handshake_t*);
*/

#define __CMP_NO_INIT_FUNC_ func_do_nothing
#define __CMP_NO_INIT_MFUNC__ func_with_param_do_nothing
#define __CMP_NO_DEINIT_FUNC_ func_do_nothing
#define __CMP_NO_DEINIT_MFUNC__ func_with_param_do_nothing
#define __NO_TEST_FUNC_ func_with_param_return_true

#ifdef NDEBUG

#define CMP_TEST(CMP_TEST_FUNC, CMP_VAR, CMP_NAME)

#else
#define CMP_TEST(CMP_TEST_FUNC, CMP_VAR, CMP_NAME)     							   \
	CMP_CHECK_EXISTANCE(CMP_VAR,  CMP_NAME);									   \
	ASSERT_TRUE(CMP_TEST_FUNC(CMP_VAR) == BOOL_TRUE, "Test failed at component: %s", CMP_NAME)

#endif /* NDEBUG */




#define CMP_DEMAND(name, lvalue, supplier, ...)                		 \
	debug_flow("%s demand an item from %p", name, supplier);         \
	lvalue = supplier(__VA_ARGS__)						         	 \

#define CMP_RECEIVE(name, item)				                     	 \
	debug_flow("%s received an item %p", name, item);        		 \

#define CMP_SUPPLY(name, item)				                     	 \
	debug_flow("%s supplied an item %p", name, item);        		 \

#define CMP_SEND(name, receiver, item)								 \
	debug_flow("%s send an item %p to %p ", name, item, receiver);   \
	receiver(item);									 				 \

#define CMP_SEND_WITH_ARGS(name, receiver, item, ...)				 \
	debug_flow("%s send an item %p to %p ", name, item, receiver);   \
	receiver(__VA_ARGS__, item);					 				 \


#define CMP_DEF_THIS(TYPE, VALUE) TYPE* this = VALUE;

#define CMP_DEF_VAR(TYPE, NAME) TYPE* NAME = NULL;
#define CMP_STAT_DEF_VAR(TYPE, NAME) static TYPE* NAME = NULL;

#define CMP_DEF_PUFFER_RECV_WAIT(										 \
								cmp_name, 								 \
								cmp_type,								 \
								cmp_var, 								 \
								item_type, 								 \
								PROC_NAME							  	 \
								)										 \
	static void PROC_NAME(item_type* item)								 \
	{																	 \
		CMP_DEF_THIS(cmp_type, cmp_var);								 \
		datapuffer_t    *puffer = this->puffer;							 \
		signal_t        *signal = this->signal;							 \
		bool_t  		 was_empty;										 \
																		 \
		/*CMP_PUFFER_RECV_PROC_WAIT(cmp_name, item_type, puffer, signal,*/ \
				/*item, puffer->is_empty, was_empty, puffer->is_full);	 */\
		CMP_PUFFER_RECV_PROC_WAIT(cmp_name, item_type, puffer, signal,   \
				item, datapuffer_isempty(puffer), was_empty, datapuffer_isfull(puffer));\
																		 \
	}/*#PROC_NAME end*/										 		     \


#define CMP_DEF_PUFFER_SUPPLY_WAIT(										 \
								cmp_name, 								 \
								cmp_type,								 \
								cmp_var,								 \
								item_type, 								 \
								PROC_NAME							  	 \
								)										 \
	static item_type* PROC_NAME()									 	 \
	{																	 \
		CMP_DEF_THIS(cmp_type, cmp_var);								 \
		item_type       *result = NULL;									 \
		datapuffer_t    *puffer = this->puffer;							 \
		signal_t        *signal = this->signal;							 \
		bool_t  		 was_full;										 \
																		 \
		/*CMP_PUFFER_SUPPLY_PROC_WAIT(cmp_name, item_type, puffer, signal,*/ \
				/*result, puffer->is_empty, puffer->is_full, was_full);*/	 \
		CMP_PUFFER_SUPPLY_PROC_WAIT(cmp_name, item_type, puffer, signal, \
				result, datapuffer_isempty(puffer), datapuffer_isfull(puffer), was_full);	 \
		return result;													 \
	}/*#PROC_NAME end*/										 		     \



#define CMP_PUFFER_SUPPLY_PROC_WAIT(									 	 \
									name,									 \
									type, 								 	 \
									puffer, 								 \
									signal, 								 \
									item,									 \
									is_empty, 								 \
									is_full, 								 \
									was_full								 \
									)										 \
	signal_lock(signal);											    	 \
	if(is_empty == BOOL_TRUE){											     \
		/*logging("%s is empty", name);*/			 		 	 			 \
		signal_wait(signal);										     	 \
	}																	     \
																		     \
    was_full = is_full;														 \
	item = (type*) datapuffer_read(puffer);						  			 \
																		     \
	if(was_full == BOOL_TRUE){											     \
		signal_set(signal);										    		 \
	}																	     \
	/*CMP_SUPPLY(name, item);*/												 \
	signal_unlock(signal);     												 \


#define CMP_PUFFER_RECV_PROC_WAIT(									         \
								   name,									 \
								   type, 								     \
								   puffer, 								     \
								   signal, 								     \
								   item,									 \
								   is_empty, 						     	 \
								   was_empty, 							     \
								   is_full									 \
								)			 							     \
	signal_lock(signal);													 \
	/*CMP_RECEIVE(name, item);*/									 		 \
	if(is_full == BOOL_TRUE){												 \
		runtime_warning("%s is full", name);				 		 	 	 \
		signal_wait(signal);												 \
	}																		 \
		 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	     \
    was_empty = is_empty;													 \
	datapuffer_write(puffer, (void*) item);								 	 \
																		     \
	if(was_empty == BOOL_TRUE){											 	 \
		signal_set(signal);													 \
	}																		 \
																			 \
	signal_unlock(signal);										     		 \


#define CMP_CTOR_PROC(type, var, name)                      			\
		PRINTING_CONSTRUCTING_SG(name);									\
		if(var != NULL){												\
			runtime_warning("%s is already constructed", name);			\
			return;														\
		}																\
		var = (type*) malloc(sizeof(type));								\
		BZERO(var, sizeof(type));										\
		PRINTING_SG_IS_CONSTRUCTED(name);


#define CMP_MCTOR_PROC(TYPE, VALUE, CMP_NAME)                      		\
		PRINTING_CONSTRUCTING_SG(CMP_NAME);								\
		VALUE = (TYPE*) malloc(sizeof(TYPE));							\
		BZERO(VALUE, sizeof(TYPE));										\
		PRINTING_SG_IS_CONSTRUCTED(CMP_NAME);							\


#define CMP_DEF_CTOR(PROC_NAME,											\
					  TYPE,												\
					  VALUE, 											\
					  CMP_NAME, 										\
					  INIT_PROC_NAME, 									\
					  TESTING_PROC_NAME)            					\
	void PROC_NAME()													\
	{																	\
			CMP_CTOR_PROC(TYPE, VALUE, CMP_NAME)						\
			PRINTING_INIT_SG(CMP_NAME);									\
			INIT_PROC_NAME();											\
			PRINTING_INIT_COMPLETED;									\
			PRINTING_TESTING_SG(CMP_NAME);								\
			CMP_TEST(TESTING_PROC_NAME, VALUE, CMP_NAME)                \
			PRINTING_TEST_IS_COMPLETED;									\
	}																	\


#define CMP_DEF_MCTOR(PROC_NAME,										\
					  TYPE, 	 										\
					  CMP_NAME, 										\
					  INIT_PROC_NAME, 									\
					  TESTING_PROC_NAME)            					\
	TYPE* PROC_NAME()													\
	{																	\
			TYPE* result = NULL;										\
			CMP_MCTOR_PROC(TYPE, result, CMP_NAME)						\
			PRINTING_INIT_SG(CMP_NAME);									\
			INIT_PROC_NAME(result);										\
			PRINTING_INIT_COMPLETED;									\
			PRINTING_TESTING_SG(CMP_NAME);								\
			CMP_TEST(TESTING_PROC_NAME, result, CMP_NAME)               \
			PRINTING_TEST_IS_COMPLETED;									\
			return result;												\
	}																	\


#define CMP_DTOR_PROC(TYPE, VALUE, CMP_NAME)	                        \
		PRINTING_DESTRUCTING_SG(CMP_NAME);								\
		if(VALUE == NULL){												\
			logging("%s", CMP_NAME" is already destructed or never exists");  \
			return;														\
		}																\
		free(VALUE);													\
		PRINTING_SG_IS_DESTRUCTED(CMP_NAME);							\



#define CMP_DEF_DTOR(PROC_NAME, 										\
					  TYPE, 											\
					  VALUE, 											\
					  CMP_NAME, 										\
					  DEINIT_PROC_NAME)   								\
	void PROC_NAME()													\
	{																	\
			PRINTING_DEINIT_SG(CMP_NAME);								\
			DEINIT_PROC_NAME();											\
			CMP_DTOR_PROC(TYPE, VALUE, CMP_NAME);						\
	}																	\


#define CMP_DEF_MDTOR(PROC_NAME, 										\
					  TYPE, 											\
					  CMP_NAME, 										\
					  DEINIT_PROC_NAME)   								\
	void PROC_NAME(void *cmp)											\
	{																	\
			PRINTING_DEINIT_SG(CMP_NAME);								\
			DEINIT_PROC_NAME(cmp);										\
			CMP_DTOR_PROC(TYPE, cmp, CMP_NAME);							\
	}																	\




#define CMP_CHECK_EXISTANCE(CMP_VAR, CMP_NAME)							\
	ASSERT_TRUE(CMP_VAR != NULL, GET_TEXT_SG_NOTEXISTS(CMP_NAME));




#define CMP_DEF(DECL_TYPE,													\
				 CMP_TYPE, 													\
				 CMP_NAME,													\
				 CMP_VAR,													\
				 CTOR_PROC_NAME,											\
				 DTOR_PROC_NAME,											\
				 INIT_PROC_NAME,											\
				 TESTING_PROC_NAME,											\
				 DEINIT_PROC_NAME)											\
						 	 	 	 	 	 	 	 	 	 	 	 	 	\
	CMP_STAT_DEF_VAR(CMP_TYPE, CMP_VAR);									\
																			\
	DECL_TYPE CMP_DEF_CTOR(CTOR_PROC_NAME, 									\
		CMP_TYPE, 															\
		CMP_VAR, 															\
		CMP_NAME,															\
		INIT_PROC_NAME,														\
		TESTING_PROC_NAME);													\
																			\
	DECL_TYPE CMP_DEF_DTOR(DTOR_PROC_NAME,									\
		CMP_TYPE, 															\
		CMP_VAR, 															\
		CMP_NAME,															\
		DEINIT_PROC_NAME);													\



#define CMP_MDEF(DECL_TYPE,													\
				 CMP_TYPE, 													\
				 CMP_NAME,													\
				 CTOR_PROC_NAME,											\
				 DTOR_PROC_NAME,											\
				 INIT_PROC_NAME,											\
				 TESTING_PROC_NAME,											\
				 DEINIT_PROC_NAME)											\
						 	 	 	 	 	 	 	 	 	 	 	 	 	\
																			\
	DECL_TYPE CMP_DEF_MCTOR(CTOR_PROC_NAME, 								\
		CMP_TYPE, 															\
		CMP_NAME,															\
		INIT_PROC_NAME,														\
		TESTING_PROC_NAME);													\
																			\
	DECL_TYPE CMP_DEF_MDTOR(DTOR_PROC_NAME,									\
		CMP_TYPE, 															\
		CMP_NAME,															\
		DEINIT_PROC_NAME);													\




#define CMP_DEF_GET_PROC(PROC_NAME, CMP_TYPE, CMP_VAR)						\
	CMP_TYPE* PROC_NAME()													\
	{																		\
		return CMP_VAR;														\
	}																		\

#define CMP_BIND(lvalue, rvalue)				    						\
		PRINTING_BIND_SG_TO_PROC(lvalue, rvalue);							\
		lvalue = rvalue;													\


#define CMP_CONNECT(lvalue, rvalue)		            						\
		PRINTING_CONNECT_SG1_TO_SG2(lvalue, rvalue);						\
		lvalue = rvalue;													\

#define CMP_ASSIGN_SG1_TO_SG2(lvalue, rvalue)		            			\
		PRINTING_ASSIGNING_SG1_TO_SG2(lvalue, rvalue);					    \
		lvalue = rvalue;													\

#define CMP_DECL_SGPUFFER(TYPE_NAME, ITEM_TYPE) 							\
	typedef struct TYPE_NAME##_struct_t 									\
	{ 																		\
		datapuffer_t* puffer; 												\
		void         (*receiver)(ITEM_TYPE*); 								\
		ITEM_TYPE*   (*supplier)(); 										\
		signal_t      *signal; 												\
	}TYPE_NAME##_t;


#define CMP_DECL_RECPUFFER(TYPE_NAME, ITEM_TYPE) 							\
	typedef struct TYPE_NAME##_struct_t 									\
	{ 																		\
		datapuffer_t* puffer; 												\
		void         (*receiver)(ITEM_TYPE*); 								\
		ITEM_TYPE*   (*supplier)(); 										\
		spin_t      *spin; 													\
	}TYPE_NAME##_t;

//declare and define a puffer component using signals
#define CMP_DEF_SGPUFFER(DECL_TYPE,											\
				 ITEM_TYPE,													\
				 ITEM_DTOR,													\
				 CMP_NAME,													\
				 CMP_VAR,													\
				 PUFFER_LENGTH,												\
				 CTOR_PROC_NAME,											\
				 DTOR_PROC_NAME												\
				 )															\
				 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	\
		CMP_DECL_SGPUFFER(CMP_VAR, ITEM_TYPE);								\
		static void CMP_VAR##_init();										\
		static void CMP_VAR##_deinit();										\
																			\
		CMP_DEF(DECL_TYPE, 		 										    \
		CMP_VAR##_t,    										 	    	\
		 CMP_NAME,    		 								         	    \
		 CMP_VAR,        	  							 	  				\
		 CTOR_PROC_NAME,    	  											\
		 DTOR_PROC_NAME,     	   											\
		 CMP_VAR##_init,           											\
		 __NO_TEST_FUNC_,             										\
		 CMP_VAR##_deinit            										\
		);																	\
																			\
    CMP_DEF_PUFFER_RECV_WAIT(CMP_NAME, CMP_VAR##_t, CMP_VAR, ITEM_TYPE,     \
				CMP_VAR##_process_receiver)									\
																			\
	CMP_DEF_PUFFER_SUPPLY_WAIT(CMP_NAME, CMP_VAR##_t, CMP_VAR, ITEM_TYPE,	\
				CMP_VAR##_process_supplier)								    \
																			\
	void CMP_VAR##_init()													\
	{																		\
		CMP_VAR->puffer = datapuffer_ctor(PUFFER_LENGTH);					\
		CMP_VAR->signal = signal_ctor();									\
		CMP_BIND(CMP_VAR->receiver, CMP_VAR##_process_receiver)			    \
		CMP_BIND(CMP_VAR->supplier, CMP_VAR##_process_supplier)				\
	}																		\
																			\
	void CMP_VAR##_deinit()													\
	{																		\
		datapuffer_t*  puffer = CMP_VAR->puffer;							\
		GEN_PUFF_CLEAR_PROC(puffer, ITEM_TYPE, ITEM_DTOR);					\
		datapuffer_dtor(CMP_VAR->puffer);									\
		signal_dtor(CMP_VAR->signal);										\
	}																		\




//declare and define a puffer component usd for recycling types
#define CMP_DEF_RECPUFFER(DECL_TYPE,                                                                            \
                                 ITEM_TYPE,                                                                                                     \
                                 ITEM_CTOR,                                                                                                     \
                                 ITEM_DTOR,                                                                                                     \
                                 ITEM_CLEAN,                                                                                            \
                                 CMP_NAME,                                                                                                      \
                                 CMP_VAR,                                                                                                       \
                                 PUFFER_LENGTH,                                                                                         \
                                 CTOR_PROC_NAME,                                                                                        \
                                 DTOR_PROC_NAME                                                                                         \
                                 )                                                                                                                      \
                                                                                                                                                        \
                CMP_DECL_RECPUFFER(CMP_VAR, ITEM_TYPE);                                                         \
                static void CMP_VAR##_init();                                                                           \
                static void CMP_VAR##_deinit();                                                                         \
                                                                                                                                                        \
                CMP_DEF(DECL_TYPE,                                                                                                  \
                CMP_VAR##_t,                                                                                                    \
                 CMP_NAME,                                                                                                  \
                 CMP_VAR,                                                                                                               \
                 CTOR_PROC_NAME,                                                                                                \
                 DTOR_PROC_NAME,                                                                                                \
                 CMP_VAR##_init,                                                                                                \
                 __NO_TEST_FUNC_,                                                                                       \
                 CMP_VAR##_deinit                                                                                       \
                );                                                                                                                                      \
                                                                                                                                                        \
        static void CMP_VAR##_process_receiver(ITEM_TYPE* item)                                 \
        {                                                                                                                                               \
                CMP_DEF_THIS(CMP_VAR##_t, CMP_VAR);                                                                     \
                datapuffer_t    *puffer = this->puffer;                                                         \
                spin_t          *spin = this->spin;                                                                     \
                spin_lock(spin);                                                                                                        \
                                                                                                                                                        \
                /*CMP_RECEIVE(CMP_NAME, item);  */                                                                      \
                /*if(puffer->is_full == BOOL_TRUE){                                                               */\
                if(datapuffer_isfull(puffer) == BOOL_TRUE){                                                     \
                        runtime_warning("%s is full", CMP_NAME);                                                \
                        spin_unlock(spin);                                                                                      \
                        ITEM_DTOR(item);                                                                                                \
                        return;                                                                                                                 \
                }                                                                                                                                       \
                                                                                                                                                        \
                ITEM_CLEAN(item);                                                                                                       \
                datapuffer_write(puffer, (void*) item);                                                         \
                                                                                                                                                        \
                spin_unlock(spin);                                                                                              \
        }/*#PROC_NAME end*/                                                                                                             \
                                                                                                                                                        \
        static ITEM_TYPE* CMP_VAR##_process_supplier()                                                  \
        {                                                                                                                                               \
                CMP_DEF_THIS(CMP_VAR##_t, CMP_VAR);                                                                     \
                ITEM_TYPE       *result = NULL;                                                                         \
                datapuffer_t    *puffer = this->puffer;                                                         \
                spin_t          *spin = this->spin;                                                                     \
                                                                                                                                                        \
                spin_lock(spin);                                                                                                \
                /*if(puffer->is_empty == BOOL_TRUE){                                                      */\
            if(datapuffer_isempty(puffer) == BOOL_TRUE){                                                \
                        logging("%s is empty", CMP_NAME);                                                               \
                        spin_unlock(spin);                                                                                              \
                        result = ITEM_CTOR();                                                                                   \
                        return result;                                                                                                  \
                }                                                                                                                                       \
                                                                                                                                                        \
                result = (ITEM_TYPE*) datapuffer_read(puffer);                                          \
                                                                                                                                                        \
                /*CMP_SUPPLY(CMP_NAME, result);*/                                                                       \
                spin_unlock(spin);                                                                                              \
                return result;                                                                                                          \
        }/*#PROC_NAME end*/                                                                                                     \
                                                                                                                                                        \
        void CMP_VAR##_init()                                                                                                   \
        {                                                                                                                                               \
                CMP_VAR->puffer = datapuffer_ctor(PUFFER_LENGTH);                                       \
                CMP_VAR->spin = spin_ctor();                                                                            \
                CMP_BIND(CMP_VAR->receiver, CMP_VAR##_process_receiver)                     \
                CMP_BIND(CMP_VAR->supplier, CMP_VAR##_process_supplier)                         \
        }                                                                                                                                               \
                                                                                                                                                        \
        void CMP_VAR##_deinit()                                                                                                 \
        {                                                                                                                                               \
                datapuffer_t*  puffer = CMP_VAR->puffer;                                                        \
                GEN_PUFF_CLEAR_PROC(puffer, ITEM_TYPE, ITEM_DTOR);                                      \
                datapuffer_dtor(CMP_VAR->puffer);                                                                       \
                spin_dtor(CMP_VAR->spin);                                                                                       \
        }                                                                                                                                               \




#define CMP_THREAD(													\
				DECL_TYPE,											\
				CMP_TYPE,											\
				CMP_UNIQUE,											\
				CMP_NAME,											\
				CMP_INIT_PROC_NAME, 								\
				CMP_DEINIT_PROC_NAME, 								\
				THR_START_THREAD_PROC_NAME, 						\
				THR_STOP_THREAD_PROC_NAME, 							\
				THR_MAIN_PROC_NAME									\
				)													\
	DECL_TYPE thread_t* _thr_##CMP_UNIQUE = NULL;					\
	LIB_DEF_THREAD(_thr_##CMP_UNIQUE##_entrypoint, 					\
		   __THR_NO_INIT_PROC__,      								\
		   THR_MAIN_PROC_NAME,  						 			\
		   __THR_NO_DEINIT_PROC__         							\
		  );														\
	CMP_DEF(DECL_TYPE,												\
				CMP_TYPE,       									\
				CMP_NAME,   										\
				_cmp_##CMP_UNIQUE,									\
				_cmp_##CMP_UNIQUE##_ctor,           				\
				_cmp_##CMP_UNIQUE##_dtor,           				\
				 CMP_INIT_PROC_NAME,              					\
				 __NO_TEST_FUNC_,             						\
				 CMP_DEINIT_PROC_NAME               				\
				);													\
DECL_TYPE void* THR_START_THREAD_PROC_NAME() 						\
{																	\
	PRINTING_STARTING_SG(CMP_NAME);									\
																	\
	_cmp_##CMP_UNIQUE##_ctor();										\
	_thr_##CMP_UNIQUE = make_thread(_thr_##CMP_UNIQUE##_entrypoint, _cmp_##CMP_UNIQUE); \
																	\
	start_thread(_thr_##CMP_UNIQUE);								\
																	\
	PRINTING_SG_IS_STARTED(CMP_NAME);								\
																	\
	return NULL;													\
}																	\
																	\
DECL_TYPE void* THR_STOP_THREAD_PROC_NAME() 						\
{																	\
	CMP_TYPE *cmp = _cmp_##CMP_UNIQUE;								\
	thread_t *thread = _thr_##CMP_UNIQUE;							\
	PRINTING_STOPPING_SG(CMP_NAME);									\
																	\
	stop_thread(thread);											\
	_cmp_##CMP_UNIQUE##_dtor(cmp);									\
	_thr_##CMP_UNIQUE = NULL;										\
	_cmp_##CMP_UNIQUE = NULL;										\
	PRINTING_SG_IS_STOPPED(CMP_NAME);								\
																	\
	return NULL;													\
}																	\


#define CMP_PUSH_THREADER(											\
				ITEM_TYPE,											\
				CMP_UNIQUE,											\
				CMP_NAME,											\
				THR_START_THREAD_PROC_NAME, 						\
				THR_STOP_THREAD_PROC_NAME 							\
				)													\
																	\
typedef struct _cmp_##CMP_UNIQUE##_struct_t							\
{																	\
	signal_t *signal;												\
	void     (*receiver)(ITEM_TYPE*);								\
	ITEM_TYPE* item;												\
	void     (*send)(ITEM_TYPE*);									\
}_cmp_##CMP_UNIQUE##_t;												\
static void _cmp_##CMP_UNIQUE##_receiver(ITEM_TYPE*);				\
static void _cmp_##CMP_UNIQUE##_init();								\
static void _cmp_##CMP_UNIQUE##_deinit();							\
static void _cmp_##CMP_UNIQUE##_receiver(ITEM_TYPE *item);			\
static void _thr_##CMP_UNIQUE##_main_proc(thread_t *thread);		\
CMP_THREAD(															\
				static,											    \
				_cmp_##CMP_UNIQUE##_t,								\
				CMP_UNIQUE,											\
				CMP_NAME,											\
				_cmp_##CMP_UNIQUE##_init, 							\
				_cmp_##CMP_UNIQUE##_deinit, 						\
				THR_START_THREAD_PROC_NAME, 						\
				THR_STOP_THREAD_PROC_NAME, 							\
				_thr_##CMP_UNIQUE##_main_proc						\
				)													\
void _cmp_##CMP_UNIQUE##_init()										\
{																	\
	CMP_DEF_THIS(_cmp_##CMP_UNIQUE##_t, _cmp_##CMP_UNIQUE);			\
	this->signal = signal_ctor();									\
	this->item = NULL;												\
	CMP_BIND(_cmp_##CMP_UNIQUE->receiver, _cmp_##CMP_UNIQUE##_receiver); \
}																	\
void _cmp_##CMP_UNIQUE##_deinit()									\
{																	\
	CMP_DEF_THIS(_cmp_##CMP_UNIQUE##_t, _cmp_##CMP_UNIQUE);			\
	signal_dtor(this->signal);										\
}																	\
void _cmp_##CMP_UNIQUE##_receiver(ITEM_TYPE *item)					\
{																	\
	CMP_DEF_THIS(_cmp_##CMP_UNIQUE##_t, _cmp_##CMP_UNIQUE);			\
	signal_lock(this->signal);										\
	this->item = item;												\
	signal_set(this->signal);										\
	signal_unlock(this->signal);									\
}																	\
void _thr_##CMP_UNIQUE##_main_proc(thread_t *thread)				\
{																	\
	CMP_DEF_THIS(_cmp_##CMP_UNIQUE##_t, (_cmp_##CMP_UNIQUE##_t*) thread->arg);		\
	do																\
	{																\
		signal_lock(this->signal);									\
		signal_wait(this->signal);									\
		/*CMP_SEND(CMP_NAME, this->send, this->item);*/				\
		this->send(this->item);										\
		signal_unlock(this->signal);								\
	}while(thread->state == THREAD_STATE_RUN);						\
}																	\




#define CMP_THREADS(												\
				DECL_TYPE,											\
				CMP_TYPE,											\
				CMP_UNIQUE,											\
				CMP_NAME,											\
				CHAIN_CTOR_PROC_NAME,								\
				CHAIN_DTOR_PROC_NAME,								\
				CHAIN_ITR_PROC_NAME,								\
				THR_INIT_PROC_NAME, 								\
				CMP_INIT_PROC_NAME,									\
				THR_DEINIT_PROC_NAME,								\
				CMP_DEINIT_PROC_NAME, 								\
				THR_MAIN_PROC_NAME,									\
				THR_ACTIVATOR_TYPE, 								\
				CMP_ACTIVATOR_ATTR, 								\
				THR_START_THREAD_PROC_NAME, 						\
				THR_STOP_THREAD_PROC_NAME 							\
				)													\
	CMP_MDEF(DECL_TYPE,												\
			CMP_TYPE,       									  	\
			CMP_NAME,   										    \
			_cmp_##CMP_UNIQUE##_ctor,           					\
			_cmp_##CMP_UNIQUE##_dtor,           					\
			 CMP_INIT_PROC_NAME,              						\
			 __NO_TEST_FUNC_,             							\
			 CMP_DEINIT_PROC_NAME               					\
			);														\
typedef struct _thrs_##CMP_UNIQUE##_struct_t						\
{																	\
	thread_t*      threads[NTRT_MAX_THREAD_NUM];						\
	rwmutex_t*     rwmutex;											\
}_thrs_##CMP_UNIQUE##_t;											\
																	\
static _thrs_##CMP_UNIQUE##_t _thrs_##CMP_UNIQUE;					\
DECL_TYPE void CHAIN_CTOR_PROC_NAME()								\
{																	\
	int32_t  index;													\
	_thrs_##CMP_UNIQUE.rwmutex = rwmutex_ctor();					\
	for(index = 0; index < NTRT_MAX_THREAD_NUM; ++index){			\
		_thrs_##CMP_UNIQUE.threads[index] = NULL;					\
	}																\
}																	\
																	\
DECL_TYPE void CHAIN_DTOR_PROC_NAME()								\
{																	\
	rwmutex_dtor(_thrs_##CMP_UNIQUE.rwmutex);						\
}																	\
																	\
DECL_TYPE void _rdlock_##CMP_UNIQUE()								\
{																	\
	debug_rdlock(CMP_NAME);											\
	rwmutex_read_lock(_thrs_##CMP_UNIQUE.rwmutex);					\
}																	\
																	\
DECL_TYPE void _rdunlock_##CMP_UNIQUE()								\
{																	\
	debug_rdunlock(CMP_NAME);										\
	rwmutex_read_unlock(_thrs_##CMP_UNIQUE.rwmutex);				\
}																	\
																	\
DECL_TYPE void _wrlock_##CMP_UNIQUE()								\
{																	\
	debug_wrlock(CMP_NAME);											\
	rwmutex_write_lock(_thrs_##CMP_UNIQUE.rwmutex);					\
}																	\
																	\
DECL_TYPE void _wrunlock_##CMP_UNIQUE()								\
{																	\
	debug_wrunlock(CMP_NAME);										\
	rwmutex_write_unlock(_thrs_##CMP_UNIQUE.rwmutex);				\
}																	\
																	\
DECL_TYPE bool_t CHAIN_ITR_PROC_NAME(int32_t* index, thread_t **thread) \
{																	\
	*thread = NULL;													\
	for(; *index < NTRT_MAX_THREAD_NUM								\
		&& _thrs_##CMP_UNIQUE.threads[*index] == NULL; ++(*index)); \
	if(*index == NTRT_MAX_THREAD_NUM){								\
		return BOOL_FALSE;											\
	}																\
	*thread = _thrs_##CMP_UNIQUE.threads[*index];					\
	return BOOL_TRUE;												\
}																	\
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	\
LIB_DEF_THREAD(_thr_##CMP_UNIQUE##_entrypoint, 						\
		   THR_INIT_PROC_NAME,      								\
		   THR_MAIN_PROC_NAME,  						 			\
		   THR_DEINIT_PROC_NAME         							\
		  );														\
DECL_TYPE void* THR_START_THREAD_PROC_NAME(THR_ACTIVATOR_TYPE *activator) 	\
{																	\
	int32_t   index;												\
	CMP_TYPE *cmp;													\
	thread_t *thread;												\
	PRINTING_STARTING_SG(CMP_NAME);									\
																	\
	cmp = _cmp_##CMP_UNIQUE##_ctor();								\
	thread = make_thread(_thr_##CMP_UNIQUE##_entrypoint, cmp);		\
	cmp->CMP_ACTIVATOR_ATTR = activator;							\
	_rdlock_##CMP_UNIQUE();											\
	for(index = 0; _thrs_##CMP_UNIQUE.threads[index] != NULL; ++index); \
	_rdunlock_##CMP_UNIQUE();										\
	_wrlock_##CMP_UNIQUE();											\
	_thrs_##CMP_UNIQUE.threads[index] = thread;						\
	_wrunlock_##CMP_UNIQUE();										\
	start_thread(thread);											\
																	\
	PRINTING_SG_IS_STARTED(CMP_NAME);								\
																	\
	return NULL;													\
}																	\
																	\
DECL_TYPE void* THR_STOP_THREAD_PROC_NAME(THR_ACTIVATOR_TYPE *activator) 		\
{																	\
	CMP_TYPE *cmp = NULL;											\
	thread_t *thread = NULL;										\
	int32_t   index;												\
	PRINTING_STOPPING_SG(CMP_NAME);									\
																	\
	_wrlock_##CMP_UNIQUE();											\
	for(index = 0; 													\
		CHAIN_ITR_PROC_NAME(&index, &thread) == BOOL_TRUE; 			\
		++index) {													\
		cmp = (CMP_TYPE*) thread->arg;								\
		if(cmp->CMP_ACTIVATOR_ATTR == activator){break;}			\
	}																\
	if(thread == NULL){												\
		runtime_warning("Didn't find component for activator %p",	\
						activator);  								\
		return NULL; 												\
	}																\
	_thrs_##CMP_UNIQUE.threads[index] = NULL;						\
	_wrunlock_##CMP_UNIQUE();										\
	stop_thread(thread);											\
	thread_dtor(thread);											\
	_cmp_##CMP_UNIQUE##_dtor(cmp);									\
	PRINTING_SG_IS_STOPPED(CMP_NAME);								\
																	\
	return NULL;													\
}																	\


#endif /* INCGUARD_CMP_PREDEFS_H_ */
