/**
 * @file lib_defs.h
 * @brief base definitions used by the mpt program
 * @author Bal�zs, Kreith; Debrecen, Hungary
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @date 2014.02.11
*/
#ifndef INCGUARD_NTRT_LIBRARY_DEFINITIONS_H_
#define INCGUARD_NTRT_LIBRARY_DEFINITIONS_H_

#include "inc_predefs.h"
#include "inc_inet.h"
#include "inc_mtime.h"


/** \def SIZE_INADDR
      \brief The size of an IPv4 address
  */
#define SIZE_INADDR  sizeof(struct in_addr)

/** \def SIZE_IN6ADDR
      \brief The size of an IPv6 address
  */
#define SIZE_IN6ADDR sizeof(struct in6_addr)

/** \def SIZE_DGRAM
      \brief The size of datagarams
  */
#define SIZE_DGRAM   65536


 
/** \typedef bool_t
      \brief Boolean constants
  */
typedef enum
{
	BOOL_FALSE = 0,  ///< Indicates a boolean false value
	BOOL_TRUE = 1    ///< Indicates a boolean true value
}bool_t;

#ifdef _WIN32 //atomic type declarations
/** \typedef uint64_t
      \brief unsigned intiger with 64bits long representation
  */
typedef unsigned long uint64_t;

/** \typedef int64_t
      \brief signed intiger with 64bits long representation
  */
typedef long int64_t;

/** \typedef uint32_t
      \brief unsigned intiger with 32bits long representation
  */
typedef unsigned int uint32_t;

/** \typedef uint32_t
      \brief signed intiger with 32bits long representation
  */
typedef int int32_t;

/** \typedef uint16_t
      \brief unsigned intiger with 16bits long representation
  */
typedef unsigned short uint16_t;

/** \typedef int16_t
      \brief signed intiger with 16bits long representation
  */
typedef short int16_t;

/** \typedef uint8_t
      \brief unsigned intiger with 8bits long representation
  */
typedef unsigned char uint8_t;

/** \typedef int8_t
      \brief signed intiger with 8bits long representation
  */
typedef char int8_t;

/** \typedef byte_t
      \brief indicates a single byte
  */

#else 
#include <sys/types.h>
#endif //atomic type declarations for windows

typedef char char_t;

/** \typedef ipversion_t
      \brief ip address versions
  */

typedef unsigned char byte_t;

/** \typedef char_t
      \brief indicates a single character
  */

typedef enum
{
	IP_VERSION_4 = 4,
	IP_VERSION_6 = 6
}ipversion_t;

/** \typedef connection_status_t
      \brief indicates connection statuses
  */
typedef enum 
{
	CONNECTION_STATUS_DOWN = 0,
	CONNECTION_STATUS_UP = 1
}connection_status_t;

/** \typedef connection_permission_t
      \brief indicates permission statuses
  */
typedef enum 
{
	CONNECTION_PERMISSION_READ = 1,
	CONNECTION_PERMISSION_WRITE = 2
}connection_permission_t;


/** \typedef ipv6_addr_t
      \brief describes an ipv6 address
  */
typedef union ipv6_addr_union_t
{
	uint32_t    value[4];
	uint8_t     bytes[16];
}ipv6_addr_t;

/** \typedef ipv4_addr_t
      \brief describes an ipv4 address
  */
typedef union ipv4_addr_union_t
{
	uint32_t    value;
	uint8_t     bytes[4];
}ipv4_addr_t;


/** \typedef ipaddr_t
      \brief indicates an ip address, which can be IPv4 or IPv6
  */
typedef union ipaddr_union_t
{
	ipv4_addr_t   ipv4;
	ipv6_addr_t   ipv6;
}ipaddr_t;



/** \typedef ipv6_header_t
      \brief describes an ipv6 header
  */
typedef struct ipv6_packet_header_struct_t
{
	uint32_t      traffic_class1 : 4;
	uint32_t      version        : 4;
	uint32_t      traffic_class2 : 4;
	uint32_t      flow_label     : 20; //<- 32
	uint32_t      payload_length : 16;
	uint32_t      next_header    : 8;
	uint32_t      hop_limit      : 8;  //<- 64
	ipv6_addr_t   src_addr;      //128 //<- 192
	ipv6_addr_t   dst_addr;      //128 //<- 320
}ipv6_packet_header_t;


