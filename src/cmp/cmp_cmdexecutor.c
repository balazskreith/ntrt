#include "cmp_cmdexecutor.h"

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


static void _cmp_cmdexecutor_init();
static void _cmp_cmdexecutor_deinit();
CMP_DEF(,                                       /*type of definitions*/                                                                                    \
                cmp_cmdexecutor_t,       /*type of component*/                                                                                        \
                 CMP_NAME_CMDEXECUTOR,    /*name of the component*/                                                                                   \
                 _cmp_cmdexecutor,        /*variable name of the component*/                                                          \
                 cmp_cmdexecutor_ctor,    /*name of the constructor function implemented automatically*/     \
                 cmp_cmdexecutor_dtor,    /*name of the destructor function implemented automatically*/      \
                 _cmp_cmdexecutor_init,   /*name of the external function called after construction*/        \
                 __NO_TEST_FUNC_,      /*name of the external function called after initialization*/       \
                 _cmp_cmdexecutor_deinit  /*name of the external function called before destruction*/        \
                );

typedef struct cmp_cmdinterpreter_struct_t
{
  char_t command_file[255];
  uint32_t seen_line;
}cmp_cmdinterpreter_t;

#define CMP_NAME_CMDINTERPRETER "Command Interpreter Component"
static void  _cmp_cmdinterpreter_init();
static void _thr_cmdinterpreter_main_proc(thread_t*);
CMP_THREAD(
           static,                         /*type of declaration*/
           cmp_cmdinterpreter_t,   /*type of the threaded component*/
           cmdinterpreter,                  /*unique name used for thread and component*/
           CMP_NAME_CMDINTERPRETER, /*name of the component*/
           _cmp_cmdinterpreter_init,         /*name of the process initialize the component*/
           __CMP_NO_DEINIT_FUNC_,       /*name of the process deinitialize the component*/
           _cmdinterpreter_start,   /*name of the process activate the thread*/
           _cmdinterpreter_stop,            /*name of the process deactivate the thread*/
           _thr_cmdinterpreter_main_proc    /*name of the process called in the thread*/
           );

CMP_DEF_GET_PROC(get_cmp_cmdexecutor, cmp_cmdexecutor_t, _cmp_cmdexecutor);

static void* _cmdexecutor_restart();
static void* _cmdexecutor_start();
static void* _cmdexecutor_stop();
//----------------------------------------------------------------------------------------------------
//---------------------------- Private definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

void  _cmp_cmdexecutor_init()
{
  CMP_BIND(_cmp_cmdexecutor->start,           _cmdexecutor_start);
  CMP_BIND(_cmp_cmdexecutor->stop,            _cmdexecutor_stop);
  CMP_BIND(_cmp_cmdexecutor->restart,         _cmdexecutor_restart);
}

void _cmp_cmdexecutor_deinit()
{

}

void* _cmdexecutor_start()
{
  CMP_DEF_THIS(cmp_cmdexecutor_t, _cmp_cmdexecutor);
  if(!sysio->command_file_initialized){
    return NULL;
  }

  _cmdinterpreter_start();
  return NULL;
}

void* _cmdexecutor_stop()
{
  if(!sysio->command_file_initialized){
    return NULL;
  }
  _cmdinterpreter_stop();
  return NULL;
}

void* _cmdexecutor_restart()
{
    _cmdexecutor_stop();
    _cmdexecutor_start();
    return NULL;
}

void  _cmp_cmdinterpreter_init()
{
  CMP_DEF_THIS(cmp_cmdinterpreter_t, _cmp_cmdinterpreter);
  strcpy(this->command_file, sysio->command_file);
  this->seen_line = 0;
}

//----------------------------------------------------------------------------------------------------
//---------------------------- Processes --------------------------------------------
//----------------------------------------------------------------------------------------------------
static void _command_interpreter_process(const char_t* str);

void _thr_cmdinterpreter_main_proc(thread_t *thread)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int32_t i=0, read_num, wait_in_seconds;
    CMP_DEF_THIS(cmp_cmdinterpreter_t, (cmp_cmdinterpreter_t*) thread->arg);

    fp = fopen(this->command_file, "r");
    if (fp == NULL){
        WARNINGPRINT("%s doesn't exists", this->command_file);
        return;
    }

    do
    {
      read = getline(&line, &len, fp);
      if(read == -1){
        break;
      }
      sscanf(line, "%d %d", &read_num, &wait_in_seconds);
      for(i=0; i<read_num; ++i){
        read = getline(&line, &len, fp);
        if(read == -1){
          break;
        }
        _command_interpreter_process(line);
      }

      if(read == -1){
        break;
      }

      thread_sleep(wait_in_seconds * 1000);

    }while(thread->state == THREAD_STATE_RUN);

    fclose(fp);
    if (line)
        free(line);
    INFOPRINT("Command interpreter is finished with file %s", this->command_file);
}



void _command_interpreter_process(const char_t* str)
{
  int32_t var_id;
  uint32_t var_value;
  mapped_var_t *mapped_var;
  const char_t* cmd = str + 2;

  INFOPRINT("Executing command %s", str);
  if(str[0] == 'O'){
    sysio->opcall(cmd);
    goto done;
  }else if(str[0] != 'I'){
    WARNINGPRINT("Only command started with \'I[:space:]\' or \'O[:space:]\' are accepted. This is not: %s", str);
    goto done;
  }

  if(sscanf(cmd, "MAPPED_VAR_%d_%d", &var_id, &var_value) == 2){
    dmap_wrlock_table_mapped_var();
    mapped_var = dmap_get_mapped_var_by_var_id(var_id);
    if(mapped_var){
      INFOPRINT("Mapped var %d value changed from %d to %d", var_id, mapped_var->value, var_value);
      mapped_var->value = var_value;
    }else{
      WARNINGPRINT("Coundn't find mapped var with id: %d", var_id);
    }
    dmap_wrunlock_table_mapped_var();
    goto done;
  }

done:
  return;
}



