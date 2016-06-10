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
command_t* make_command(char_t *desc, rcallback_t* action, callback_t* cback);


/** \fn callback_t* make_command_sa(void(*action)())
     \brief Make a command with a simple action
     \param desc Pointing to a character string describes the command
	 \param action Pointing to a void fn() an executor will call when executing this command
	 \return Returns a prepared command
  */
command_t* make_command_sa(char_t *desc, void*(*action)());

/** \fn callback_t* make_command_pa(void(*paction)(void*), void *aparam)
     \brief Make a command with a parametirezed action
     \param desc Pointing to a character string describes the command
	 \param paction Pointing to a void fn(void*) function an executor will call when executing this command
	 \param aparam Pointing to a void* type parameter an executor will pass to the called void fn(void*) function
	 \return Returns a prepared command
  */
command_t* make_command_pa(char_t *desc, void*(*paction)(void*), void *aparam);

/** \fn callback_t* make_command_sa_sc(void(*action)(), void (*callback)())
     \brief Make a command with a simple action and a simple callback
     \param desc Pointing to a character string describes the command
	 \param Pointing to a void fn() function an executor will call when executing this command
	 \param Pointing to a void fn() function an executor will call after it executed the command
	 \return a prepared command
  */
command_t* make_command_sa_sc(char_t *desc, void*(*action)(), void (*callback)());

/** \fn callback_t* make_command_sa_sc(void(*action)(void*), void *action_parameter)
     \brief make a command with a simple action and a simple callback
     \param desc Pointing to a character string describes the command
	 \param pointing to a void fn() function an executor will call when executing this command
	 \param pointing to a void fn() function an executor will call after it executed the command
	 \return a prepared command
  */
command_t* make_command_pa_sc(char_t *desc, void*(*paction)(void*), void *aparam, void (*callback)());

/** \fn callback_t* make_command_sa_sc(void(*action)(void*), void *action_parameter)
     \brief make a command with simple action and parametrezied callback
     \param desc Pointing to a character string describes the command
	 \param action Pointing to a void fn() function an executor will call when executing this command
	 \param cback Pointing to a void fn(void*) function an executor will call after it executed this command
	 \param cparam Pointing to a void* type parameter an executor will pass to the called void fn(void*) function
	 \return a prepared command
  */
command_t* make_command_sa_pc(char_t *desc, void*(*action)(), void (*cback)(void*), void *cparam);


/** \fn callback_t* make_command_sa_sc(void(*action)(void*), void *action_parameter)
     \brief make a command with a parameterized action and a parameterized callback
     \param desc Pointing to a character string describes the command
	 \param paction Pointing to a void fn(void*) function an executor will call when executing this command
	 \param aparam Pointing to a void* type parameter an executor will pass to the called void fn(void*) function
	 \param cback Pointing to a void fn(void*) function an executor will call after it executed this command
	 \param cparam Pointing to a void* type parameter an executor will pass to the called void fn(void*) function
	 \return a prepared command
  */
command_t* make_command_pa_pc(char_t *desc, void*(*action)(void*), void*, void (*callback)(void*), void *callback_parameter);


/** \fn callback_t* make_packet(int32_t size)
     \brief Make a packet
	 \param size determining the size of the bytes the packet will contains.
	 \return Returns a prepared packet
  */
packet_t* make_packet();

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


request_t *make_request(byte_t command, byte_t status, byte_t *message, connection_t *connection, int32_t size);

eventer_arg_t *make_eventer_arg(int32_t event, void *arg);

#endif //INCGUARD_NTRT_LIBRARY_MAKERS_H_
