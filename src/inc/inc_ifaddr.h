#ifdef _WIN32 //include libraries if the platform uses windows begin


#else
#include <ifaddrs.h>

#endif //include libraries if the platform uses windows end

#ifndef INCGUARD_INC_IFADDR_H_
#define INCGUARD_INC_IFADDR_H_

#include "lib_descs.h"

array_t* get_machine_ip();

#endif /* INCGUARD_INC_IFADDR_H_ */


