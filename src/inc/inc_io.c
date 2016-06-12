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


