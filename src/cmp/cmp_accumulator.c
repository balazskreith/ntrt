#include "cmp_accumulator.h"

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


static void _cmp_accumulator_init();
static void _cmp_accumulator_deinit();


CMP_DEF(, 			      	/*type of definitions*/ 									      \
		 cmp_accumulator_t,       /*type of component*/ 											      \
		 CMP_NAME_ACCUMULATOR,    /*name of the component*/ 										      \
		 _cmp_accumulator,        /*variable name of the component*/  								  \
		 cmp_accumulator_ctor,    /*name of the constructor function implemented automatically*/       \
		 cmp_accumulator_dtor,    /*name of the destructor function implemented automatically*/        \
		 _cmp_accumulator_init,   /*name of the external function called after construction*/          \
		 __NO_TEST_FUNC_,   /*name of the external function called after initialization*/         \
		 _cmp_accumulator_deinit  /*name of the external function called before destruction*/          \
		);


CMP_DEF_GET_PROC(get_cmp_accumulator, cmp_accumulator_t, _cmp_accumulator);

static void* _accumulator_restart();
static void* _accumulator_start();
static void* _accumulator_stop();
static void _accumulator_record_receiver(record_t* record);
static void _accumulator_result_requester(int32_t listener_id, record_t* result);

static void _push_record(accumulator_t *accumulator, record_t* record);
static void _obsolate_records(accumulator_t *accumulator);
static void _records_subtract(record_t* result, record_t* subtraction, int32_t length);
static void _records_addition(record_t* result, record_t* addition, int32_t length);
//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_accumulator_init()
{
    //constructions
  _cmp_accumulator->mutex = mutex_ctor();
    //Binding
    CMP_BIND(_cmp_accumulator->start,            _accumulator_start);
    CMP_BIND(_cmp_accumulator->stop,             _accumulator_stop);
    CMP_BIND(_cmp_accumulator->restart,          _accumulator_restart);
    CMP_BIND(_cmp_accumulator->record_receiver,  _accumulator_record_receiver);
    CMP_BIND(_cmp_accumulator->result_requester, _accumulator_result_requester);

}

void _cmp_accumulator_deinit()
{
  mutex_dtor(_cmp_accumulator->mutex);
}

void* _accumulator_start()
{
  CMP_DEF_THIS(cmp_accumulator_t, _cmp_accumulator);
  pcap_listener_t *pcap_listener;
  accumulator_t *accumulator;
  int32_t index;

  dmap_rdlock_table_pcapls();
  for(index = 0; dmap_itr_table_pcapls(&index, &pcap_listener) == BOOL_TRUE; ++index){
    accumulator = &this->accumulators[index];
    accumulator->records = datapuffer_ctor(pcap_listener->puffer_size);
    memset(&accumulator->result, 0, sizeof(record_t));
    accumulator->result.listener_id = index;
    accumulator->length = pcap_listener->feature_num;
  }
  dmap_rdunlock_table_pcapls();

  dmap_lock_sysdat();
  this->accumulation_time = dmap_get_sysdat()->accumulation_time;
  dmap_unlock_sysdat();
  return NULL;
}

void* _accumulator_stop()
{
  CMP_DEF_THIS(cmp_accumulator_t, _cmp_accumulator);
  pcap_listener_t *pcap_listener;
  accumulator_t *accumulator;
  int32_t index;

  dmap_rdlock_table_pcapls();
  for(index = 0; dmap_itr_table_pcapls(&index, &pcap_listener) == BOOL_TRUE; ++index){
    accumulator = &this->accumulators[index];
    datapuffer_dtor(accumulator->records);
  }
  dmap_rdunlock_table_pcapls();
  memset(&this->accumulators, 0, sizeof(accumulator_t) * NTRT_MAX_PCAPLS_NUM);
  return NULL;
}

void* _accumulator_restart()
{
    _accumulator_stop();
    _accumulator_start();
    return NULL;
}
//----------------------------------------------------------------------------------------------------
//---------------------------- Processes --------------------------------------------
//----------------------------------------------------------------------------------------------------

void _accumulator_record_receiver(record_t* record)
{
  CMP_DEF_THIS(cmp_accumulator_t, _cmp_accumulator);
  record_t* item;
  accumulator_t *accumulator;

  mutex_lock(this->mutex);
  if(!record){
    int32_t i;
    for(i=0; i<NTRT_MAX_PCAPLS_NUM; ++i){
      _obsolate_records(&this->accumulators[i]);
    }
    goto done;
  }

  accumulator = &this->accumulators[record->listener_id];
  _push_record(accumulator, record);

done:
  mutex_unlock(this->mutex);
}

void _accumulator_result_requester(int32_t listener_id, record_t* result)
{
  CMP_DEF_THIS(cmp_accumulator_t, _cmp_accumulator);
  accumulator_t *accumulator;

  mutex_lock(this->mutex);
  accumulator = this->accumulators + listener_id;
  _obsolate_records(accumulator);
  memcpy(result, &accumulator->result, sizeof(record_t));
  mutex_unlock(this->mutex);
}

void _push_record(accumulator_t *accumulator, record_t* record)
{
  CMP_DEF_THIS(cmp_accumulator_t, _cmp_accumulator);
  datapuffer_t* records = accumulator->records;
  record_t* item;
  if(datapuffer_isfull(records)){
    WARNINGPRINT("Recordspuffer for pcap_%d is too small", record->listener_id);
    item = datapuffer_read(records);
    _records_subtract(&accumulator->result, item, accumulator->length);
    this->send_record(item);
  }
  _records_addition(&accumulator->result, record, accumulator->length);
  datapuffer_write(records, record);

}

void _obsolate_records(accumulator_t *accumulator)
{
  CMP_DEF_THIS(cmp_accumulator_t, _cmp_accumulator);
  datapuffer_t* records = accumulator->records;
  record_t* item;
  if(!records){
    goto done;
  }
again:
  if(datapuffer_isempty(records)){
    goto done;
  }
  item = datapuffer_peek(records);
  if(this->accumulation_time < diffmtime_fromnow(&item->timestamp)){
    item = datapuffer_read(records);
    _records_subtract(&accumulator->result, item, accumulator->length);
    this->send_record(item);
    goto again;
  }
done:
  return;
}


void _records_subtract(record_t* result, record_t* subtraction, int32_t length)
{
  int32_t i;
  for(i=0; i<length; ++i){
    result->items[i] -= subtraction->items[i];
  }
}

void _records_addition(record_t* result, record_t* addition, int32_t length)
{
  int32_t i;
  for(i=0; i<length; ++i){
    result->items[i] += addition->items[i];
  }
}
