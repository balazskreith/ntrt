#ifndef INCGUARD_LIB_PREDEFS_H_
#define INCGUARD_LIB_PREDEFS_H_
#include "lib_defs.h"

void func_do_nothing();
void func_with_param_do_nothing(void *param);
void func_with_2param_do_nothing(void *param);
bool_t func_return_true();
bool_t func_with_param_return_true(void *param);

#define __THR_NO_INIT_PROC__ func_with_param_do_nothing
#define __THR_NO_DEINIT_PROC__ func_with_param_do_nothing

#define __THR_NO_ACT_INIT_PROC__ func_with_param_do_nothing
#define __THR_NO_DEACT_DEINIT_PROC__ func_with_param_do_nothing

#define LIB_DEF_THREAD( 							\
					   THR_PROC_NAME,  				\
					   THR_INIT_PROC_NAME, 			\
					   THR_MAIN_PROC_NAME, 			\
					   THR_DEINIT_PROC_NAME 		\
					  ) 							\
	static void* THR_PROC_NAME(void *arg) 			\
	{												\
		thread_t *thread = (thread_t*) arg;         \
		THR_INIT_PROC_NAME(thread);					\
		thread->state = THREAD_STATE_RUN;			\
		THR_MAIN_PROC_NAME(thread);					\
		thread->state = THREAD_STATE_STOPPED;		\
		THR_DEINIT_PROC_NAME(thread);				\
		return NULL;								\
	}												\


#define LIB_DEF_CTOR(TYPE, PROC_NAME)					\
		TYPE* PROC_NAME()								\
		{												\
			TYPE *result;								\
			result = (TYPE*) malloc(sizeof(TYPE));		\
			memset(result, 0, sizeof(TYPE));			\
			return result;								\
		}												\


#define LIB_DEF_DTOR(TYPE, PROC_NAME)					\
		PROC_NAME(void* param)							\
		{												\
			TYPE* target;								\
			LOGGING_SG_IS_CALLED(#TYPE"_dtor");			\
			if(param == NULL){							\
				return;									\
			}											\
			target = (TYPE*) param;						\
			free(target);								\
			target = NULL;								\
		}												\

#endif /* INCGUARD_LIB_PREDEFS_H_ */
