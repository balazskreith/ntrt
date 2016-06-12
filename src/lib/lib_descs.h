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
#include <pcap.h>

#define NTRT_MAX_FEATURES_NUM 32
#define NTRT_MAX_THREAD_NUM 32
#define NTRT_MAX_PCAPLS_NUM 32

#define NTRT_TICK_PER_MS_RATIO 1.024
#define NTRT_MS_PER_TICK_RATIO 0.9765625


typedef struct eveter_arg_struct_t
{
	int32_t      event;
	void*        arg;
}eventer_arg_t;


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
  uint32_t     sampling_rate;
  uint32_t     pcap_listeners_num;
  uint32_t     sniff_size;
}sysdat_t;

typedef struct pcap_listener_struct_t{
  uint32_t           feature_num;
  char_t             device[255];
  char_t             output[255];
  int32_t            append_output;
  char_t             pcap_filter[1024];
  pcap_t*            handler;
  datchain_t*        features;
  bpf_u_int32        mask;               /* Our netmask */
  bpf_u_int32        net;                /* Our IP */
  struct bpf_program fp;                 /* The compiled filter */
  uint32_t           values[NTRT_MAX_FEATURES_NUM];
}pcap_listener_t;

typedef struct record_struct_t{
  uint32_t    items[32];
  uint32_t    length;
}record_t;

typedef struct sniff_struct_t{
  const u_char*       packet;
  struct pcap_pkthdr* header;
}sniff_t;

typedef struct feature_struct_t{
  char_t     name[255];
  char_t     identifier[128];
  uint32_t (*evaluator)(sniff_t*,ptr_t);
  ptr_t      evaluator_data;
}feature_t;

typedef struct devlegoio_struct_t
{
	//tunnel functionalities
	//udp socket functions
	int32_t          (*udpsock_open_and_bind)(int32_t);
	int32_t          (*udpsock_send)(int32_t, const char*, size_t, int32_t, struct sockaddr*, socklen_t);
	int32_t          (*udpsock_recv)(int32_t, char_t*, size_t, int32_t, struct sockaddr*, socklen_t*);
	int32_t          (*udpsock_open)(int32_t, int32_t, int32_t);
	void             (*udpsock_close)(int32_t);
	//print functionalities
	void     	 (*print_stdout)(const char*,...);
	void     	 (*print_stdlog)(const char*,...);

	void             (*command_cli)(const char*,...);
	int32_t          (*opcall)(const char*);
	char_t           config_dir[255];
//	char_t           bash_dir[255];
	//
}devlegoio_t;

devlegoio_t *sysio;

#endif //INCGUARD_NTRT_LIBRARY_DESCRIPTIONS_H_
