/* we currently use the SDL threading abstraction, but later it would make
   sense to get rid of it together with the rest of the SDL. */
#include "sc_threads.h"
#include "SDL_thread.h"

sc_thread_t *
sc_new_thread(sc_threadfunc func, void *closure)
{
    SDL_Thread *rv = SDL_CreateThread((int (*)(void *))func, closure);
    return (sc_thread_t *)rv;
}

void
sc_thread_join(sc_thread_t *thread, int *return_value)
{
    SDL_WaitThread((SDL_Thread *)thread, return_value);
}

uint32_t
sc_thread_get_id(sc_thread_t *thread)
{
    return SDL_GetThreadID((SDL_Thread *)thread);
}

void
sc_thread_kill(sc_thread_t *thread)
{
    SDL_KillThread((SDL_Thread *)thread);
}

sc_mutex_t *
sc_new_mutex(void)
{
    return (sc_mutex_t *)SDL_CreateMutex();
}

void
sc_mutex_acquire(sc_mutex_t *mutex)
{
    if (SDL_mutexP((SDL_mutex *)mutex) != 0)
        sc_critical_error(SC_ETHREAD, __FILE__, __LINE__,
                          "Unable to acquire mutex. (%s)", SDL_GetError());
}

void
sc_mutex_release(sc_mutex_t *mutex)
{
    if (SDL_mutexV((SDL_mutex *)mutex) != 0)
        sc_critical_error(SC_ETHREAD, __FILE__, __LINE__,
                          "Unable to release mutex. (%s)", SDL_GetError());
}

void
sc_free_mutex(sc_mutex_t *mutex)
{
    if (!mutex)
        return;
    SDL_DestroyMutex((SDL_mutex *)mutex);
}

sc_cond_t *
sc_new_cond(void)
{
    return (sc_cond_t *)SDL_CreateCond();
}

void
sc_free_cond(sc_cond_t *cond)
{
    if (!cond)
        return;
    SDL_DestroyCond((SDL_cond *)cond);
}

void
sc_cond_signal(sc_cond_t *cond)
{
    if (SDL_CondSignal((SDL_cond *)cond) != 0)
        sc_critical_error(SC_ETHREAD, __FILE__, __LINE__,
                          "Could not signal condition. (%s)", SDL_GetError());
}

void
sc_cond_broadcast(sc_cond_t *cond)
{
    if (SDL_CondBroadcast((SDL_cond *)cond) != 0)
        sc_critical_error(SC_ETHREAD, __FILE__, __LINE__,
                          "Could not broadcast condition. (%s)", SDL_GetError());
}

void
sc_cond_wait(sc_cond_t *cond, sc_mutex_t *mutex)
{
    if (SDL_CondWait((SDL_cond *)cond, (SDL_mutex *)mutex) != 0)
        sc_critical_error(SC_ETHREAD, __FILE__, __LINE__,
                          "Could not wait on condition. (%s)", SDL_GetError());
}
