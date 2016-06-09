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

void init_sysio();
void server_main(int argc, char **argv);
void client_main(struct arg_str *server,
		struct arg_lit *ip6,
		struct arg_str *auth,
		struct arg_str *key,
		struct arg_int *port,
		struct arg_str *command);
/**
 * Main function of server applcation that creates the tunnel interface, starts threads and configures the connections
 */
int main(int argc, char **argv)
{
	init_sysio();
	const char* __progname__ = argv[0];

	/* SYNTAX 1: server*/
	struct arg_rex  *server_mode = arg_rex1(NULL,  NULL,  "server", NULL, REG_ICASE, NULL);
	struct arg_file *if_file  = arg_filen("i", "interfaces",  NULL, 0,argc+2, "interface configuration");
	struct arg_file *con_file = arg_filen("c",  "connection", NULL, 0, argc+2,  "connection configuration");
	struct arg_end  *end     = arg_end(20);
	void* argtable_server[] = {server_mode,if_file,con_file,end};
	int nerrors_server;
	int exitcode_server=0;

	/* SYNTAX 2: client */
	struct arg_rex  *client_mode  = arg_rex1(NULL,  NULL,  "client", NULL, REG_ICASE, NULL);
	struct arg_str  *server_addr  = arg_str0("s","server", "ip_address", "server ip address. Default is 127.0.0.1");
	struct arg_str  *auth_mode  = arg_strn("a","auth", "OP1|OP2",  0,argc+2,"Authentication mode. Default is NO");
	struct arg_str  *auth_key  = arg_strn("k","key", "[key]",  0,argc+2,"Authentication key. Default is YES");
	struct arg_lit  *is_ip6  = arg_litn("6","ip6", 0, argc + 2, "Indicates the server ip address is IP6 version");
	struct arg_int  *server_port  = arg_intn("p","port", "[0-65535]", 0,argc+2,"server port number. Default is 65050");
	struct arg_str  *command  = arg_strn(NULL,NULL,"COMMAND",0,argc+2,NULL);
	struct arg_end  *end2     = arg_end(20);
	void* argtable_client[] = {client_mode, server_addr, is_ip6, auth_mode, auth_key, server_port,command,end2};
	int nerrors_client;
	int exitcode_client=0;

	/* SYNTAX 3: description */
	struct arg_lit  *help = arg_lit0("h","help","show help");
	struct arg_lit  *vers = arg_lit0("v","verbose","description");
	struct arg_end  *end3     = arg_end(20);
	void* argtable_desc[] = {help, vers,end3};
	int nerrors_desc;
	int exitcode_desc=0;


	/* verify the argtable[] entries were allocated sucessfully */
	if (arg_nullcheck(argtable_server) != 0)
	{
		/* NULL entries were detected, some allocations must have failed */
		printf("%s: insufficient memory\n", __progname__);
		exitcode_server=1;
		goto exit;
	}

	/* Parse the command line as defined by argtable[] */
	nerrors_server = arg_parse(argc,argv,argtable_server);
	nerrors_client = arg_parse(argc,argv,argtable_client);
	nerrors_desc = arg_parse(argc,argv,argtable_desc);

	/* special case: '--help' takes precedence over error reporting */
	if (help->count > 0)
	//if (0)
	{
		printf("\nDescription: %s", __progname__);
		arg_print_syntax(stdout,argtable_desc,"\n");
		arg_print_glossary(stdout,argtable_desc,"  %-10s %s\n");
		printf("\nServer mode: %s", __progname__);
		arg_print_syntax(stdout,argtable_server,"\n");
		arg_print_glossary(stdout,argtable_server,"  %-10s %s\n");
		printf("\nClient mode: %s", __progname__);
		arg_print_syntax(stdout,argtable_client,"\n");
		arg_print_glossary(stdout,argtable_client,"  %-10s %s\n");
		exitcode_server=0;
		goto exit;
	}

	/* special case: '--version' takes precedence error reporting */
	if (vers->count > 0)
	//if (0)
	{
		printf("'%s' multipath program for performing a PhD\n",__progname__);
		printf("December 2014, Balázs Kreith\n");
		exitcode_server=0;
		goto exit;
	}

	/* If the parser returned any errors then display them and exit */

	fsm_ctor();
	//printf("%d->%s\n", strs->count, strs->sval[0]);
	//return 0;
	if(nerrors_server == 0){
		server_main(argc, argv);
	}else if(nerrors_client == 0){
		client_main(server_addr, is_ip6, auth_mode, auth_key, server_port, command);
	}else if (nerrors_desc != 0){
		/* Display the error details contained in the arg_end struct.*/
		arg_print_errors(stdout,end,__progname__);
		printf("Try '%s --help' for more information.\n",__progname__);
	}
	fsm_dtor();
exit:

	return EXIT_SUCCESS;
}

