/**
 * @file lib_tors.h
 * @brief constructors for structures defined in the library
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @developers Balázs Kreith, Debrecen Hungary
 * @date 2014.02.11
*/
#ifndef INCGUARD_NTRT_LIBRARY_CONSTRUCTORS_H_
#define INCGUARD_NTRT_LIBRARY_CONSTRUCTORS_H_
#include "lib_defs.h"
#include "lib_descs.h"


//-------------------------------------------------------------------------------------
//----------------------- Constructors--------------------------------------------------
//-------------------------------------------------------------------------------------

/** \fn callback_t* cback_ctor()
      \brief Initializes a new instance of the callback_t
	  \return Returns with a pointer pointing to an initialized callback_t
  */
callback_t*	cback_ctor();

/** \fn rcallback_t* cback_ctor()
      \brief Initializes a new instance of the rcallback_t
	  \return Returns with a pointer pointing to an initialized rcallback_t
  */
rcallback_t* rcback_ctor();

/** \fn interface_t* interface_ctor()
      \brief Initializes a new instance of the interface_t
	  \return Returns with a pointer pointing to an initialized interface_t
*/
string_t* string_ctor();

/** \fn lstring_t* lstring_ctor()
      \brief Initializes a new instance of the lstring_t
	  \return Returns with a pointer pointing to an initialized lstring_t
  */
lstring_t* lstring_ctor();

/** \fn datarray_t* datarray_ctor()
      \brief Initializes a new instance of the datarray_t
	  \param length determines the length of the array
	  \return Returns with a pointer pointing to an initialized datarray_t
  */
datarray_t* datarray_ctor(int32_t length);

/** \fn datchain_t* datchain_ctor()
      \brief Initializes a new instance of the datchain_t
	  \return Returns with a pointer pointing to an initialized path datchain_t
  */
datchain_t* datchain_ctor();

/** \fn datchain_t* cmdres_t()
      \brief Initializes a new instance of the cmdres_t
	  \return Returns with a pointer pointing to an initialized path cmdres_t
  */

/** \fn byte_t* bytes_ctor(int32_t size)
      \brief Initializes a new instance of the array of byte_t 
	  \return Returns with a pointer pointing to an initialized path byte_t
  */
byte_t* bytes_ctor(int32_t);

/** \fn eventer_arg_t* request_ctor(int32_t size)
      \brief Initializes a new instance of the array of request_t
	  \return Returns with a pointer pointing to an initialized path request_t
  */
eventer_arg_t* eventer_arg_ctor();

array_t* array_ctor(int32_t length);

groupcounter_prototype_t* groupcounter_prototype_ctor();

groupcounter_t* groupcounter_ctor();

feature_t* feature_ctor();

pcap_listener_t* pcap_listener_ctor();

record_t* record_ctor();

evaluator_item_t* evaluator_item_ctor();

mapped_var_t* mapped_var_ctor();
//-------------------------------------------------------------------------------------
//----------------------- Destructors--------------------------------------------------
//-------------------------------------------------------------------------------------

/** \fn void cback_dtor(void*)
      \brief Destroy an instance of a callback_t
  */
void cback_dtor(void* callback);

/** \fn void cback_dtor(void*)
      \brief Destroy an instance of a callback_t
  */
void rcback_dtor(void* rcallback);
/** \fn void interface_dtor(void*)
      \brief Destroy an instance of a interface_t
  */
void interface_dtor(void* interface_);

/** \fn void string_dtor(void*)
      \brief Destroy an instance of a string_t
  */
void string_dtor(void* string);

/** \fn void lstring_dtor(void*)
      \brief Destroy an instance of a lstring_t
  */
void lstring_dtor(void* lstring);

/** \fn void datarray_dtor(void*)
      \brief Destroy an instance of a datarray_t
	  \param length determines the length of the array
  */
void datarray_dtor(void* datarray);

/** \fn void datchain_dtor(void*)
      \brief Destroy an instance of a datchain_t
  */
void datchain_dtor(void* datchain, void (*data_dtor)(ptr_t));


/** \fn void bytes_dtor(void*)
      \brief Destroy an instance of a bytearray
  */
void bytes_dtor(void* target);

/** \fn void eventer_arg_dtor(void*)
      \brief Destroy an instance of a eventer_arg
  */
void eventer_arg_dtor(eventer_arg_t *eventer_arg);

/** \fn void clock_event_dtor_dtor(void*)
      \brief Destroy an instance of a clock_event_dtor
  */
void clock_event_dtor(void *clock_event);

void array_dtor(void *target);

void feature_dtor(ptr_t target);

void groupcounter_prototype_dtor(ptr_t target);

void pcap_listener_dtor(ptr_t target);

void record_dtor(ptr_t target);

void evaluator_item_dtor(ptr_t target);

void groupcounter_dtor(ptr_t target);

void mapped_var_dtor(ptr_t target);

void slist_dtor(ptr_t target, void (*data_dtor)(ptr_t));

#endif //INCGUARD_NTRT_LIBRARY_CONSTRUCTORS_H_
