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

typedef struct{
  int32_t  features_num;
  record_t features;
  int32_t  groupcounters_num;
  record_t groupcounters;
  bool_t   save_aggregations;
  record_t aggregations;
  bool_t   append_output;
  char_t   features_output[255];
  char_t   aggregation_path[255];
  int32_t  mapped_vars_num;
  uint32_t mapped_var_values[NTRT_MAX_MAPPED_VARS];
}mxrecord_t;

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
  void         (*write_mxrecord)(mxrecord_t*);
  mxrecord_t*  (*demand_mxrecord)();
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


static mxrecord_t* _mxrecord_ctor();
static void _clean_mxrecord(mxrecord_t* record);
#define CMP_PUFFER_RECYCLE_SIZE 128
#define CMP_NAME_RRECYCLE "MXRecords Recycle component"
CMP_DEF_RECPUFFER(
                static,                         /*type of the declarations*/
                mxrecord_t,                       /*name of the type of the items*/
                _mxrecord_ctor,            /*name of the process used for constructing an item if the puffer is empty.*/
                free,    /*name of the process used for deconstruting an item if the puffer is full.*/
                _clean_mxrecord,           /*name of the process clear the item when the recycle receives it*/
                CMP_NAME_RRECYCLE,      /*name of the recycle puffer*/
                _cmp_recycle,           /*name of the variable used for referencing to the recycle*/
                CMP_PUFFER_RECYCLE_SIZE, /*length of the recycle puffer*/
                _cmp_recycle_ctor,      /*name of the constructor process for this recycle*/
                _cmp_recycle_dtor       /*name of the destructor process for this recycle*/
                );


#define CMP_MXRECORDSPUFFER_SIZE 256
#define CMP_NAME_MXRECORDSPUFFER "Multiplexed Records puffer"
CMP_DEF_SGPUFFER(
                  static,                                 /*type of declaration*/
                  mxrecord_t,                              /*type of items*/
                  free,                       /*name of the destrctor process*/
                  CMP_NAME_MXRECORDSPUFFER,    /*name of the component*/
                  _cmp_mxrecordspuffer,         /*name of the variable used for referencing it*/
                  CMP_MXRECORDSPUFFER_SIZE,        /*maximal number of items in the puffer*/
                  _cmp_mxrecordspuffer_ctor,    /*name of the process used for constructing*/
                  _cmp_mxrecordspuffer_dtor     /*name of the process used for deconstructing*/
          );


typedef struct _cmp_filewriter_struct_t
{
  void         (*recycle_mxrecord)(mxrecord_t*);
  mxrecord_t*  (*demand_mxrecord)();
}_cmp_filewriter_t;

#define CMP_NAME_FILEWRITER "FileWriter component"
static void _thr_filewriter_main_proc(thread_t*);
CMP_THREAD(
                static,                         /*type of declaration*/
                _cmp_filewriter_t,   /*type of the threaded component*/
                filewriter,                  /*unique name used for thread and component*/
                CMP_NAME_FILEWRITER, /*name of the component*/
           __CMP_NO_INIT_FUNC_,         /*name of the process initialize the component*/
           __CMP_NO_DEINIT_FUNC_,       /*name of the process deinitialize the component*/
           _filewriter_start,   /*name of the process activate the thread*/
           _filewriter_stop,            /*name of the process deactivate the thread*/
           _thr_filewriter_main_proc    /*name of the process called in the thread*/
           );


static void* _recorder_start();
static void* _recorder_stop();
static void* _sender_restart();
static uint32_t _mapped_var_evaluator(uint32_t map_id);
static void _update_features_file(mxrecord_t *mxrecord);
static void _aggregation_saver(mxrecord_t *mxrecord);

void  _cmp_recorder_init()
{
  _cmp_recordsrelayer_ctor();
  _cmp_mxrecordspuffer_ctor();
  _cmp_filewriter_ctor();
  _cmp_recycle_ctor();

  CMP_BIND(_cmp_recorder->start, _recorder_start);
  CMP_BIND(_cmp_recorder->stop, _recorder_stop);

  CMP_CONNECT(_cmp_recordsrelayer->features_requester, &_cmp_recorder->features_requester);
  CMP_CONNECT(_cmp_recordsrelayer->groupcounter_requester, &_cmp_recorder->groupcounter_requester);

  CMP_CONNECT(_cmp_recordsrelayer->write_mxrecord, _cmp_mxrecordspuffer->receiver);
  CMP_CONNECT(_cmp_recordsrelayer->demand_mxrecord, _cmp_recycle->supplier);

  CMP_CONNECT(_cmp_filewriter->recycle_mxrecord, _cmp_recycle->receiver);
  CMP_CONNECT(_cmp_filewriter->demand_mxrecord, _cmp_mxrecordspuffer->supplier);
}

void _cmp_recorder_deinit()
{
  _cmp_recordsrelayer_dtor();
  _cmp_mxrecordspuffer_dtor();
  _cmp_filewriter_dtor();
  _cmp_recycle_dtor();
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
  _filewriter_start();

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
  _filewriter_stop();
  return NULL;
}


//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


