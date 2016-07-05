#include "cmp_groupcounter.h"

#include "lib_defs.h"
#include "lib_descs.h"
#include "inc_texts.h"
#include "inc_predefs.h"
#include "dmap.h"
#include "dmap_sel.h"
#include "cmp_tests.h"

#include "cmp_predefs.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "lib_interrupting.h"
#include "etc_sniffex.h"
#include "lib_tors.h"


static void _cmp_groupcounter_init();
static void _cmp_groupcounter_deinit();


CMP_DEF(, 			      	/*type of definitions*/ 									      \
		 cmp_groupcounter_t,       /*type of component*/ 											      \
		 CMP_NAME_GROUPCOUNTER,    /*name of the component*/ 										      \
		 _cmp_groupcounter,        /*variable name of the component*/  								  \
		 cmp_groupcounter_ctor,    /*name of the constructor function implemented automatically*/       \
		 cmp_groupcounter_dtor,    /*name of the destructor function implemented automatically*/        \
		 _cmp_groupcounter_init,   /*name of the external function called after construction*/          \
		 __NO_TEST_FUNC_,   /*name of the external function called after initialization*/         \
		 _cmp_groupcounter_deinit  /*name of the external function called before destruction*/          \
		);


CMP_DEF_GET_PROC(get_cmp_groupcounter, cmp_groupcounter_t, _cmp_groupcounter);

static void* _groupcounter_restart();
static void* _groupcounter_start();
static void* _groupcounter_stop();
static void _groupcounter_sniff_receiver(sniff_t* sniff, int32_t listener_id);
static void _groupcounter_record_requester(int32_t listener_id, record_t* result);

//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_groupcounter_init()
{
    //constructions
  _cmp_groupcounter->mutex = mutex_ctor();
    //Binding
    CMP_BIND(_cmp_groupcounter->start,            _groupcounter_start);
    CMP_BIND(_cmp_groupcounter->stop,             _groupcounter_stop);
    CMP_BIND(_cmp_groupcounter->restart,          _groupcounter_restart);
    CMP_BIND(_cmp_groupcounter->sniff_receiver,   _groupcounter_sniff_receiver);
    CMP_BIND(_cmp_groupcounter->record_requester, _groupcounter_record_requester);

}

void _cmp_groupcounter_deinit()
{
  mutex_dtor(_cmp_groupcounter->mutex);
}

void* _groupcounter_start()
{
  CMP_DEF_THIS(cmp_groupcounter_t, _cmp_groupcounter);
  pcap_listener_t *pcap_listener;
  groupcounters_holder_t* holder;
  groupcounter_prototype_t *prototype;
  groupcounter_t* groupcounter;
  slist_t* it;
  int32_t index;

  dmap_rdlock_table_pcapls();
  dmap_rdlock_table_groupcounter_protos();
  for(index = 0; dmap_itr_table_pcapls(&index, &pcap_listener) == BOOL_TRUE; ++index){
    it     = pcap_listener->groupcounter_prototypes;
    holder = &this->holders[index];
    for(; it; it = it->next){
      prototype = it->data;
      groupcounter = make_groupcounter(prototype);
      holder->groupcounters = slist_append(holder->groupcounters, groupcounter);
      groupcounter->interface.init(groupcounter, pcap_listener);
    }
  }
  dmap_rdunlock_table_groupcounter_protos();
  dmap_rdunlock_table_pcapls();

  return NULL;
}

void* _groupcounter_stop()
{
  CMP_DEF_THIS(cmp_groupcounter_t, _cmp_groupcounter);
  pcap_listener_t *pcap_listener;
  groupcounters_holder_t* holder;
  int32_t index;

  dmap_rdlock_table_pcapls();
  dmap_rdlock_table_groupcounter_protos();
  for(index = 0; dmap_itr_table_pcapls(&index, &pcap_listener) == BOOL_TRUE; ++index){
    holder = &this->holders[index];
    slist_dtor(holder->groupcounters, groupcounter_dtor);
    holder->groupcounters = NULL;
  }
  dmap_rdunlock_table_groupcounter_protos();
  dmap_rdunlock_table_pcapls();
  return NULL;
}

void* _groupcounter_restart()
{
    _groupcounter_stop();
    _groupcounter_start();
    return NULL;
}
//----------------------------------------------------------------------------------------------------
//---------------------------- Processes --------------------------------------------
//----------------------------------------------------------------------------------------------------

void _groupcounter_sniff_receiver(sniff_t* sniff, int32_t listener_id)
{
  CMP_DEF_THIS(cmp_groupcounter_t, _cmp_groupcounter);
  groupcounters_holder_t*   holder;
  groupcounter_t* groupcounter;
  slist_t* it;
  if(!sniff){
    //TODO: considering impulse
    goto done;
  }

  mutex_lock(this->mutex);

  holder = &this->holders[listener_id];
  for(it = holder->groupcounters; it; it = it->next){
    groupcounter = it->data;
    groupcounter->interface.add_sniff(groupcounter, sniff);
  }

  mutex_unlock(this->mutex);

done:
  return;
}

void _groupcounter_record_requester(int32_t listener_id, record_t* result)
{
  CMP_DEF_THIS(cmp_groupcounter_t, _cmp_groupcounter);
  groupcounters_holder_t*   holder;
  groupcounter_t* groupcounter;
  slist_t* it;
  int32_t i;

  mutex_lock(this->mutex);
  memset(result, 0, sizeof(record_t));

  holder = &this->holders[listener_id];
  result->listener_id = listener_id;
  set_mtime(&result->timestamp);

  for(i = 0, it = holder->groupcounters; it; it = it->next, ++i){
    groupcounter = it->data;
    result->items[i] = groupcounter->interface.get_counter(groupcounter);
  }

  mutex_unlock(this->mutex);

}

