/**
 * @file lib_descs.h
 * @brief struct descriptions used by the mpt program
 * @author Bal�zs, Kreith; Debrecen, Hungary
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @date 2014.02.11
*/
#ifndef INCGUARD_NTRT_LIBRARY_DESCRIPTIONS_H_
#define INCGUARD_NTRT_LIBRARY_DESCRIPTIONS_H_

#include <time.h>
#include "lib_defs.h"
#include "lib_threading.h"
#include <signal.h>

#define NTRT_MAX_PATH_NUM 10
#define NTRT_MAX_NETWORK_NUM 10
#define NTRT_MAX_TUNNEL_NUM 2
#define NTRT_MAX_CONNECTION_NUM 2
#define NTRT_MAX_INTERFACE_NUM 10
#define NTRT_MAX_THREAD_NUM 32

#define NTRT_MAX_SCHCAT_NUM 4
#define NTRT_SCHEDULER_CLOCK_BASE 16
#define NTRT_TICK_PER_MS_RATIO 1.024
#define NTRT_MS_PER_TICK_RATIO 0.9765625

#define NTRT_SCHEDULER_SIGNAL SIGRTMIN
#define NTRT_PSENDER_SIGNAL SIGRTMIN + 1

#define NTRT_PACKET_LENGTH 2048
#define NTRT_MTU_SIZE 1500

/**
 * @defgroup CMDValues CMD values
 * @brief CMD values starts from 0xA0
 * @{
 */
#define NTRT_REQ_CMD_KEEPALIVE           0xA1    ///< Keepalive packet
#define NTRT_REQ_CMD_ECHO                0xA2    ///< ECHO
#define NTRT_REQ_CMD_P_STATUS_CHANGE     0xA3    ///< Path status change request
#define NTRT_REQ_CMD_CONNECTION_SEND     0xA4    ///< Connection update request
#define NTRT_REQ_CMD_NETWORK_SEND        0xA5    ///< Connection update request
#define NTRT_REQ_CMD_PATH_SEND           0xA6    ///< Connection update request

/// @}

/**
 * @defgroup StatusValues Status values
 * @brief Status values of connections and pathes
 * @{
 */
#define NTRT_REQ_STAT_OK                 0x00    ///< Status OK
#define NTRT_REQ_STAT_PATH_DOWN          0x80    ///< Path is down
#define NTRT_REQ_STAT_IF_DOWN            0x81    ///< Interface is down
#define NTRT_REQ_STAT_ADDRESS_DOWN       0x82    ///< Address is down
#define NTRT_REQ_STAT_DISABLED           0xFF    ///< Disabled
/// @}


#define NTRT_CONNECTION_STRUCT_DEFAULT_NAME "connection"
#define NTRT_CONNECTION_STRUCT_DEFAULT_IP_VERSION 6
#define NTRT_CONNECTION_STRUCT_DEFAULT_SOURCE_IP "127.0.0.1"
#define NTRT_CONNECTION_STRUCT_DEFAULT_DEST_IP "127.0.0.1"
#define NTRT_CONNECTION_STRUCT_DEFAULT_DATA_REMOTE_PORT 65060
#define NTRT_CONNECTION_STRUCT_DEFAULT_DATA_LOCAL_PORT 65060
#define NTRT_CONNECTION_STRUCT_DEFAULT_PERMISSION 0
#define NTRT_CONNECTION_STRUCT_DEFAULT_CMD_PORT 65050

#define NTRT_CONNECTION_STRUCT_DEFAULT_DEADTIME 20
#define NTRT_CONNECTION_STRUCT_DEFAULT_KEEPALIVE 5
#define NTRT_CONNECTION_STRUCT_DEFAULT_STATUS 0
#define NTRT_CONNECTION_STRUCT_DEFAULT_SCHEDULING_TYPE "windowed"
#define NTRT_CONNECTION_STRUCT_DEFAULT_AUTH_KEY 0
#define NTRT_CONNECTION_STRUCT_DEFAULT_AUTH_TYPE 0

typedef struct eveter_arg_struct_t
{
	int32_t      event;
	void*        arg;
}eventer_arg_t;

/// Data structure of tunnel interface
typedef struct tunnel_struct_t
{
    char_t      if_name[128];   ///< The name of the tunnel interface e.g. tun1
    char_t      device[128];    ///< The name of the tunnel device
    byte_t      ip4[4];         ///< The IPv4 address of the tunnel
	uint16_t    ip4len;         ///< The prefix length of the IPv4 address
	byte_t      ip6[16];        ///< The IPv6 address of the tunnel
    uint16_t    ip6len;         ///< The pref.length of the IPv6 address
    uint16_t    cmd_port_rcv;   ///< The port number for multipath commands to accept
    uint16_t    cmd_port_snd;   ///< Port number to send multipath commands
    int32_t     fd;
    int32_t     sockd;

    uint32_t    playout_cut_size;
    int32_t     playout_max_time;
} tunnel_t;


