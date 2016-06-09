#include "lib_threading.h"
#include "inc_texts.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "inc_unistd.h"
#include "lib_debuglog.h"

//#define LOCKS_LOGS_ENABLED

mutex_t* mutex_ctor()
{
	mutex_t* result;
	result = (mutex_t*) malloc(sizeof(mutex_t));
	BZERO(result, sizeof(mutex_t));
	//pthread_mutex_init(result , NULL);
	return result;
}

void mutex_dtor(void *mutex)
{
	mutex_t *target;
	if(mutex == NULL){
		return;
	}
	target = (mutex_t*) mutex;
	pthread_mutex_destroy(target);
}


void mutex_lock(mutex_t* mutex)
{
	debug_lockcall("mutex_lock");
	pthread_mutex_lock(mutex);
}

void mutex_unlock(mutex_t* mutex)
{
	debug_lockcall("mutex_unlock");
	pthread_mutex_unlock(mutex);
}

spin_t* spin_ctor()
{
	spin_t* result;
	result = (spin_t*) malloc(sizeof(spin_t));
	BZERO(result, sizeof(spin_t));
	pthread_spin_init(result, 0);
	return result;
}

void spin_dtor(spin_t *spin)
{
	spin_t *target;
	if(spin == NULL){
		return;
	}

	target = (spin_t*) spin;
	pthread_spin_destroy(target);
}


void spin_lock(spin_t* spin)
{
	debug_lockcall("spin_lock");
	pthread_spin_lock(spin);
}

void spin_unlock(spin_t* spin)
{
	debug_lockcall("spin_unlock");
	pthread_spin_unlock(spin);
}

signal_t* signal_ctor()
{
	signal_t* result;
	result = (signal_t*) malloc(sizeof(signal_t));
	BZERO(result, sizeof(signal_t));
	result->mutex = mutex_ctor();
	result->waiting = BOOL_FALSE;
	result->waiters = 0;
	pthread_cond_init (&(result->cond), NULL);
	return result;
}

void signal_dtor(signal_t *signal)
{
	signal_t *target;
	if(signal == NULL){
		return;
	}

	target = (signal_t*) signal;
	mutex_dtor(target->mutex);
	pthread_cond_destroy(&(target->cond));
	free(signal);
}

void signal_set(signal_t *signal)
{
	debug_lockcall("signal_set");
	pthread_cond_signal(&(signal->cond));
}

void signal_setall(signal_t *signal)
{
	debug_lockcall("signal_set");
	pthread_cond_broadcast(&signal->cond);
}

void signal_wait(signal_t *signal)
{
	debug_lockcall("signal_wait");
	++signal->waiters;
	signal->waiting = BOOL_TRUE;
	pthread_cond_wait(&(signal->cond), signal->mutex);
	signal->waiting = BOOL_FALSE;
	--signal->waiters;
}

void signal_lock(signal_t *signal)
{
	debug_lockcall("signal_lock");
	pthread_mutex_lock(signal->mutex);
}

void signal_unlock(signal_t *signal)
{
	debug_lockcall("signal_unlock");
	pthread_mutex_unlock(signal->mutex);
}

void signal_release(signal_t *signal)
{
	debug_lockcall("signal_release");
	if(pthread_mutex_trylock(signal->mutex) == EDEADLK){
		signal_unlock(signal);
	}
}

#define BARRIER_FLAG (1UL<<31)
barrier_t* barrier_ctor()
{
	barrier_t* result;
	result = (barrier_t*) malloc(sizeof(barrier_t));
	BZERO(result, sizeof(barrier_t));
	result->mutex = mutex_ctor();
	result->gatenum = 0;
	result->current = 0;
	pthread_cond_init (&(result->cond), NULL);
	return result;
}

void barrier_dtor(barrier_t *barrier)
{
	if(barrier == NULL){
		return;
	}

	pthread_mutex_lock(barrier->mutex);
	while (barrier->current > BARRIER_FLAG)
	{
		/* Wait until everyone exits the barrier */
		pthread_cond_wait(&barrier->cond, barrier->mutex);
	}
	pthread_mutex_unlock(barrier->mutex);

	mutex_dtor(barrier->mutex);
	pthread_cond_destroy(&(barrier->cond));
	free(barrier);
}

