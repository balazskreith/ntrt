

//if we want to use interrupting we must include a library called rt.
//in that case we need to define the following macro:

#ifdef REALTIME_LIBRARY_NEEDED
#ifndef INCGUARD_LIB_INTERRUPTING_H_
#define INCGUARD_LIB_INTERRUPTING_H_
#include "lib_descs.h"
#include "inc_unistd.h"
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/timex.h>
#include <sys/types.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <linux/ptp_clock.h>
#include <sys/timerfd.h>

#ifndef ADJ_SETOFFSET
#define ADJ_SETOFFSET 0x0100
#endif

#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

typedef struct interrupt_timer_struct_t
{
	struct sigevent   sigevent;
	struct itimerspec timeout;
	timer_t           timerid;
	//clockid_t         clkid;
}interrupt_timer_t;

typedef struct timeperiod_struct_t{
	uint64_t   s  : 34;
	uint64_t   ms : 10;
	uint64_t   us : 10;
	uint64_t   ns : 10;
}timeinterval_t;

typedef struct interrupt_struct_t
{
	int               signum;
	struct sigaction  action;
	bool_t            initial_block;
	sigset_t          mask;
	interrupt_timer_t timer;
	timeinterval_t    period;
	timeinterval_t    initial;
}interrupt_t;


typedef struct timer_struct_t
{
	int               fd;
	uint64_t          expired;
	timeinterval_t    period;
	timeinterval_t    initial;
	struct itimerspec new_value;
}timerfd_t;

void init_interrupt(interrupt_t *interrupt, int signum, bool_t initial_block);
void set_interrupt_sh(interrupt_t *interrupt, void (*handler)(int));
void set_interrupt_ph(interrupt_t *interrupt, void (*handler)(int));
void interrupt_block(interrupt_t *interrupt);
void interrupt_unblock(interrupt_t *interrupt);
void interrupt_wait(interrupt_t *interrupt);
void interrupt_waitandunblock(interrupt_t *interrupt);
void interrupt_waitandcallsh(interrupt_t *interrupt);

void interrupt_arm(interrupt_t *interrupt, clockid_t clkid);
void interrupt_disarm(interrupt_t *interrupt);
void set_timeinterval(timeinterval_t *timeinterval, int32_t s, int32_t ms, int32_t us, int32_t ns);

timerfd_t* timer_ctor();
void timer_dtor(timerfd_t *timer);
int32_t timer_wait(timerfd_t *timer);
void timer_arm(timerfd_t *timer);
void timer_disarm(timerfd_t *timer);

#endif /* INCGUARD_LIB_INTERRUPTING_H_ */
#endif /* REALTIME_LIBRARY_NEEDED*/
