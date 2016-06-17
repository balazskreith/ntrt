#include "cmp_evaluator.h"

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


static void _cmp_evaluator_init();
static void _cmp_evaluator_deinit();


CMP_DEF(, 			      	/*type of definitions*/ 									      \
		 cmp_evaluator_t,       /*type of component*/ 											      \
		 CMP_NAME_EVALUATOR,    /*name of the component*/ 										      \
		 _cmp_evaluator,        /*variable name of the component*/  								  \
		 cmp_evaluator_ctor,    /*name of the constructor function implemented automatically*/       \
		 cmp_evaluator_dtor,    /*name of the destructor function implemented automatically*/        \
		 _cmp_evaluator_init,   /*name of the external function called after construction*/          \
		 __NO_TEST_FUNC_,   /*name of the external function called after initialization*/         \
		 _cmp_evaluator_deinit  /*name of the external function called before destruction*/          \
		);

static void clean_record(record_t* record);
#define CMP_PUFFER_RECYCLE_SIZE 2048
#define CMP_NAME_RRECYCLE "Records Recycle component"
CMP_DEF_RECPUFFER(
                static,                         /*type of the declarations*/
                record_t,                       /*name of the type of the items*/
                record_ctor,            /*name of the process used for constructing an item if the puffer is empty.*/
                record_dtor,    /*name of the process used for deconstruting an item if the puffer is full.*/
                clean_record,           /*name of the process clear the item when the recycle receives it*/
                CMP_NAME_RRECYCLE,      /*name of the recycle puffer*/
                _cmp_recycle,           /*name of the variable used for referencing to the recycle*/
                CMP_PUFFER_RECYCLE_SIZE, /*length of the recycle puffer*/
                _cmp_recycle_ctor,      /*name of the constructor process for this recycle*/
                _cmp_recycle_dtor       /*name of the destructor process for this recycle*/
                );


CMP_DEF_GET_PROC(get_cmp_evaluator, cmp_evaluator_t, _cmp_evaluator);

static void* _evaluator_restart();
static void* _evaluator_start();
static void* _evaluator_stop();
static void _evaluator_process(sniff_t* sniff);
static double sampling_rate_;
//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_evaluator_init()
{
    //constructions
    _cmp_recycle_ctor();

    //Binding
    CMP_BIND(_cmp_evaluator->start,           _evaluator_start);
    CMP_BIND(_cmp_evaluator->stop,            _evaluator_stop);
    CMP_BIND(_cmp_evaluator->restart,         _evaluator_restart);
    CMP_BIND(_cmp_evaluator->sniff_receiver,  _evaluator_process);

    //Connecting
    CMP_CONNECT(_cmp_evaluator->record_receiver, _cmp_recycle->receiver);
    CMP_CONNECT(_cmp_evaluator->demand_record,   _cmp_recycle->supplier);

    //data

}

void _cmp_evaluator_deinit()
{
  _cmp_recycle_dtor();
}

void* _evaluator_start()
{
  CMP_DEF_THIS(cmp_evaluator_t, _cmp_evaluator);
  pcap_listener_t *pcap_listener;
  cmp_evaluator_item_t *cmp_evaluator_item;
  int32_t index;

  dmap_rdlock_table_pcapls();
  for(index = 0; dmap_itr_table_pcapls(&index, &pcap_listener) == BOOL_TRUE; ++index){
    cmp_evaluator_item = &this->evaluators[index];
    cmp_evaluator_item->evaluators = slist_cpy(pcap_listener->evaluators);
    cmp_evaluator_item->feature_num = pcap_listener->feature_num;
  }
  dmap_rdunlock_table_pcapls();

  dmap_lock_sysdat();
  sampling_rate_ = dmap_get_sysdat()->sampling_rate;
  dmap_unlock_sysdat();
  return NULL;
}

void* _evaluator_stop()
{
  CMP_DEF_THIS(cmp_evaluator_t, _cmp_evaluator);
  pcap_listener_t *pcap_listener;
  cmp_evaluator_item_t *cmp_evaluator_item;
  int32_t index;

  dmap_rdlock_table_pcapls();
  for(index = 0; dmap_itr_table_pcapls(&index, &pcap_listener) == BOOL_TRUE; ++index){
    cmp_evaluator_item = &this->evaluators[index];
    //TODO: free the copied slist:         cmp_evaluator_item->evaluators
  }
  dmap_rdunlock_table_pcapls();
  return NULL;
}

void* _evaluator_restart()
{
    _evaluator_stop();
    _evaluator_start();
    return NULL;
}
//----------------------------------------------------------------------------------------------------
//---------------------------- Processes --------------------------------------------
//----------------------------------------------------------------------------------------------------

void _evaluator_process(sniff_t* sniff)
{
  CMP_DEF_THIS(cmp_evaluator_t, _cmp_evaluator);

  slist_t *it;
  evaluator_item_t *evaluator_item;
  feature_t* feature;
  int32_t i;
  record_t *record;
  cmp_evaluator_item_t *cmp_evaluator_item;

  if(!sniff){
    //It is an impulse
    this->send(NULL);
    return;
  }

  record = this->demand_record();

  cmp_evaluator_item = &this->evaluators[sniff->listener_id];
  for(i = 0, it = cmp_evaluator_item->evaluators; it; it = it->next, ++i){
    evaluator_item = it->data;
    feature = evaluator_item->feature;
    record->items[i] = feature->evaluator(sniff, &evaluator_item->evaluator_container);
  }

  record->listener_id = sniff->listener_id;
  set_mtime(&record->timestamp);

  this->send(record);

}

void clean_record(record_t* record)
{
  memset(record, 0, sizeof(record_t));
}

