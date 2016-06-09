#include "sys_prints.h"
#include <stdlib.h>
#include <stdio.h>
#include "inc_inet.h"
#include "dmap.h"
#include <stdarg.h>

static void _print_tun(FILE *stream, tunnel_t *tun,     sysdat_t *sysdat);
static void _print_con(FILE *stream, connection_t *con);
static void _print_inf(FILE *stream, interface_t *inf);
static void _print_net(FILE *stream, network_t *net);
static void _print_pth(FILE *stream, path_t *pth);

/*
#define PRINT_INFO(STREAM, TEXT, PRINTF_TYPE, VALUE)                     \
	fprintf(STREAM, "%s: %*"PRINTF_TYPE"\n", TEXT, 30-strlen(TEXT), VALUE)

#define PRINT_INFO_STRING(STREAM, TEXT, VALUE)                           \
		PRINT_INFO(STREAM, TEXT, "s", VALUE)

#define PRINT_INFO_NUM(STREAM, TEXT, VALUE)                              \
		PRINT_INFO(STREAM, TEXT, "d", VALUE)
*/

void* sys_stdout_cmd(const char* format, ...)
{
	char_t    text[1024];
	va_list args;
	va_start (args, format);
	vsprintf(text, format, args);
	va_end (args);
	INFOPRINT("%s", text);
	return NULL;
}

void* sys_print_all()
{
	sys_print_all_tun();
	sys_print_all_con();
	sys_print_all_inf();
	sys_print_all_net();
	sys_print_all_pth();
	return NULL;
}

void* sys_print_all_tun()
{
	int32_t   index;
	tunnel_t *tun = NULL;
	dmap_lock_sysdat();
	dmap_rdlock_table_tun();
	for(index = 0; dmap_itr_table_tun(&index, &tun) == BOOL_TRUE; ++index){
		sys_print_tun(tun);
	}
	dmap_rdunlock_table_tun();
	dmap_unlock_sysdat();
	return NULL;
}

void* sys_print_tun(tunnel_t *tun)
{
	_print_tun(stdout, tun, dmap_get_sysdat());
	return NULL;
}


void* sys_print_all_con()
{
	int32_t   index;
	connection_t *con = NULL;
	dmap_rdlock_table_con();
	for(index = 0; dmap_itr_table_con(&index, &con) == BOOL_TRUE; ++index){
		sys_print_con(con);
	}
	dmap_rdunlock_table_con();
	return NULL;
}

void* sys_print_con(connection_t *con)
{
	_print_con(stdout, con);
	return NULL;
}


void* sys_print_all_inf()
{
	int32_t   index;
	interface_t *inf = NULL;
	dmap_rdlock_table_inf();
	for(index = 0; dmap_itr_table_inf(&index, &inf) == BOOL_TRUE; ++index){
		sys_print_inf(inf);
	}
	dmap_rdunlock_table_inf();
	return NULL;
}

void* sys_print_inf(interface_t *inf)
{
	_print_inf(stdout, inf);
	return NULL;
}

void* sys_print_all_net()
{
	int32_t   index;
	network_t *net = NULL;
	dmap_rdlock_table_net();
	for(index = 0; dmap_itr_table_net(&index, &net) == BOOL_TRUE; ++index){
		sys_print_net(net);
	}
	dmap_rdunlock_table_net();
	return NULL;
}

void* sys_print_net(network_t *net)
{
	_print_net(stdout, net);
	return NULL;
}

void* sys_print_all_pth()
{
	int32_t   index;
	path_t *pth = NULL;
	dmap_rdlock_table_pth();
	for(index = 0; dmap_itr_table_pth(&index, &pth) == BOOL_TRUE; ++index){
		sys_print_pth(pth);
	}
	dmap_rdunlock_table_pth();
	return NULL;
}

void* sys_print_pth(path_t *pth)
{
	_print_pth(stdout, pth);
	return NULL;
}


//----------------------------------------------------------------------------
void _print_tun(FILE *stream, tunnel_t *tun, sysdat_t *sysdat)
{
    char ipstr[256];
    fprintf(stream, "\n");
    fprintf(stream, "Tunnel information:\n");
    fprintf(stream, "    Tunnel interface : %s \n", tun->if_name);
    fprintf(stream, "    Tunnel device    : %s \n", tun->device);
    inet_ntop(AF_INET, &tun->ip4, ipstr, 255);

    if (tun->ip4len)
    	fprintf(stream, "    Tunnel ipv4 addr.: %s/%d \n", ipstr, tun->ip4len);
    inet_ntop(AF_INET6, &tun->ip6, ipstr, 255);

    if (tun->ip6len)
    	fprintf(stream, "    Tunnel ipv6 addr.: %s/%d \n", ipstr, tun->ip6len);
		fprintf(stream, "    CMD server port number  : %d \n", tun->cmd_port_rcv );
		fprintf(stream, "    CMD client port number  : %d \n", tun->cmd_port_snd );
	//    fprintf(stream, "    CMD client socket id    : %d \n", tun->cmd_socket_snd );
		fprintf(stream, "    Global server    : %d \n", sysdat->mp_global_server );
		fprintf(stream, "\n");
	}




