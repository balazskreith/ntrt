#include "cmp_sniffer.h"

#include "dmap_sel.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "cmp_predefs.h"
#include "lib_descs.h"
#include "lib_puffers.h"
#include "lib_threading.h"
#include "lib_tors.h"
#include "lib_makers.h"
#include <time.h>
#include <stdlib.h>
#include "inc_mtime.h"
#include <stdio.h>
#include "inc_unistd.h"
#include "inc_texts.h"
#include "inc_inet.h"
#include "inc_predefs.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "inc_io.h"
#include "lib_funcs.h"
#include "etc_utils.h"
#include "cmp_defs.h"
#include "etc_endian.h"
#include "etc_sniffex.h"

//----------------------------------------------------------------------------------------------------
//---------------------------- Private type declarations ---------------------------------------------
//----------------------------------------------------------------------------------------------------

#define CMP_NAME_PCAPLISTENER "PCAP Listener component"
typedef struct _cmp_listener_struct_t
{
	int32_t              sockd;
	pcap_listener_t*     pcap_listener;
	int32_t              pcap_listener_id;
	void               (*send)(sniff_t*); //send to the packet router
}_cmp_listener_t;


//----------------------------------------------------------------------------------------------------
//---------------------------- Public definitions ----------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void  _cmp_sniffer_init();
static void  _cmp_sniffer_deinit();

CMP_DEF(, 			      		/*type of definitions*/ 										   \
		cmp_sniffer_t,       /*type of component*/ 										  	   \
		 CMP_NAME_SNIFFER, /*name of the component*/ 										   \
		 _cmp_sniffer,        /*variable name of the component*/  							   \
		 cmp_sniffer_ctor,    /*name of the constructor function implemented automatically*/     \
		 cmp_sniffer_dtor,    /*name of the destructor function implemented automatically*/      \
		 _cmp_sniffer_init,   /*name of the external function called after construction*/        \
		 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
		 _cmp_sniffer_deinit  /*name of the external function called before destruction*/        \
		);																						   \

CMP_DEF_GET_PROC(get_cmp_sniffer, cmp_sniffer_t, _cmp_sniffer);

//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

static void _thr_listener_init(thread_t*);
static void _thr_listener_proc_main(thread_t*);
static void _cmp_listener_deinit(_cmp_listener_t*);
CMP_THREADS(static,          //type of declarations
		_cmp_listener_t,  //type of components
		conlistener,		 //unique name used for identifying generated variables and processes
		CMP_NAME_PCAPLISTENER,//name of the component running in the thread
		_listeners_ctor,  //name of the constructor process used for creating a thread chain
		_listeners_dtor,  //name of the destructor process used for disposing a thread chain
		_itr_conlisteners,   //name of the iterator process
		_thr_listener_init,  //name of the process initialize a thread
		__CMP_NO_INIT_MFUNC__,   //name of the process initialize a component
		__THR_NO_DEINIT_PROC__, //name of the process deinitialize a thread
		_cmp_listener_deinit,//name of the process deinitialize a component
		_thr_listener_proc_main, //name of the thread process called after initialization
		pcap_listener_t,               //name of the type used for activating different thread
		pcap_listener,                 //name of the attribute inside the component used for pointing to the activator type
		_conlistener_start,         //name of the process used creating and activating a component
		_conlistener_stop           //name of the process used for deactivating and disposing a threaded component
		)


static void _reset_sniff(sniff_t* sniff);

//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------


void  _cmp_sniffer_init()
{
	int32_t   index, count;

	//constructions
	_listeners_ctor();

	//Binding
	CMP_BIND(_cmp_sniffer->start,        _conlistener_start);
	CMP_BIND(_cmp_sniffer->stop,         _conlistener_stop);

	//dmap reservation

}