/// Data structure of physical interface
typedef struct interface_struct_t
{
    char_t    name[128];        ///< The name of the interface
    uint32_t  ip4;              ///< The IPv4 address
    uint8_t   ip4len;           ///< The IPv4 prefix length
    uint32_t  ip6[4];           ///< The IPv6 address
    uint8_t   ip6len;           ///< The IPv6 prefix length
    uint32_t  mac_local[2];     ///< The MAC address
    uint32_t  ip4_gw;           ///< The IPv4 address of the gateway
    uint32_t  ip6_gw[4];        ///< The IPv6 address of the gateway
    uint32_t  mac_gw[2];        ///< The MAC address of the gateway
} interface_t;

typedef union path_status_union_t
{
	struct
	{
		uint8_t if_down   : 1;
		uint8_t addr_down : 1;
		uint8_t not_used  : 5;
		uint8_t path_down : 1;
	};
	uint8_t value;
}path_status_t;

/// Data structure of a single path in a connection
typedef struct path_struct_t
{
    uint32_t        ip_local[4];    ///< The local IP address of the path, can be v4 or v6
    uint32_t        ip_remote[4];   ///< The remote IP address of the path, can be v4 or v6
    uint32_t        ip_gw[4];       ///< The IP address of the outgoing gateway, can be v4 or v6
    uint32_t        mac_local[2];   ///< The MAC address of the local interface ( for EUI-64 too)
    uint32_t        mac_gw[2];      ///< The MAC address of the outgoing gateway (for EUI-64)
    uint32_t        packet_max;     ///< The maximum number of packets to send continously on the path
    uint32_t        header[32];     ///< buffer to hold the header (Eth, IP, UDP) data for raw socket
    uint16_t        weight_in;      ///< The weight value of the incoming traffic
    uint16_t        weight_out;     ///< The weight value of the outgoing traffic
    uint8_t         status;         ///< The status of the path
    uint8_t         ip_version;     ///< The IP version of the path

    uint32_t        bandwidth;
    uint32_t        latency;
   // time_t    last_keepalive; ///< Time of last keepalive received
    char_t          interface_name[128];      ///< The name of the local physical interface
	int32_t         con_dmap_id;  ///< dmap_id used for linking the path to a connection
} path_t;

/// Data struct for a network connection
typedef struct network_struct_t
{
  int32_t  version;
  uint32_t source[4];
  uint32_t source_prefix_length;
  uint32_t destination[4];
  uint32_t destination_prefix_length;
  int32_t  con_dmap_id;    ///< dmap_id used for linking the path to a connection
} network_t;

#define NTRT_SCHEDULING_TYPES_NUM 2
typedef enum
{
	PACKET_SCHEDULING_WINDOWED    = 0,
	PACKET_SCHEDULING_BALANCED    = 1,
}packet_scheduling_t;

/// Data structure of a connection
typedef struct connection_struct_t
{
    char_t     name[128];   ///< The name of the connection
    char_t     filename[128];    ///< The configuraton file of the connection
    uint32_t   ip_local[4]; ///< The local  IP address of the tunnel can be v4 or v6
    uint32_t   ip_remote[4];///< The remote IP address of the tunnel can be v4 or v6
    uint16_t   port_local;  ///< The UDP local endpoint id (port number)
    uint16_t   port_remote; ///< The UDP remote endpoint id (port number)
    uint16_t   cmd_port_remote; ///< The cmd remote UDP port number
    uint32_t   path_packet; ///< The number of packets, sent on the actual path of the connection
    uint64_t   conn_packet; ///< The number of packets sent on the connection
    uint16_t   path_count;  ///< The number of the existing paths related to this connection
    uint16_t   network_count; ///< The number of defined networks related to this connection
    uint16_t   path_index;  ///< The index of the actual path in the mpath array
    uint8_t    status;      ///< The status of the connection
    uint8_t    permission;  ///< Allow connection updates (1: send bit, 2: receive bit)
    uint8_t    ip_version;  ///< The IP version of the connection (4 or 6)
    uint8_t    keepalive;   ///< Keepalive message interval in secs
    uint16_t   deadtimer;   ///< Keepalive timeout value in secs
    uint32_t   waitrand;    ///< Security token used at communication
    uint8_t    waitround;   ///< Client phase in communication
    char_t     waithash[32]; ///< Expected checksum for receiving data (SHA-256 is 32 byte long)
    uint8_t    auth_type;   ///< The authentication code for the connection
    char_t     auth_key[128];///< The key value of the authentication
    //shandler_t shandler;
    int32_t    sockd;
    packet_scheduling_t scheduling_type;
    //int32_t   path_ids[NTRT_MAX_PATH_NUM]; ///< The paths asociated to the connection
    //int32_t   network_ids[NTRT_MAX_NETWORK_NUM]; ///< The networks connected by the connection

} connection_t; //sum: 484 byte!

