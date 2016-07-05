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

#define NTRT_MAX_MAPPED_VARS 32
#define NTRT_MAX_FEATURES_NUM 32
#define NTRT_MAX_GROUPCOUNTERS_NUM 32
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
  uint32_t     accumulation_time;
  uint32_t     pcap_listeners_num;
  uint32_t     sniff_size;
}sysdat_t;

typedef struct pcap_listener_struct_t{
  uint32_t           feature_num;
  uint32_t           accumulation_length;
  char_t             device[255];
  char_t             output[255];
  uint32_t           puffer_size;
  int32_t            append_output;
  char_t             pcap_filter[1024];
  pcap_t*            handler;
//  datchain_t*        features;
  uint32_t           mapped_vars_num;
  uint32_t           mapped_var_ids[NTRT_MAX_MAPPED_VARS];

  uint32_t           groupcounter_num;

  slist_t*           evaluators;
  slist_t*           groupcounter_prototypes;
  bpf_u_int32        mask;               /* Our netmask */
  bpf_u_int32        net;                /* Our IP */
  struct bpf_program fp;                 /* The compiled filter */
}pcap_listener_t;

typedef struct record_struct_t{
  uint32_t    items[NTRT_MAX_FEATURES_NUM];
  int32_t     listener_id;
  mtime_t     timestamp;
}record_t;

typedef enum
{
        EXTENDED_BOOL_UNDEFINED = -1,
        EXTENDED_BOOL_FALSE = 0,  ///< Indicates a boolean false value
        EXTENDED_BOOL_TRUE = 1    ///< Indicates a boolean true value
}extended_bool_t;

typedef struct mapped_var_struct_t{
  int32_t   identifier;
  uint32_t  value;
}mapped_var_t;

typedef struct sniff_struct_t{
  const u_char*       packet;
  struct pcap_pkthdr* header;
  extended_bool_t     is_ip_packet;
  extended_bool_t     is_udp_packet;
  extended_bool_t     is_tcp_packet;
  struct  in_addr     ip_src,ip_dst;
  u_char              ip_p;                  /* protocol */
  u_short             ip_len;
  int32_t             size_ip;
  u_short             port_src;               /* source port */
  u_short             port_dst;               /* destination port */
  int32_t             size_payload;
  u_char*             payload;
  int32_t             listener_id;
  struct{
    extended_bool_t analyzed;
    u_char          payload_type;
    u_short         seq_num;
  }rtp;
}sniff_t;

typedef struct evaluator_container_struct_t{
  int32_t port_num;
  int32_t payload_type;
  u_short seq_num;
  bool_t  seq_num_initialized;
}evaluator_container_t;

typedef struct feature_struct_t{
  char_t                     name[255];
  char_t                     identifier[128];
  uint32_t                 (*evaluator)(sniff_t*,evaluator_container_t*);
}feature_t;


typedef struct groupcounter_interface_struct_t groupcounter_interface_t;
typedef struct groupcounter_struct_t groupcounter_t;

struct groupcounter_interface_struct_t{
  void     (*init)(groupcounter_t*, ptr_t);
  void     (*add_sniff)(groupcounter_t*,sniff_t*);
  int32_t  (*get_counter)(groupcounter_t*);
  void     (*deinit)(groupcounter_t*);
};

struct groupcounter_struct_t{
  groupcounter_interface_t interface;
  slist_t*                 flows;
  int32_t                  flowcounts;
  int32_t                  timeout_treshold;
};

typedef struct groupcounter_prototype_struct_t{
  char_t                   identifier[128];
  groupcounter_interface_t interface;
//  void     (*collection_data_dtor)(ptr_t);
//  void     (*push_cb)(sniff_t*,groupcounter_item_t*);
//  void     (*pop_cb)(groupcounter_item_t*, ptr_t);
}groupcounter_prototype_t;



typedef struct evaluation_item_struct_t{
  feature_t            *feature;
  evaluator_container_t evaluator_container;
}evaluator_item_t;

typedef struct devlegoio_struct_t
{
	//tunnel functionalities
	//udp socket functions
	//print functionalities
	void     	 (*print_stdout)(const char*,...);
	void     	 (*print_stdlog)(const char*,...);

	int32_t          (*opcall)(const char*);
	char_t           config_file[255];
	char_t           command_file[255];
	bool_t           command_file_initialized;
//	char_t           bash_dir[255];
	//
}devlegoio_t;

devlegoio_t *sysio;

#endif //INCGUARD_NTRT_LIBRARY_DESCRIPTIONS_H_