void  _cmp_sniffer_deinit()
{
	_listeners_dtor();
}
void _thr_listener_init(thread_t *thread)
{
    _cmp_listener_t     *cmp = (_cmp_listener_t*) thread->arg;
    pcap_listener_t     *pcap_listener = cmp->pcap_listener;
    char_t               errbuf[PCAP_ERRBUF_SIZE];      /* Error string */
    int32_t              sampling_rate;
    int32_t              promisc;

    //connect
    CMP_CONNECT(cmp->send, _cmp_sniffer->send);

    dmap_lock_sysdat();
    sampling_rate = dmap_get_sysdat()->sampling_rate;
//    sampling_rate = 0;
    dmap_unlock_sysdat();

    //Todo: Below!
    //promisc       = dmap_get_sysdat()->promisc;
    promisc = 0;
    dmap_rdlock_table_pcapls();
    cmp->pcap_listener_id = dmap_get_id_by_pcapls(pcap_listener);
    /* get network number and mask associated with capture device */
    if (pcap_lookupnet(pcap_listener->device, &pcap_listener->net, &pcap_listener->mask, errbuf) == -1) {
            ERRORPRINT("Couldn't get netmask for device %s: %s\n", pcap_listener->device, errbuf);
            pcap_listener->net = 0;
            pcap_listener->mask = 0;
            fsm_kill();
            goto done;
    }
    /* print capture info */
    INFOPRINT("Pcap filter is set to device: %s", pcap_listener->device);

    /* open capture device */
    pcap_listener->handler = pcap_open_live(pcap_listener->device, SNAP_LEN, promisc, sampling_rate, errbuf);
    if (pcap_listener->handler == NULL) {
            ERRORPRINT("Couldn't open device %s: %s\n", pcap_listener->device, errbuf);
            fsm_kill();
            goto done;
    }

    /* make sure we're capturing on an Ethernet device [2] */
    if (pcap_datalink(pcap_listener->handler) != DLT_EN10MB) {
            ERRORPRINT("%s is not an Ethernet\n", pcap_listener->device);
            fsm_kill();
            goto done;
    }

    if(strlen(pcap_listener->pcap_filter)){
        INFOPRINT("Filter expression: %s\n", pcap_listener->pcap_filter);
        /* compile the filter expression */
        if (pcap_compile(pcap_listener->handler,
                         &pcap_listener->fp,
                         pcap_listener->pcap_filter,
                         0,
                         pcap_listener->net) == -1) {
                ERRORPRINT("Couldn't parse filter %s: %s\n",
                        pcap_listener->pcap_filter, pcap_geterr(pcap_listener->handler));
                fsm_kill();
                goto done;
        }

        /* apply the compiled filter */
        if (pcap_setfilter(pcap_listener->handler, &pcap_listener->fp) == -1) {
                ERRORPRINT("Couldn't install filter %s: %s\n",
                        pcap_listener->pcap_filter, pcap_geterr(pcap_listener->handler));
                fsm_kill();
                goto done;
        }
    }

done:
  dmap_rdunlock_table_pcapls();

}

void _cmp_listener_deinit(_cmp_listener_t* cmp)
{
  pcap_listener_t     *pcap_listener = cmp->pcap_listener;
  /* cleanup */
  if(strlen(pcap_listener->pcap_filter)){
    pcap_freecode(&pcap_listener->fp);
  }
  pcap_close(pcap_listener->handler);
}


//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _thr_listener_proc_main(thread_t *thread)
{
	CMP_DEF_THIS(_cmp_listener_t, (_cmp_listener_t*) thread->arg);
	pcap_listener_t    *pcap_listener = this->pcap_listener;
        struct pcap_pkthdr  header;      /* The header that pcap gives us */
        const u_char       *packet;           /* The actual packet */
        sniff_t            sniff;

	do{
          packet = pcap_next(pcap_listener->handler, &header);
          _reset_sniff(&sniff);
          //setup sniff
          sniff.packet = packet;
          sniff.header = &header;
          sniff.listener_id = this->pcap_listener_id;

          this->send(&sniff);
	}while(thread->state == THREAD_STATE_RUN);
}

void _reset_sniff(sniff_t* sniff)
{
  memset(sniff,0, sizeof(sniff_t));
  sniff->is_ip_packet  = EXTENDED_BOOL_UNDEFINED;
  sniff->is_udp_packet = EXTENDED_BOOL_UNDEFINED;
  sniff->is_tcp_packet = EXTENDED_BOOL_UNDEFINED;
  sniff->rtp.analyzed  = EXTENDED_BOOL_UNDEFINED;
}