int barrier_wait(barrier_t *barrier)
{
	pthread_mutex_lock(barrier->mutex);

	while (barrier->current > BARRIER_FLAG){
		/* Wait until everyone exits the barrier */
		pthread_cond_wait(&barrier->cond, barrier->mutex);
	}

	/* Are we the first to enter? */
	if (barrier->current == BARRIER_FLAG){
		barrier->current = 0;
	}

	barrier->current++;
	if (barrier->current == barrier->gatenum){
		barrier->current += BARRIER_FLAG - 1;
		pthread_cond_broadcast(&barrier->cond);
		pthread_mutex_unlock(barrier->mutex);

		return PTHREAD_BARRIER_SERIAL_THREAD;
	}

	while (barrier->current < BARRIER_FLAG){
		/* Wait until enough threads enter the barrier */
		pthread_cond_wait(&barrier->cond, barrier->mutex);
	}

	barrier->current--;

	/* Get entering threads to wake up */
	if (barrier->current == BARRIER_FLAG){
		pthread_cond_broadcast(&barrier->cond);
	}

	pthread_mutex_unlock(barrier->mutex);

	return 0;
}

rwmutex_t *rwmutex_ctor()
{
	rwmutex_t* result;
	result = (rwmutex_t*) malloc(sizeof(rwmutex_t));
	BZERO(result, sizeof(rwmutex_t));
	pthread_rwlock_init(result, NULL);
	return result;
}

void rwmutex_dtor(void *rwmutex)
{
	rwmutex_t *target;
	if(rwmutex == NULL){
		return;
	}
	target = (rwmutex_t*) rwmutex;
	pthread_rwlock_destroy(target);
}

void rwmutex_read_lock(rwmutex_t *rwmutex)
{
	debug_lockcall("rwmutex_read_lock");
	pthread_rwlock_rdlock(rwmutex);
}

void rwmutex_write_lock(rwmutex_t *rwmutex)
{
	debug_lockcall("rwmutex_write_lock");
	pthread_rwlock_wrlock(rwmutex);
}

void rwmutex_read_unlock(rwmutex_t *rwmutex)
{
	debug_lockcall("rwmutex_read_unlock");
	pthread_rwlock_unlock(rwmutex);
}
void rwmutex_write_unlock(rwmutex_t *rwmutex)
{
	debug_lockcall("rwmutex_write_unlock");
	pthread_rwlock_unlock(rwmutex);
}



void thread_sleep(uint16_t ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
	
}


void thread_usleep(uint16_t us)
{
#ifdef _WIN32
	printf("thread_usleep is not implemented.");
#else
	usleep(us);
#endif

}


thread_t* thread_ctor()
{
	thread_t* result;
	result = (thread_t*) malloc(sizeof(thread_t));
	result->arg = NULL;
	result->process = NULL;
	result->state = THREAD_STATE_CONSTRUCTED;
	pthread_attr_init(&(result->handler_attr));
	pthread_attr_setdetachstate(&(result->handler_attr), PTHREAD_CREATE_JOINABLE);
	return result;
}

void setup_thread(thread_t* thread, void* (*process)(void*) , void* arg)
{
	if(thread == NULL || thread->state != THREAD_STATE_CONSTRUCTED){
		WARNINGPRINT("The desired thread you want to start is not exists or not in constructed state");
		return;
	}
	thread->process = process;
	thread->arg = arg;
	thread->state = THREAD_STATE_READY;
}

void start_thread(thread_t* thread)
{
	if(thread == NULL || thread->state != THREAD_STATE_READY){
		WARNINGPRINT("Thread start is called and the desired thread is not exists or not in ready state");
		return;
	}
	thread->state = THREAD_STATE_STARTED;
	thread->thread_id =
			pthread_create(
				&(thread->handler),       /*the thread handler*/
				&(thread->handler_attr),  /*the attribute of the thread handler*/
				thread->process,          /*the main process of the thread*/
				(void*)thread             /*the argument the thread main process will gets.*/
			);
}


void stop_thread(thread_t* thread)
{
	int32_t       wait = 0;
	const int32_t wait_limit = 10;
	if(thread == NULL || thread->state != THREAD_STATE_RUN ){
		WARNINGPRINT("Thread stop is called and the desired thread is not in run state");
		return;
	}
	thread->state = THREAD_STATE_STOP;
	while(thread->state != THREAD_STATE_STOPPED && wait++ < wait_limit){
		thread_sleep(50);
	}
	if(wait >= wait_limit){
		WARNINGPRINT("thread must be cancelled");
		pthread_cancel(thread->handler);
	}
	thread->state = THREAD_STATE_READY;
}

void thread_dtor(void *thread)
{
	DEBUGPRINT("thread dtor is called");
	thread_t* target;
	if(thread == NULL){
		return;
	}
	target = (thread_t*) thread;
	if(target->state == THREAD_STATE_RUN){
		stop_thread(target);
	}
	pthread_attr_destroy(&(target->handler_attr));
	target->process = NULL;
	target->thread_id = -1;
	free(target);
}
