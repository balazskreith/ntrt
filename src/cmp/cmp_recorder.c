#include "cmp_recorder.h"

#include "dmap_sel.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "cmp_predefs.h"
#include "lib_descs.h"
#include "lib_puffers.h"
#include "lib_threading.h"
#include <time.h>
#include <stdlib.h>
#include "inc_mtime.h"
#include "etc_utils.h"
#include <stdio.h>
#include "inc_texts.h"
#include "inc_unistd.h"
#include "inc_inet.h"
#include "inc_predefs.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "inc_io.h"
#include "lib_tors.h"
#include "etc_endian.h"
#include "lib_interrupting.h"

//----------------------------------------------------------------------------------------------------
//---------------------------- Public function definitions -------------------------------------------
//----------------------------------------------------------------------------------------------------
static void  _cmp_recorder_init();
static void  _cmp_recorder_deinit();
CMP_DEF(, 			      		/*type of definitions*/ 										   \
		cmp_recorder_t,       /*type of component*/ 										  	   \
		 CMP_NAME_RECORDER,    /*name of the component*/ 										   \
		 _cmp_recorder,        /*variable name of the component*/  							   \
		 cmp_recorder_ctor,    /*name of the constructor function implemented automatically*/     \
		 cmp_recorder_dtor,    /*name of the destructor function implemented automatically*/      \
		 _cmp_recorder_init,   /*name of the external function called after construction*/        \
		 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
		 _cmp_recorder_deinit  /*name of the external function called before destruction*/        \
		);																						   \

CMP_DEF_GET_PROC(get_cmp_recorder, cmp_recorder_t, _cmp_recorder);

typedef struct _cmp_recordsrelayer_struct_t
{
  void         (**features_requester)(int32_t, record_t*, record_t*);
  void         (**groupcounter_requester)(int32_t, record_t*);
  int32_t         sampling_rate;
}_cmp_recordsrelayer_t;

#define CMP_NAME_RECORDSRELAYER "Records relayer component"
static void _thr_recordsrelayer_main_proc(thread_t*);
CMP_THREAD(
                static,                         /*type of declaration*/
           _cmp_recordsrelayer_t,   /*type of the threaded component*/
           recordsrelayer,                  /*unique name used for thread and component*/
           CMP_NAME_RECORDSRELAYER, /*name of the component*/
           __CMP_NO_INIT_FUNC_,         /*name of the process initialize the component*/
           __CMP_NO_DEINIT_FUNC_,       /*name of the process deinitialize the component*/
           _recordsrelayer_start,   /*name of the process activate the thread*/
           _recordsrelayer_stop,            /*name of the process deactivate the thread*/
           _thr_recordsrelayer_main_proc    /*name of the process called in the thread*/
           );

static void* _recorder_start();
static void* _recorder_stop();
static void* _sender_restart();
static void _update_file(record_t* record,  record_t* groupcounter_record, pcap_listener_t* pcap_listener);
static void _aggregation_saver(pcap_listener_t *pcap_listener, record_t *record);

void  _cmp_recorder_init()
{
  _cmp_recordsrelayer_ctor();

  CMP_BIND(_cmp_recorder->start, _recorder_start);
  CMP_BIND(_cmp_recorder->stop, _recorder_stop);

  CMP_CONNECT(_cmp_recordsrelayer->features_requester, &_cmp_recorder->features_requester);
  CMP_CONNECT(_cmp_recordsrelayer->groupcounter_requester, &_cmp_recorder->groupcounter_requester);
}

void _cmp_recorder_deinit()
{
  _cmp_recordsrelayer_dtor();

}


static void _recordsrelayer_thr_init()
{

  dmap_lock_sysdat();
  _cmp_recordsrelayer->sampling_rate = dmap_get_sysdat()->sampling_rate;
  dmap_unlock_sysdat();

}

void* _recorder_start()
{
  _recordsrelayer_thr_init();
  _recordsrelayer_start();

   return NULL;
}

void* _sender_restart()
{
  _recorder_stop();
  _recorder_start();
  return NULL;
}

