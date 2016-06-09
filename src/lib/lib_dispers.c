#include "lib_dispers.h"
#include "lib_threading.h"
#include "dmap.h"

void dispose_thread(thread_t *thread)
{
	stop_thread(thread);
	dmap_rem_thr(thread);
}

