/**
 * @file lib_makers.h
 * @brief base makers
 * @author Bal�zs, Kreith; Debrecen, Hungary
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @date 2014.02.11
*/
#ifndef INCGUARD_NTRT_LIBRARY_MAKERS_H_
#define INCGUARD_NTRT_LIBRARY_MAKERS_H_
#include "lib_defs.h"
#include "lib_descs.h"
#include "inc_predefs.h"


/** \fn callback_t* make_callback(void (*action)(), void (*parameterized_action)(void*), void *action_parameter)
      \brief Make a callback
	  \param action Pointing to a simple void fn()
	  \param paction Pointing to a parameterized action to call
	  \param aparam Poitning to a parameter passed to a parameterized action
	  \return Returns a prepared callback
  */
callback_t* make_callback(void (*action)(), 
						  void (*paction)(void*), 
						  void *aparam);

/** \fn callback_t* make_callback_sa(void (*action)())
      \brief Make a callback with a simple action
	  \param action Pointing to a simple void fn()
	  \return Returns a prepared callback
  */
callback_t* make_callback_sa(void (*action)());

/** \fn callback_t* make_callback_pa(void (*parameterized_action)(void*), void *action_parameter)
     \brief Make a callback with a parameterized action
	 \param paction Pointing to a parameterized action to call
	 \param aparam Poitning to a parameter passed to a parameterized action
	 \return Returns a prepared callback
  */
callback_t* make_callback_pa(void (*paction)(void *), void *aparam);

/** \fn callback_t* make_rcallback(void (*action)(), void (*parameterized_action)(void*), void *action_parameter)
      \brief Make a callback
	  \param action Pointing to a simple void* fn()
	  \param paction Pointing to a parameterized action to call
	  \param aparam Poitning to a parameter passed to a parameterized action
	  \return Returns a prepared callback
  */
rcallback_t* make_rcallback(void* (*action)(), 
						  void* (*paction)(void*), 
						  void *aparam);

/** \fn rcallback_t* make_rcallback_sa(void (*action)())
      \brief Make a callback with a simple action
	  \param action Pointing to a simple void* fn()
	  \return Returns a prepared callback
  */
rcallback_t* make_rcallback_sa(void* (*action)());

/** \fn callback_t* make_rcallback_pa(void* (*parameterized_action)(void*), void *action_parameter)
     \brief Make a callback with a parameterized action
	 \param paction Pointing to a parameterized action to call
	 \param aparam Poitning to a parameter passed to a parameterized action
	 \return Returns a prepared callback
  */
rcallback_t* make_rcallback_pa(void* (*paction)(void *), void *aparam);

/** \fn callback_t* make_command(callback_t *action, callback_t *cback)
     \brief Make a command
     \param desc Pointing to a character string describes the command
	 \param action Pointing to an callback an executor will call when executing this command
	 \param cback Pointing to an callback an executor will call after it executed the command
	 \return Returns a prepared command
  */

/** \fn callback_t* make_string(char_t* characters)
     \brief make a string and copy the characters from the passed argument
	 \param paction Pointing to the characters which is going to be copied to the string
	 \return Returns a string contains a copy of the characters
  */
string_t* make_string(char_t* characters);

/** \fn callback_t* make_thread(void *(*method)(void*))
     \brief make a thread and register it into dmap_table_thr
	 \param method Pointing to the entry point of the thread
	 \param dmap_thr_id Pointing to the argument
	 \return Returns a thread
  */
thread_t *make_thread(void *(*method)(void*), void *arg);

barrier_t *make_barrier(int32_t gatenum);

eventer_arg_t *make_eventer_arg(int32_t event, void *arg);

feature_t* make_feature(const char_t* name,
                        const char_t* identifier,
                        uint32_t (*evaluator)(sniff_t*,evaluator_container_t*));

groupcounter_prototype_t* make_groupcounter_prototype(const char_t* identifier,
                                                      void     (*setup)(groupcounter_t*, ptr_t),
                                                      void     (*add_sniff)(groupcounter_t*,sniff_t*),
                                                      int32_t  (*get_counter)(groupcounter_t*),
                                                      void     (*deinit)(groupcounter_t*)
                                                      );

groupcounter_t* make_groupcounter(groupcounter_prototype_t* prototype);

datchain_t* make_datchain(ptr_t item);

evaluator_item_t* make_evaluator_item(feature_t* feature, evaluator_container_t* evaluator_container);

slist_t* make_slist(ptr_t item);
dlist_t* make_dlist(ptr_t item);

mapped_var_t* make_mapped_var(int32_t id);

#endif //INCGUARD_NTRT_LIBRARY_MAKERS_H_
