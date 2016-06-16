#ifndef INCGUARD_NTRT_COMPONENT_SNIFFER_H_
#define INCGUARD_NTRT_COMPONENT_SNIFFER_H_
#include "cmp_predefs.h"
#include "lib_descs.h"

//--------------------------------------------------
//precver components begin (cmp_precver)
//----------------------------------------------------
#define CMP_NAME_SNIFFER "PCAP Sniffer component"
typedef struct cmp_sniffer_struct_t
{
	void      (*send)(sniff_t*);
	void*     (*start)(pcap_listener_t*);
	void*     (*stop)(pcap_listener_t*);
}cmp_sniffer_t;
//--------------------------------------------------
//precver components end (cmp_precver)
//----------------------------------------------------

void cmp_sniffer_ctor();
void cmp_sniffer_dtor();
cmp_sniffer_t* get_cmp_sniffer();



#endif //INCGUARD_NTRT_COMPONENT_SNIFFER_H_