void _thr_recordsrelayer_main_proc(thread_t *thread)
{
    CMP_DEF_THIS(_cmp_recordsrelayer_t, (_cmp_recordsrelayer_t*) thread->arg);
    void           (*features_requester)(int32_t, record_t*, record_t*);
    void           (*groupcounter_requester)(int32_t, record_t*);
    int32_t          listener_id;
    pcap_listener_t *pcap_listener;
    mxrecord_t*      mxrecord;

    features_requester = *(this->features_requester);
    groupcounter_requester = *(this->groupcounter_requester);

    do
    {
        PROFILING("_thr_recordsrelayer_main_proc",

        mxrecord = this->demand_mxrecord();

        dmap_wrlock_table_pcapls();
        for(listener_id = 0; dmap_itr_table_pcapls(&listener_id, &pcap_listener) == BOOL_TRUE; ++listener_id){
          features_requester(listener_id, &mxrecord->features, &mxrecord->aggregations);
          mxrecord->features_num = pcap_listener->feature_num;
          if(0 < pcap_listener->groupcounter_num){
            groupcounter_requester(listener_id, &mxrecord->groupcounters);
            mxrecord->groupcounters_num = pcap_listener->groupcounter_num;
          }

          mxrecord->append_output = pcap_listener->append_output;
          pcap_listener->append_output = BOOL_TRUE;
          strcpy(mxrecord->features_output, pcap_listener->output);


          if(0 < pcap_listener->mapped_vars_num){
              int32_t i;
              mxrecord->mapped_vars_num = pcap_listener->mapped_vars_num;
              dmap_rdlock_table_mapped_var();
              for(i=0; i<pcap_listener->mapped_vars_num; ++i){
                uint32_t map_id;
                map_id = pcap_listener->mapped_var_ids[i];
                mxrecord->mapped_var_values[i] = _mapped_var_evaluator(map_id);
              }
              dmap_rdunlock_table_mapped_var();
          }

          if(pcap_listener->save_aggregation){
            mxrecord->save_aggregations = BOOL_TRUE;
            strcpy(mxrecord->aggregation_path, pcap_listener->aggregation_path);
          }

          //TODO: In mxrecord distinguishing
          this->write_mxrecord(mxrecord);
        }
        dmap_wrunlock_table_pcapls();

        );

        thread_sleep(this->sampling_rate);

    }while(thread->state == THREAD_STATE_RUN);
}



void _thr_filewriter_main_proc(thread_t *thread)
{
    CMP_DEF_THIS(_cmp_filewriter_t, (_cmp_filewriter_t*) thread->arg);
    int32_t          listener_id;
    pcap_listener_t *pcap_listener;
    mxrecord_t*      mxrecord;

    do
    {
        mxrecord = this->demand_mxrecord();
        _update_features_file(mxrecord);
        if(mxrecord->save_aggregations){
          _aggregation_saver(mxrecord);
        }
        this->recycle_mxrecord(mxrecord);
    }while(thread->state == THREAD_STATE_RUN);
}




void _aggregation_saver(mxrecord_t *mxrecord)
{
  CMP_DEF_THIS(cmp_recorder_t, (cmp_recorder_t*) _cmp_recorder);
  FILE *fp;
  int32_t i,c;

  fp=fopen(mxrecord->aggregation_path, "w+");
  for(i=0,c=mxrecord->features_num; i<c; ++i){
    fprintf(fp,"%d%c", mxrecord->aggregations.items[i], i == (c-1) ? '\n' : ',');
  }
  fclose(fp);
}


//----------------------------------------------------------------------------------------------------
//------------------------- Utils  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

uint32_t _mapped_var_evaluator(uint32_t map_id)
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

void _update_features_file(mxrecord_t *mxrecord)
{
  FILE *fp;
  char_t  end;
  int32_t i,c;
  if(mxrecord->append_output){
    fp=fopen(mxrecord->features_output, "a+");
  }else{
    fp=fopen(mxrecord->features_output, "w+");
  }

  end = 0 < mxrecord->mapped_vars_num || 0 < mxrecord->groupcounters_num? ',' : '\n';
  for(i=0,c=mxrecord->features_num; i<c; ++i){
      fprintf(fp,"%d%c", mxrecord->features.items[i], i == (c-1) ? end : ',');
  }

  end = 0 < mxrecord->mapped_vars_num ? ',' : '\n';
  for(i=0,c=mxrecord->groupcounters_num; i<c; ++i){
    fprintf(fp,"%d%c", mxrecord->groupcounters.items[i], i == (c-1) ? end : ',');
  }

  c=mxrecord->mapped_vars_num;
  if(!c){
    goto done;
  }

  dmap_rdlock_table_mapped_var();
  for(i=0; i<c; ++i){
    fprintf(fp,"%d%c", mxrecord->mapped_var_values[i], i == (c-1) ? '\n' : ',');
  }
  dmap_rdunlock_table_mapped_var();

done:
  fclose(fp);
}

mxrecord_t* _mxrecord_ctor()
{
  mxrecord_t* result;
  result = (mxrecord_t*) malloc(sizeof(mxrecord_t));
  memset(result, 0, sizeof(mxrecord_t));
  return result;
}

void _clean_mxrecord(mxrecord_t* mxrecord)
{
  memset(mxrecord, 0, sizeof(mxrecord_t));
}
