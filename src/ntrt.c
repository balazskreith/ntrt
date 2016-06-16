#include <stdio.h>
#include <stdlib.h>
#include "inc_inet.h"
#include "inc_predefs.h"
#include "inc_socket.h"
#include "inc_unistd.h"
#include "inc_io.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "inc_predefs.h"
#include "fsm.h"
#include "etc_argtable2.h"
#include "lib_interrupting.h"

static interrupt_t exit_signal;
static void init_sysio();
static void program_main(struct arg_int  *time, struct arg_file *cfg_file, struct arg_file *cmd_file);
static void _exit_handler(int sgno);
/**
 * Main function of server applcation that creates the tunnel interface, starts threads and configures the connections
 */
int main(int argc, char **argv)
{
    init_sysio();
    const char* __progname__ = argv[0];

    init_interrupt(&exit_signal, SIGINT, BOOL_FALSE);
    set_interrupt_sh(&exit_signal, _exit_handler);

    /* SYNTAX 1: server*/
    struct arg_lit  *help = arg_lit0("h","help","show help");
    struct arg_int  *time      = arg_int0("t", "time", "<n>", "time interval");
    struct arg_file *cfg_file = arg_filen("c",  "config_file", NULL, 0, argc+2,  "configuration file");
    struct arg_file *cmd_file = arg_filen("m",  "command_file", NULL, 0, argc+2,  "command file");
    struct arg_end  *end     = arg_end(20);
    void* argtable[] = {help, time, cfg_file, cmd_file, end};
    int nerrors;
    int exitcode=EXIT_SUCCESS;


    /* verify the argtable[] entries were allocated sucessfully */
    if (arg_nullcheck(argtable) != 0)
    {
      /* NULL entries were detected, some allocations must have failed */
      ERRORPRINT("%s: insufficient memory\n",__progname__);
      exitcode=1;
      goto exit;
    }

    /* Parse the command line as defined by argtable[] */
    nerrors = arg_parse(argc,argv,argtable);

    /* special case: '--help' takes precedence over error reporting */
    if (help->count > 0)
    {
      INFOPRINT("Usage: %s", __progname__);
      arg_print_syntax(stdout,argtable,"\n");
      INFOPRINT("Network Traffic Rate Tracker (ntrt).\n\n");
      arg_print_glossary(stdout,argtable,"  %-20s %s\n");
      exitcode=0;
      goto exit;
    }

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0)
    {
      /* Display the error details contained in the arg_end struct.*/
      arg_print_errors(stdout,end,__progname__);
      INFOPRINT("Try '%s --help' for more information.\n",__progname__);
      exitcode=1;
      goto exit;
    }

    fsm_ctor();
    program_main(time, cfg_file, cmd_file);
    fsm_dtor();
exit:
	return exitcode;
}

void _exit_handler(int sgno)
{
  fsm_kill();
}

void program_main(struct arg_int  *time, struct arg_file *cfg_file, struct arg_file *cmd_file)
{
    int sec;

    if(cfg_file->count){
      strcpy(sysio->config_file, *(cfg_file->filename));
    }

    if(cmd_file->count){
      strcpy(sysio->command_file, *(cmd_file->filename));
      sysio->command_file_initialized = BOOL_TRUE;
    }

    get_fsm()->fire(NTRT_EVENT_SETUP, NULL);
    get_fsm()->fire(NTRT_EVENT_START, NULL);

    if(!time->count){
      while(1){
        thread_sleep(1000);
      }
    }else{
      sec = *(time->ival);
      thread_sleep(1000 * sec);
    }

    get_fsm()->fire(NTRT_EVENT_STOP, NULL);
    get_fsm()->fire(NTRT_EVENT_SHUTDOWN, NULL);

}

void init_sysio()
{
	sysio = (devlegoio_t*) malloc(sizeof(devlegoio_t));
	sysio->print_stdlog = sysio_print_stdlog;
	sysio->print_stdout = sysio_print_stdout;
	sysio->opcall = system;
	sysio->command_file_initialized = BOOL_FALSE;
	strcpy(sysio->config_file, "./config.ini");
}
