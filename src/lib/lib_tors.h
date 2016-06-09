/**
 * @file lib_tors.h
 * @brief constructors for structures defined in the library
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @developers Balázs Kreith, Debrecen Hungary
 * @date 2014.02.11
*/
#ifndef INCGUARD_MPT_LIBRARY_CONSTRUCTORS_H_
#define INCGUARD_MPT_LIBRARY_CONSTRUCTORS_H_
#include "lib_defs.h"
#include "lib_descs.h"


//-------------------------------------------------------------------------------------
//----------------------- Constructors--------------------------------------------------
//-------------------------------------------------------------------------------------

/** \fn packet_t* packet_ctor()
      \brief Initializes a new instance of the packet_t
	  \return Returns with a pointer pointing to an initialized packet_t
  */
packet_t* packet_ctor();

/** \fn command_t* cmd_ctor()
      \brief Initializes a new instance of the command_t
	  \return Returns with a pointer pointing to an initialized command_t
  */
command_t* cmd_ctor();

/** \fn command_t* handshake_ctor()
      \brief Initializes a new instance of the handshake_t
	  \return Returns with a pointer pointing to an initialized handshake_t
  */

tunnel_t* tunnel_ctor();


/** \fn connection_t* conn_ctor()
      \brief Initializes a new instance of the connection_t
	  \return Returns with a pointer pointing to an initialized connection_t
  */
connection_t* conn_ctor();

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
interface_t* interface_ctor();

/** \fn string_t* string_ctor()
      \brief Initializes a new instance of the string_t
	  \return Returns with a pointer pointing to an initialized string_t
  */
string_t* string_ctor();

/** \fn lstring_t* lstring_ctor()
      \brief Initializes a new instance of the lstring_t
	  \return Returns with a pointer pointing to an initialized lstring_t
  */
lstring_t* lstring_ctor();

/** \fn path_t* path_ctor()
      \brief Initializes a new instance of the path_t
	  \return Returns with a pointer pointing to an initialized path_t
  */
path_t*	path_ctor();

/** \fn network_t* network_ctor()
      \brief Initializes a new instance of the network_t
	  \return Returns with a pointer pointing to an initialized network_t
  */
network_t* network_ctor();

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
cmdres_t* cmdres_ctor();



/** \fn byte_t* bytes_ctor(int32_t size)
      \brief Initializes a new instance of the array of byte_t 
	  \return Returns with a pointer pointing to an initialized path byte_t
  */
byte_t* bytes_ctor(int32_t);


/** \fn byte_t* request_ctor(int32_t size)
      \brief Initializes a new instance of the array of request_t
	  \return Returns with a pointer pointing to an initialized path request_t
  */
request_t* request_ctor();

/** \fn pathring_t* request_ctor(int32_t size)
      \brief Initializes a new instance of the array of request_t
	  \return Returns with a pointer pointing to an initialized path request_t
  */
pathring_t* pathring_ctor();

/** \fn eventer_arg_t* request_ctor(int32_t size)
      \brief Initializes a new instance of the array of request_t
	  \return Returns with a pointer pointing to an initialized path request_t
  */
eventer_arg_t* eventer_arg_ctor();

array_t* array_ctor(int32_t length);

//-------------------------------------------------------------------------------------
//----------------------- Destructors--------------------------------------------------
//-------------------------------------------------------------------------------------

/** \fn void packet_dtor(packet_t*)
      \brief Destroy an instance of a packet_t
  */
void packet_dtor(packet_t* packet);

/** \fn void cmd_dtor(command_t*)
      \brief Destroy an instance of a command_t
  */
void cmd_dtor(command_t* command);

/** \fn void tunnel_dtor(void*)
      \brief Destroy an instance of a tunnel_t
  */
void tunnel_dtor(void* tunnel);

/** \fn void waitresponse_dtor(void*)
      \brief Destroy an instance of a wait_t
  */
void wait_dtor(void* waitsignal);

/** \fn void conn_dtor(void*)
      \brief Destroy an instance of a connection_t
  */
void conn_dtor(void* connection);

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

/** \fn void path_dtor(void*)
      \brief Destroy an instance of a path_t
  */
void path_dtor(void* path);

/** \fn void network_dtor(void*)
      \brief Destroy an instance of a network_t
  */
void network_dtor(void* network);

/** \fn void datarray_dtor(void*)
      \brief Destroy an instance of a datarray_t
	  \param length determines the length of the array
  */
void datarray_dtor(void* datarray);

/** \fn void datchain_dtor(void*)
      \brief Destroy an instance of a datchain_t
  */
void datchain_dtor(void* datchain);

/** \fn void datchain_dtor(void*)
      \brief Destroy an instance of a cmdres_t
  */
void cmdres_dtor(void* target);

/** \fn void keyval_ctor(void*)
      \brief Destroy an instance of a keyval_t
  */
void keyval_dtor(void *target);

/** \fn void map_dtor(void*)
      \brief Destroy an instance of a keyval_t
  */
void map_dtor(void *target);

/** \fn void bytes_dtor(void*)
      \brief Destroy an instance of a bytearray
  */
void bytes_dtor(void* target);

/** \fn void request_dtor(void*)
      \brief Destroy an instance of a request
  */
void request_dtor(request_t *request);

/** \fn void pathring_dtor(void*)
      \brief Destroy an instance of a pathring
  */
void pathring_dtor(pathring_t *pathring);

/** \fn void eventer_arg_dtor(void*)
      \brief Destroy an instance of a eventer_arg
  */
void eventer_arg_dtor(eventer_arg_t *eventer_arg);

/** \fn void clock_event_dtor_dtor(void*)
      \brief Destroy an instance of a clock_event_dtor
  */
void clock_event_dtor(void *clock_event);

void array_dtor(void *target);

#endif //INCGUARD_MPT_LIBRARY_CONSTRUCTORS_H_
