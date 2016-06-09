#ifndef INCGUARD_MPT_LIBRARY_THREADING_H_
#define INCGUARD_MPT_LIBRARY_THREADING_H_

#include "../inc/inc_predefs.h"
#include "lib_defs.h"
//#include "lib_descs.h"
#include <pthread.h>

#define MPT_USE_SPINLOCK

typedef pthread_spinlock_t    spin_t;
typedef pthread_mutex_t       mutex_t;
typedef pthread_cond_t        cond_t;
typedef pthread_t             threader_t;
typedef pthread_attr_t        threader_attr_t;
typedef pthread_rwlock_t      rwmutex_t;
typedef pthread_rwlockattr_t  rwmutex_attr_t;

typedef struct signal_struct_t
{
	mutex_t           *mutex;
	cond_t             cond;
	volatile bool_t    waiting;
	volatile int32_t   waiters;
}signal_t;

typedef struct barrier_struct_t
{
	uint32_t         gatenum;
	uint32_t         current;
	pthread_mutex_t *mutex;
	pthread_cond_t   cond;
}barrier_t;

typedef enum{
	THREAD_STATE_CONSTRUCTED = 1,
	THREAD_STATE_READY   = 2,
	THREAD_STATE_STARTED = 3,
	THREAD_STATE_RUN = 4,
	THREAD_STATE_STOP = 5,
	THREAD_STATE_STOPPED = 6,
}thread_state_t;

typedef struct thread_struct_t
{
	int32_t          thread_id;
	threader_t       handler;
	threader_attr_t  handler_attr;
	volatile thread_state_t state;
	void *(*process)(void*);
	void *arg;
} thread_t;

mutex_t* mutex_ctor();
void mutex_dtor(void *mutex);
void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);

spin_t* spin_ctor();
void spin_dtor(spin_t *spin);
void spin_lock(spin_t* mutex);
void spin_unlock(spin_t* mutex);

signal_t* signal_ctor();
void signal_dtor(signal_t *signal);
void signal_set(signal_t *signal);
void signal_setall(signal_t *signal);
void signal_wait(signal_t *signal);
void signal_lock(signal_t *signal);
void signal_unlock(signal_t *signal);
void signal_release(signal_t *signal);

barrier_t* barrier_ctor();
void barrier_dtor(barrier_t *barrier);
int barrier_wait(barrier_t *barrier);

rwmutex_t *rwmutex_ctor();
void rwmutex_dtor(void *rwmutex);
void rwmutex_read_lock(rwmutex_t *rwmutex);
void rwmutex_write_lock(rwmutex_t *rwmutex);
void rwmutex_read_unlock(rwmutex_t *rwmutex);
void rwmutex_write_unlock(rwmutex_t *rwmutex);


void thread_sleep(uint16_t ms);
void thread_usleep(uint16_t ms);
thread_t* thread_ctor();
void thread_dtor(void *thread);

void start_thread(thread_t*);
void stop_thread(thread_t*);
void setup_thread(thread_t*, void* (*)(void*), void*);

#endif //INCGUARD_MPT_LIBRARY_THREADING_H_