/** \typedef ipv4_header_t
      \brief describes an ipv4 header in network ordered byte format
  */
typedef struct ipv4_packet_header_struct_t
{
	uint32_t       IHL                 : 4;
	uint32_t       version             : 4;
	uint32_t       ECN                 : 2;
	uint32_t       DSCP                : 6;
	uint32_t       total_length        : 16; //<-32
	uint32_t       identification      : 16;
	uint32_t       fragment_offset1    : 5;
	uint32_t       flags               : 3;
	uint32_t       fragment_offset2    : 8;// <-64
	uint32_t       time_to_live        : 8;
	uint32_t       protocol            : 8;
	uint32_t       header_checksum     : 16; //<- 96
	ipv4_addr_t    src_addr;          //32  //<- 128
	ipv4_addr_t    dst_addr;          //32  //<- 160
}ipv4_packet_header_t;


typedef union ip_packet_headunion_header_t
{
	struct{
		uint8_t                 first_nibble : 4;
		uint8_t                 version  : 4;
	};
	ipv4_packet_header_t        ipv4;
	ipv6_packet_header_t        ipv6;
}ip_packet_header_t;


/** \typedef callback_t
      \brief describes a callback used by the actions for calling routines
  */
typedef struct callback_struct_t{
	void		   (*simple)();                ///< pointing to a simple void func();
	void		   (*parameterized)(void*);    ///< pointing to a parameterized void func(void*);
	void			*parameter;                ///< pointing to the parameter of the parameterized function.
}callback_t;

/** \typedef rcallback_t
      \brief describes a callback used by the actions for calling routines
  */
typedef struct rcallback_struct_t{
	void*		   (*simple)();                ///< pointing to a simple void func();
	void*		   (*parameterized)(void*);    ///< pointing to a parameterized void func(void*);
	void*			parameter;                 ///< pointing to the parameter of the parameterized function.
	bool_t          disposable;
}rcallback_t;

/** \typedef command_state_t
      \brief describes the states a command
  */
typedef enum{
	COMMAND_STATE_CREATED = 0,            ///< Indicates that the command is created
	COMMAND_STATE_ISSUED = 1,             ///< Indicates that the command is fullfilled with actions
	COMMAND_STATE_PUFFERED = 2,           ///< Indicates that the command is buffered for executions
	COMMAND_STATE_EXECUTEABLE = 3,        ///< Indicating weather the command is executeable or not.
	COMMAND_STATE_EXECUTED = 4,           ///< Indicates that the command is executed
	COMMAND_STATE_TRASHED = 5,            ///< Indicates that the command is dropped to the trash for destruction.
}command_state_t;


typedef struct diagmsg_struct_t
{
	byte_t     src[16];
}diagmsg_t;

/** \typedef string_t
      \brief A 256 maximal length character string
  */
typedef struct string_struct_t{
	char_t		text[256];       ///< The text a string store
	int32_t     length;          ///< The length of the string
}string_t;

/** \typedef lstring_t
      \brief A 4096 maximal length character string
  */
typedef struct lstring_struct_t
{
	char_t		text[4096];      ///< The text a string store
	int32_t     length;          ///< The length of the string
}lstring_t;

/** \typedef datarray_t
      \brief describes an array of consecutive fixed amount of unspecified data 
  */
typedef struct datarray_struct_t{
	void**		items;              ///< The items belongs to the array
	int32_t     length;             ///< The length of the array
}datarray_t;


/** \typedef datarray_t
      \brief describes a chain of consecutive unspecified data 
  */
typedef struct datchain_struct_t{
	void*                     item;      ///< Pointing to the actual data item
	struct datchain_struct_t* next;      ///< Pointing to the next element of the chain
	struct datchain_struct_t* prev;      ///< Pointing to the previous element of the chain
}datchain_t;

#endif //INCGUARD_NTRT_LIBRARY_DEFINITIONS_H_
