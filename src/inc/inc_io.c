#include "inc_io.h"
#include <stdlib.h>
#include "inc_texts.h"
#include "inc_predefs.h"
#include "lib_descs.h"
#include "dmap.h"
#include <stdarg.h>



void sysio_print_stdlog(const char *format,...)
{
	va_list args;
	va_start (args, format);
	vfprintf (stderr, format, args);
	va_end (args);
}

void sysio_print_stdout(const char *format,...)
{
	va_list args;
	va_start (args, format);
	vfprintf (stdout, format, args);
	va_end (args);
}

int32_t sysio_tunnel_start(tunnel_t *tun)
{
	char cmdbuf[256], ipstr[256];
	//int mtu = 1452; // 1500-sizeof(IPv6+UDP header)
	int mtu = MPT_MTU_SIZE - 48; // 1500-sizeof(IPv6+UDP header)
	int qlen = MPT_MTU_SIZE, sockfd;
	struct ifreq ifr;
	int32_t result;
	if ((result = open(tun->device, O_RDWR)) < 0){
		EXERROR("Tunnel inteface opening error. ", errno);
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;  // for tun (point-to-point) interface
	strncpy(ifr.ifr_name, tun->if_name, IFNAMSIZ);
	if (ioctl(result, TUNSETIFF, (void *) &ifr) < 0) {
		/* Try old ioctl */
		if (ioctl(result, OTUNSETIFF, (void *) &ifr) < 0) {
			close(result);
			EXERROR("Tunnel interface IOCTL error.", errno);
			return -2;
		}
	}
	sockfd = socket(PF_INET, SOCK_DGRAM, 0); // set the outgoing que length
	if (sockfd > 0) {
			strcpy(ifr.ifr_name, tun->if_name);
			ifr.ifr_qlen = qlen;
			if (ioctl(sockfd, SIOCSIFTXQLEN, &ifr) < 0) {
				fprintf(stderr, "Tunnel interface qlen IOCTL error - %d.", errno);
			}
			close(sockfd);
	}

	if (tun->ip4len) {
		inet_ntop(AF_INET, &tun->ip4, ipstr, 255);
		sprintf(cmdbuf,"%s/mpt_tunnel_start.sh ipv4 %s %s/%d %d",
			sysio->bash_dir, tun->if_name, ipstr, tun->ip4len, mtu);
//printf("Cmd: %s\n", cmdbuf);
		system(cmdbuf);
	}

	if (tun->ip6len ) {
		inet_ntop(AF_INET6, &tun->ip6, ipstr, 255);
		sprintf(cmdbuf,"%s/mpt_tunnel_start.sh ipv6 %s %s/%d %d",
			sysio->bash_dir, tun->if_name, ipstr, tun->ip6len, mtu);
//printf("Cmd: %s\n", cmdbuf);
		system(cmdbuf);
	}
	return result;
}

void sysio_tunnel_stop(tunnel_t *tun)
{
	if(tun == NULL){
		ERRORPRINT("The tunnel you want to close is not exists.");
		return;
	}
	close(tun->fd);
}


int32_t sysio_bind_socket(int32_t port)
{
	//initialize
	int32_t sock, status;
	struct sockaddr_in6 sin6;
	int32_t sin6len;
	int off = 0;

	//sock = socket(PF_INET6, SOCK_DGRAM,0);
	sock = socket(AF_INET6, SOCK_DGRAM,0);
    setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof off);

	sin6len = sizeof(struct sockaddr_in6);
	memset(&sin6, 0, sin6len);

	//just use the first address returned in the structure
	sin6.sin6_port = htons(port);
	sin6.sin6_family = AF_INET6;
	sin6.sin6_addr = in6addr_any;

	status = bind(sock, (struct sockaddr *)&sin6, sin6len);
	if(-1 == status){
		EXERROR("socket bind error", EXIT_FAILURE);
	}

	status = getsockname(sock, (struct sockaddr *)&sin6, &sin6len);
	return sock;
}


