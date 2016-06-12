#include "inc_ifaddr.h"
#include "lib_funcs.h"


array_t* get_machine_ip()
{
	array_t* result = array_ctor(1);

	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char *addr;
	int32_t *item = (int32_t*) malloc(sizeof(int32_t) * 4);
	memset(item, 0, sizeof(int32_t) * 4);

	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) {
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			addr = inet_ntoa(sa->sin_addr);
			memcpy(&item[3], addr, 4);
		}
	}
	freeifaddrs(ifap);

	int32_t index = dmap_add_etc(item, free);
	return index;
	set_array_item(result, 0, item);
	return result;
}

