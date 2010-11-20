/* threading abstraction */
#ifndef _INC_SC_THREADS_H_
#define _INC_SC_THREADS_H_

#include "sc_boot.h"

struct sc_thread;
typedef struct sc_thread sc_thread_t;

struct sc_mutex;
typedef struct sc_mutex sc_mutex_t;

struct sc_cond;
typedef struct sc_cond sc_cond_t;

typedef int (*sc_threadfunc)(void *closure);

/* creates a new thread */
sc_thread_t *sc_new_thread(sc_threadfunc func, void *closure);

/* waits for a thread */
void sc_thread_join(sc_thread_t *thread, int *return_value);

/* return the id of the thread */
uint32_t sc_thread_get_id(sc_thread_t *thread);

/* kills a thread */
void sc_thread_kill(sc_thread_t *thread);

/* creates a new mutex */
sc_mutex_t *sc_new_mutex(void);

/* destroy the mutex */
void sc_free_mutex(sc_mutex_t *mutex);

/* lock the mutex */
void sc_mutex_acquire(sc_mutex_t *mutex);

/* unlock the mutex */
void sc_mutex_release(sc_mutex_t *mutex);

/* creates a condition variable */
sc_cond_t *sc_new_cond(void);

/* destroys a condition variable */
void sc_free_cond(sc_cond_t *cond);

/* restarts one of the threads waiting for the condition */
void sc_cond_signal(sc_cond_t *cond);

/* restarts all threads that are waiting for the condition */
void sc_cond_broadcast(sc_cond_t *cond);

/* waits for the condition variable, waiting on the mutex passed.
   That mutex must be locked before entering the function and it
   will be re-locked later. */
void sc_cond_wait(sc_cond_t *cond, sc_mutex_t *mutex);

#endif
