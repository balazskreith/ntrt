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

#define CMP_PUFFER_RECORDS_SIZE 128
#define CMP_NAME_RECORDSPUFFER "Records Signal Puffer"
CMP_DEF_SGPUFFER(
                        static,                                 /*type of declaration*/
                        record_t,                              /*type of items*/
                        record_dtor,                       /*name of the destrctor process*/
                        CMP_NAME_RECORDSPUFFER,    /*name of the component*/
                        _cmp_recordpuffer,         /*name of the variable used for referencing it*/
                        CMP_PUFFER_RECORDS_SIZE,  /*maximal number of items in the puffer*/
                        _cmp_recordspuffer_ctor,    /*name of the process used for constructing*/
                        _cmp_recordspuffer_dtor     /*name of the process used for deconstructing*/
                );

typedef struct output_file_struct_t{
  char_t    filename[255];
  bool_t    append_output;
}output_file_t;

typedef struct ratewindow_struct_t{
  int32_t       length;
  int32_t       index;
  record_t*     records;
  record_t      result;
  output_file_t output_file;
}ratewindow_t;

typedef struct _cmp_cmdrelayer_struct_t
{
        record_t*    (*demand)();
        void         (**send)(record_t*);
        ratewindow_t   rate_windows[NTRT_MAX_PCAPLS_NUM];
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
static void _records_subtract(record_t* result, record_t* subtraction);
static void _records_addition(record_t* result, record_t* addition);
static void _update_rate_window(ratewindow_t* ratewindow, record_t* record);
static void _update_file(output_file_t* output_file, record_t* record);

void  _cmp_recorder_init()
{
  _cmp_recordspuffer_ctor();
  _cmp_recordsrelayer_ctor();

  CMP_BIND(_cmp_recorder->start, _recorder_start);
  CMP_BIND(_cmp_recorder->stop, _recorder_stop);
  CMP_BIND(_cmp_recorder->receiver, _cmp_recordpuffer->receiver);

  CMP_CONNECT(_cmp_recordsrelayer->demand, _cmp_recordpuffer->supplier);
  CMP_CONNECT(_cmp_recordsrelayer->send, &_cmp_recorder->send);

}

void _cmp_recorder_deinit()
{
  _cmp_recordspuffer_dtor();
  _cmp_recordsrelayer_dtor();

}


static void _recordsrelayer_thr_init()
{
  int32_t index;
  int32_t j;
  pcap_listener_t *pcap_listener;
  CMP_DEF_THIS(_cmp_recordsrelayer_t, _cmp_recordsrelayer);

  dmap_rdlock_table_pcapls();
  for(index = 0; dmap_itr_table_pcapls(&index, &pcap_listener) == BOOL_TRUE; ++index){
    memset(&this->rate_windows[index], 0, sizeof(ratewindow_t));
    strcpy(this->rate_windows[index].output_file.filename, pcap_listener->output);
    this->rate_windows[index].output_file.append_output = pcap_listener->append_output;
    this->rate_windows[index].records = malloc(sizeof(record_t) * pcap_listener->accumulation_length);
    memset(this->rate_windows[index].records, 0, sizeof(record_t) * pcap_listener->accumulation_length);
    this->rate_windows[index].length = pcap_listener->accumulation_length;
    this->rate_windows[index].index = 0;
  }
  dmap_rdunlock_table_pcapls();
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
    record_t    *record;
    int32_t     i;
    void       (*send)(record_t*);
    int32_t      listener_id;
    ratewindow_t *ratewindow;
    //        time_t       now;
    send = *(this->send);
    do
    {
        //CMP_DEMAND(CMP_NAME_CMDRELAYER, command, this->demand);
        record = this->demand();
        listener_id = record->listener_id;
        ratewindow = &this->rate_windows[listener_id];
        _update_rate_window(ratewindow, record);
        _update_file(&ratewindow->output_file, &ratewindow->result);

        send(record);
    }while(thread->state == THREAD_STATE_RUN);
}


//----------------------------------------------------------------------------------------------------
//------------------------- Utils  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void _records_subtract(record_t* result, record_t* subtraction)
{
  int32_t i,c;
  c = MIN(result->length, subtraction->length);
  for(i=0; i<c; ++i){
    if(!result->accumulable[i]){
      continue;
    }else{
      result->items[i] -= subtraction->items[i];
    }
  }
}

void _records_addition(record_t* result, record_t* addition)
{
  int32_t i,c;
  c = MIN(result->length, addition->length);
  for(i=0; i<c; ++i){
    if(result->accumulable[i] == BOOL_FALSE){
      result->items[i] = addition->items[i];
    }else{
      result->items[i] += addition->items[i];
    }
  }
}

void _update_rate_window(ratewindow_t* ratewindow, record_t* record)
{
  _records_subtract(&ratewindow->result, ratewindow->records + ratewindow->index);
  memcpy(ratewindow->records + ratewindow->index, record, sizeof(record_t));
  _records_addition(&ratewindow->result, ratewindow->records + ratewindow->index);

  ratewindow->result.length = record->length;

  if(++(ratewindow->index) == ratewindow->length){
      ratewindow->index = 0;
  }
}

void _update_file(output_file_t* output_file, record_t* record)
{
  FILE *fp;
  int32_t i,c;
  if(output_file->append_output){
    fp=fopen(output_file->filename, "a+");
  }else{
    fp=fopen(output_file->filename, "w+");
    output_file->append_output = BOOL_TRUE;
  }
  for(i=0,c=record->length; i<c; ++i){
      fprintf(fp,"%d%c", record->items[i], i == (c-1) ? '\n' : ',');
  }
  fclose(fp);
}