typedef struct request_struct_t
{
	byte_t            message[NTRT_PACKET_LENGTH];
	int32_t           size;
	byte_t            command;
	byte_t            status;
	connection_t*     connection;
}request_t;


typedef struct array_struct_t
{
	void *items;
	int32_t length;
}array_t;

/** \typedef mptsystem_t
      \brief describes a system used global variables used by the program
  */
typedef struct sysdat_struct_t
{
	byte_t        mp_global_server;         ///< If 0, then only the live connections are allowed to send cmd
	tunnel_t     *mp_global_tun;
	connection_t *mp_global_con;
}sysdat_t;


/** \typedef command_t
      \brief describes a command
  */
typedef struct command_struct_t
{
	char_t                    name[128];
	int32_t                   id;
	rcallback_t              *action;
	callback_t               *callback;
	signal_t                 *signal;
	time_t                    created;
	volatile command_state_t  state;
	request_t                *request;
	struct command_struct_t  *next_command;
}command_t;

/** \typedef cmdres_t
      \brief describes a returns to a command
  */
typedef struct cmdres_struct_t
{
	command_t*      command;
	request_t*      request;
}cmdres_t;

#ifndef NDEBUG
typedef enum
{
	PACKET_STATE_CONSTRUCTED      = 0,
	PACKET_STATE_RECYCLE_RECEIVED = 1,
	PACKET_STATE_RECYCLE_SUPPLIED = 2,
	PACKET_STATE_SENDING          = 3
}packet_state_t;

#endif

/** \typedef packet_t
      \brief describes a packet used by the program for sending, or receiving
  */
typedef struct packet_struct_t{
	byte_t        		   *bytes;                  ///< The concrete packet bytes
	ip_packet_header_t     *header;			 	    ///< Host ordered header.
	uint16_t                length;                 ///< The length of packet bytes
	connection_t           *connection;                 ///< Poinitng to the connection the packet will use as an output
	int32_t                 con_id;
	uint32_t                source[4];				///< Contains the source address the packet come from (only if it is received)
	int32_t                 path_out;               ///< Poinitng to the dmap_index of the path the packet will be transceived
	bool_t                  local;
	uint32_t                seq_num;
#ifndef NDEBUG
	mtime_t                mtime;
	packet_state_t         state;
#endif
	int32_t                park_delay;
	int32_t                park_cat;
} packet_t;

typedef struct pathring_struct_t
{
	path_t                    *actual;
	struct pathring_struct_t  *next;
	int32_t                    counter;
	int32_t                    max;
	int32_t                    path_id;
}pathring_t;


typedef struct devlegoio_struct_t
{
	//tunnel functionalities
	int32_t          (*tunnel_start)(tunnel_t*);
	void             (*tunnel_stop)(tunnel_t*);
	int32_t          (*tunnel_read)(int32_t, void*, size_t);
	void             (*tunnel_write)(int32_t, void*, size_t);
	//udp socket functions
	int32_t          (*udpsock_open_and_bind)(int32_t);
	int32_t          (*udpsock_send)(int32_t, const char*, size_t, int32_t, struct sockaddr*, socklen_t);
	int32_t          (*udpsock_recv)(int32_t, char_t*, size_t, int32_t, struct sockaddr*, socklen_t*);
	int32_t          (*udpsock_open)(int32_t, int32_t, int32_t);
	void             (*udpsock_close)(int32_t);
	//print functionalities
	void     		 (*print_stdout)(const char*,...);
	void     		 (*print_stdlog)(const char*,...);

	void             (*command_cli)(const char*,...);
	int32_t          (*opcall)(const char*);
	char_t           config_dir[255];
	char_t           bash_dir[255];
	//
}devlegoio_t;

devlegoio_t *sysio;

#endif //INCGUARD_NTRT_LIBRARY_DESCRIPTIONS_H_
