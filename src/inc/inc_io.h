#ifdef _WIN32 //include libraries if the platform uses windows begin


#else

#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#endif //include libraries if the platform uses windows end

#ifndef INCGUARD_INC_IO_H_
#define INCGUARD_INC_IO_H_

#include "lib_descs.h"
#include "lib_defs.h"

#define NTRT_MAX_TUNRW_BYTES_NUM 2048

void sysio_print_stdlog(const char *format, ...);
void sysio_print_stdout(const char *format, ...);

int32_t sysio_bind_socket(int32_t port);


#endif /* INCGUARD_INC_IO_H_ */