void* _recorder_stop()
{
  _recordsrelayer_stop();
  return NULL;
}


//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void _thr_recordsrelayer_main_proc(thread_t *thread)
{
    CMP_DEF_THIS(_cmp_recordsrelayer_t, (_cmp_recordsrelayer_t*) thread->arg);
    record_t         record, groupcounter_record, total;
    void           (*features_requester)(int32_t, record_t*, record_t*);
    void           (*groupcounter_requester)(int32_t, record_t*);
    int32_t          listener_id;
    pcap_listener_t *pcap_listener;

    features_requester = *(this->features_requester);
    groupcounter_requester = *(this->groupcounter_requester);
    memset(&total, 0, sizeof(record_t));
    memset(&record, 0, sizeof(record_t));
    do
    {
        dmap_rdlock_table_pcapls();
        for(listener_id = 0; dmap_itr_table_pcapls(&listener_id, &pcap_listener) == BOOL_TRUE; ++listener_id){
          features_requester(listener_id, &record, &total);
          if(0 < pcap_listener->groupcounter_num){
            memset(&groupcounter_record, 0, sizeof(record_t));
            groupcounter_requester(listener_id, &groupcounter_record);
          }
          _update_file(&record, &groupcounter_record, pcap_listener);
          if(pcap_listener->save_aggregation){
            _aggregation_saver(pcap_listener, &total);
            memset(&total, 0, sizeof(record_t));
          }
          memset(&record, 0, sizeof(record_t));
        }
        dmap_rdunlock_table_pcapls();
        thread_sleep(this->sampling_rate);

    }while(thread->state == THREAD_STATE_RUN);
}

void _aggregation_saver(pcap_listener_t *pcap_listener, record_t *record)
{
  CMP_DEF_THIS(cmp_recorder_t, (cmp_recorder_t*) _cmp_recorder);
  FILE *fp;
  int32_t i,c;

  fp=fopen(pcap_listener->aggregation_path, "w+");
  for(i=0,c=pcap_listener->feature_num; i<c; ++i){
    fprintf(fp,"%d%c", record->items[i], i == (c-1) ? '\n' : ',');
  }
  fclose(fp);
}


//----------------------------------------------------------------------------------------------------
//------------------------- Utils  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

static uint32_t _mapped_var_evaluator(uint32_t map_id)
{
  mapped_var_t *mapped_var;
  uint32_t result = 0;
  mapped_var = dmap_get_mapped_var_by_var_id(map_id);
  if(!mapped_var){
    WARNINGPRINT("mapped var with id %d not found", map_id);
    goto done;
  }
  result = mapped_var->value;
done:
  return result;
}

void _update_file(record_t* record, record_t* groupcounter_record, pcap_listener_t* pcap_listener)
{
  FILE *fp;
  char_t  end;
  int32_t i,c;
  if(pcap_listener->append_output){
    fp=fopen(pcap_listener->output, "a+");
  }else{
    fp=fopen(pcap_listener->output, "w+");
    pcap_listener->append_output = BOOL_TRUE;
  }
  end = 0 < pcap_listener->mapped_vars_num || 0 < pcap_listener->groupcounter_num? ',' : '\n';
  for(i=0,c=pcap_listener->feature_num; i<c; ++i){
      fprintf(fp,"%d%c", record->items[i], i == (c-1) ? end : ',');
  }

  end = 0 < pcap_listener->mapped_vars_num ? ',' : '\n';
  for(i=0,c=pcap_listener->groupcounter_num; i<c; ++i){
    fprintf(fp,"%d%c", groupcounter_record->items[i], i == (c-1) ? end : ',');
  }

  c=pcap_listener->mapped_vars_num;
  if(!c){
    goto done;
  }

  dmap_rdlock_table_mapped_var();
  for(i=0; i<c; ++i){
    uint32_t map_id;
    map_id = pcap_listener->mapped_var_ids[i];
    fprintf(fp,"%d%c", _mapped_var_evaluator(map_id), i == (c-1) ? '\n' : ',');
  }
  dmap_rdunlock_table_mapped_var();

done:
  fclose(fp);
}