void server_main(int argc, char **argv)
{
	CRITICALPRINT("%s", "The selected mde is server");
	int min, hour, sec;
	get_fsm()->fire(MPT_EVENT_SETUP, NULL);
	get_fsm()->fire(MPT_EVENT_START, NULL);

	CRITICALPRINT("%s", "14h running v4");

	for(hour = 0; hour < 14; ++hour){
		for(min = 0; min < 60; ++min){
			for(sec = 0; sec < 60; ++sec){
				thread_sleep(1000);
			}
		}
		CRITICALPRINT("%dh elapsed", hour);
	}

	get_fsm()->fire(MPT_EVENT_STOP, NULL);
	get_fsm()->fire(MPT_EVENT_SHUTDOWN, NULL);

}

void client_main(struct arg_str *server,
		struct arg_lit *ip6,
		struct arg_str *auth,
		struct arg_str *key,
		struct arg_int *port,
		struct arg_str *command)
{
	CRITICALPRINT("%s", "The selected mde is client");
	get_fsm()->fire(MPT_EVENT_CREATE_CLIENT, NULL);
	if(command->count < 1){
		INFOPRINT("No command was given.");
		get_fsm()->fire(MPT_EVENT_SHUTDOWN, NULL);
		return;
	}
	if(auth->count > 0){
		get_fsm()->fire(MPT_EVENT_SET_CLIENT_AUTH, auth->sval[0]);
	}
	if(ip6->count > 0){
		get_fsm()->fire(MPT_EVENT_SET_CLIENT_SERVER_IP6, NULL);
	}
	if(server->count > 0){
		get_fsm()->fire(MPT_EVENT_SET_CLIENT_SERVER_ADDR, server->sval[0]);
	}
	if(key->count > 0){
		get_fsm()->fire(MPT_EVENT_SET_CLIENT_KEY, key->sval[0]);
	}
	if(port->count > 0){
		get_fsm()->fire(MPT_EVENT_SET_CLIENT_PORT, *(port->ival));
	}
	int32_t i, len = 0, str_len;
	for(i = 0; i < command->count; ++i){
		len += strlen(command->sval[i]);
	}
	char_t* cmd_str = (char_t*) malloc(sizeof(char_t) * len + 1);
	cmd_str[len] = '\0';
	for(len = 0, i = 0; i < command->count; ++i){
		str_len = strlen(command->sval[i]);
		memcpy(cmd_str + len, command->sval + i, str_len);
		len+=str_len;
	}
	*(cmd_str + len) = '\0';

	get_fsm()->fire(MPT_EVENT_CLIENT_DO, cmd_str);
	free(cmd_str);
}

void init_sysio()
{
	sysio = (devlegoio_t*) malloc(sizeof(devlegoio_t));
	sysio->print_stdlog = sysio_print_stdlog;
	sysio->print_stdout = sysio_print_stdout;
	sysio->tunnel_read = read;
	sysio->tunnel_start = sysio_tunnel_start;
	sysio->tunnel_stop = sysio_tunnel_stop;
	sysio->tunnel_write = write;
	sysio->udpsock_close = close;
	sysio->udpsock_open = socket;
	sysio->udpsock_recv = recvfrom;
	sysio->udpsock_send = sendto;
	sysio->udpsock_open_and_bind = sysio_bind_socket;
	sysio->opcall = system;
	strcpy(sysio->config_dir, "/usr/local/etc/mptsrv/conf");
	strcpy(sysio->bash_dir, "/usr/local/etc/mptsrv/bash");
}