void _print_con(FILE *stream, connection_t *conn)
{
	char lipstr[128], ripstr[128], scheduling_type[255];
	//int  af, start;

    strcpy(lipstr, ""); strcpy(ripstr, "");
    if (conn->ip_version == 4) {
            inet_ntop(AF_INET, &conn->ip_local[3], lipstr, 128);
            inet_ntop(AF_INET, &conn->ip_remote[3], ripstr, 128);
    }
    if (conn->ip_version == 6) {
            inet_ntop(AF_INET6, &conn->ip_local, lipstr, 128);
            inet_ntop(AF_INET6, &conn->ip_remote, ripstr, 128);
    }

    if(conn->scheduling_type == PACKET_SCHEDULING_BALANCED){
    	strcat(scheduling_type, "Balanced");
    }else if(conn->scheduling_type == PACKET_SCHEDULING_WINDOWED){
    	strcat(scheduling_type, "Windowed");
    }
    fprintf(stream, "\n");
    //fprintf(stream, "Multipath Connection Information(%d):\n", num);
    fprintf(stream, "Connection name  : %s \n", conn->name);
    fprintf(stream, "    Config file           : %s \n", conn->filename);
    fprintf(stream, "    IP version            : %d \n", conn->ip_version);
    fprintf(stream, "    Local  Tunnel address : %s \n", lipstr);
    fprintf(stream, "    Local  UDP Port       : %d \n", conn->port_local);
    fprintf(stream, "    Remote Tunnel address : %s \n", ripstr);
    fprintf(stream, "    Remote UDP Port       : %d \n", conn->port_remote);
    //fprintf(stream, "    Socket ID             : %d \n", conn->socket);
    fprintf(stream, "    Remote CMD UDP Port   : %d \n", conn->cmd_port_remote);
    fprintf(stream, "    Number of Paths       : %d \n", conn->path_count );
    fprintf(stream, "    Number of networks    : %d \n", conn->network_count );
    fprintf(stream, "    Packets sent out      : %lld \n", conn->conn_packet );
    fprintf(stream, "    Connection update     : %d \n", conn->permission );
    fprintf(stream, "    Keepalive time        : %d \n", conn->keepalive );
    fprintf(stream, "    Dead timer            : %d \n", conn->deadtimer );
    fprintf(stream, "    Authentication type   : %d \n", conn->auth_type );
    fprintf(stream, "    Packet scheduling type: %s \n", scheduling_type );
    //fprintf(stream, "    Authentication code   : "); printHash(conn->auth_key, keySize(conn->auth_type));
    fprintf(stream, "    Connection status     : %d\n", conn->status );

}


void _print_inf(FILE *stream, interface_t *inf)
{

}

void _print_net(FILE *stream, network_t *net)
{
	char lipstr[128], ripstr[128];
	//int  af, start;

	strcpy(lipstr, ""); strcpy(ripstr, "");
	if (net->version == 4) {
			inet_ntop(AF_INET, &net->source[3], lipstr, 128);
			inet_ntop(AF_INET, &net->destination[3], ripstr, 128);
	}
	if (net->version == 6) {
			inet_ntop(AF_INET6, &net->source, lipstr, 128);
			inet_ntop(AF_INET6, &net->destination, ripstr, 128);
	}

	fprintf(stream, "-------- Network ----------\n");
	fprintf(stream, "    IP version       : %d \n", net->version);
	fprintf(stream, "    Source           : %s \n", lipstr);
	fprintf(stream, "    Source P. length : %d \n", net->source_prefix_length);
	fprintf(stream, "    Destination      : %s \n", ripstr);
	fprintf(stream, "    Dest. P. length  : %d \n", net->destination_prefix_length);

}

void _print_pth(FILE *stream, path_t *p)
{
	char lipstr[128], ripstr[128], gwstr[128];
	unsigned char *mac;
	int af, start;

	strcpy(lipstr, ""); strcpy(ripstr, ""); strcpy(gwstr, "");

	if (p->ip_version == 6) {
		af = AF_INET6;
		start = 0;
	}
	else {
		af = AF_INET;
		start = 3;
	}
	inet_ntop(af, &p->ip_local[start], lipstr, 128);
	inet_ntop(af, &p->ip_remote[start], ripstr, 128);
	inet_ntop(af, &p->ip_gw[start], gwstr, 128);

	fprintf(stream, "  Path information:\n");
	fprintf(stream, "     Interface name:    : %s \n", p->interface_name);
//        fprintf(stream, "     IP version         : %d \n", p->ip_version);
	mac = (unsigned char *)p->mac_local;
	fprintf(stream, "     Local  MAC address : %02X:%02X:%02X:%02X:%02X:%02X \n",
						mac[0], mac[1],mac[2],mac[3],mac[4],mac[5]);
	fprintf(stream, "     Local  IP address  : %s \n", lipstr);
	mac = (unsigned char *)p->mac_gw;
	fprintf(stream, "     Gateway MAC address: %02X:%02X:%02X:%02X:%02X:%02X \n",
						mac[0], mac[1],mac[2],mac[3],mac[4],mac[5]);
	fprintf(stream, "     Gateway IP address : %s \n", gwstr);
	fprintf(stream, "     Remote IP address  : %s \n", ripstr);
	fprintf(stream, "     Incoming weight    : %d \n", p->weight_in);
	fprintf(stream, "     Outgoing weight    : %d \n", p->weight_out);
	fprintf(stream, "     Path window size   : %d \n", p->packet_max);
	fprintf(stream, "     Path Status        : %d \n", p->status);
	fprintf(stream, "     Bandwidth          : %dBit/s \n", p->bandwidth);
	fprintf(stream, "     Average latency    : %dms \n", p->latency);
}
