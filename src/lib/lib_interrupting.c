#define REALTIME_LIBRARY_NEEDED

#ifdef REALTIME_LIBRARY_NEEDED

#include "lib_interrupting.h"
#include <pthread.h>
#include <signal.h>

void init_interrupt(interrupt_t *interrupt, int signum, bool_t initial_block)
{
	interrupt->signum = signum;
	interrupt->initial_block = initial_block;
}

void set_interrupt_sh(interrupt_t *interrupt, void (*handler)(int))
{
	struct sigaction *action = &interrupt->action;
	int signum = interrupt->signum;
	bool_t initial_block = interrupt->initial_block;
	interrupt->signum = signum;

	if(initial_block == BOOL_TRUE){
		interrupt_block(interrupt);
	}else{
		interrupt_unblock(interrupt);
	}
	/* Install the signal handler. */
	action->sa_handler = handler;
	action->sa_flags = 0;
	sigemptyset(&action->sa_mask);
	sigaction(signum, action, NULL);

	return;
}



void set_timeinterval(timeinterval_t *timeinterval, int32_t s, int32_t ms, int32_t us, int32_t ns)
{
	timeinterval->s = s;
	timeinterval->ms = ms;
	timeinterval->us = us;
	timeinterval->ns = ns;
}

void set_interrupt_ph(interrupt_t *interrupt, void (*handler)(int))
{

}

__always_inline
void interrupt_block(interrupt_t *interrupt)
{
	int signum = interrupt->signum;
	sigset_t *mask = &interrupt->mask;

	/* Block the signal */
   sigemptyset(mask);
   sigaddset(mask, signum);
   //sigsetmask(SIG_BLOCK, mask);
   pthread_sigmask(SIG_BLOCK, mask, NULL);
   //sigprocmask(SIG_BLOCK, mask, NULL);
}

__always_inline
void interrupt_unblock(interrupt_t *interrupt)
{
	int signum = interrupt->signum;
	sigset_t *mask = &interrupt->mask;

	/* Unblock the signal. */
	sigemptyset(mask);
	sigaddset(mask, signum);
	//sigsetmask(SIG_UNBLOCK, mask, 0);
	pthread_sigmask(SIG_UNBLOCK, mask, NULL);
	//sigprocmask(SIG_UNBLOCK, mask, NULL);

}

void interrupt_arm(interrupt_t *interrupt, clockid_t clkid)
{
	interrupt_timer_t *interrupt_timer = &interrupt->timer;
	struct sigevent *sigevent = &interrupt_timer->sigevent;
	struct itimerspec *timeout = &interrupt_timer->timeout;
	timer_t *timerid = &interrupt_timer->timerid;
	int signum = interrupt->signum;
	timeinterval_t *period = &interrupt->period;
	timeinterval_t *initial = &interrupt->initial;

	/* Create a timer. */
	sigevent->sigev_notify = SIGEV_SIGNAL;
	sigevent->sigev_signo = signum;
	sigevent->sigev_value.sival_ptr = timerid;
	timer_create(clkid, sigevent, timerid);

	/* Start the timer. */
	timeout->it_interval.tv_sec = period->s;
	timeout->it_interval.tv_nsec = period->ms * 1000 * 1000 + period->us * 1000 + period->ns;
	timeout->it_value.tv_sec = initial->s;
	timeout->it_value.tv_nsec = initial->ms * 1000 * 1000 + initial->us * 1000 + initial->ns;
	timer_settime(*timerid, 0, timeout, NULL);
}


void interrupt_disarm(interrupt_t *interrupt)
{
	interrupt_timer_t *interrupt_timer = &interrupt->timer;
	timer_delete(interrupt_timer->timerid);
}

__always_inline
void interrupt_wait(interrupt_t *interrupt)
{
	sigwait(&interrupt->mask, &interrupt->signum);
}

__always_inline
void interrupt_waitandcallsh(interrupt_t *interrupt)
{
	sigwait(&interrupt->mask, &interrupt->signum);
	interrupt->action.sa_handler(interrupt->signum);
}

__always_inline
void interrupt_waitandunblock(interrupt_t *interrupt)
{
	sigwait(&interrupt->mask, &interrupt->signum);
	interrupt_unblock(interrupt);
}


timerfd_t* timer_ctor()
{
	timerfd_t* result;
	result = (timerfd_t*) malloc(sizeof(timerfd_t));
	memset(result, 0, sizeof(timerfd_t));
	return result;
}

void timer_dtor(timerfd_t *timer)
{
	free(timer);
}

int32_t timer_wait(timerfd_t *timer)
{
	return read(timer->fd, &timer->expired, sizeof(uint64_t));
}

void timer_arm(timerfd_t *timer)
{
	//struct itimerspec new_value;
	struct timespec now;

	clock_gettime(CLOCK_REALTIME, &now);

   /* Create a CLOCK_REALTIME absolute timer with initial
	  expiration and interval as specified in command line */

   timer->new_value.it_value.tv_sec = now.tv_sec + timer->initial.s;
   timer->new_value.it_value.tv_nsec = now.tv_nsec + (timer->initial.ms * 1000 * 1000 + timer->initial.us * 1000 + timer->initial.ns);
   timer->new_value.it_interval.tv_sec = timer->period.s;
   timer->new_value.it_interval.tv_nsec = timer->period.ms * 1000 * 1000 + timer->period.us * 1000 + timer->period.ns;

   timer->fd = timerfd_create(CLOCK_REALTIME, 0);
   timerfd_settime(timer->fd, TFD_TIMER_ABSTIME, &timer->new_value, NULL);
}

void timer_disarm(timerfd_t *timer)
{
	struct itimerspec new_value;
	new_value.it_value.tv_sec = 0;
	new_value.it_value.tv_nsec = 0;
	new_value.it_interval.tv_sec = 0;
	new_value.it_interval.tv_nsec = 0;

	timerfd_settime(timer->fd, TFD_TIMER_ABSTIME, &new_value, NULL);
}

#endif /* REALTIME_LIBRARY_NEEDED */
