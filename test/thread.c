
#include <thread.h>

#ifdef _WIN32
#  include <windows.h>
#  include <process.h>

static unsigned __stdcall
thread_entry(void* argptr) {
	thread_arg* arg = argptr;
	arg->fn(arg->arg);
	return 0;
}

#else
#  include <pthread.h>
#  include <sched.h>

static void*
thread_entry(void* argptr) {
	thread_arg* arg = argptr;
	arg->fn(arg->arg);
	return 0;
}

#endif

uintptr_t
thread_run(thread_arg* arg) {
#ifdef _WIN32
	return _beginthreadex(0, 0, thread_entry, arg, 0, 0);
#else
	pthread_t id = 0;
	int err = pthread_create(&id, 0, thread_entry, arg);
	if (err)
		return 0;
	return (uintptr_t)id;
#endif
}

void
thread_join(uintptr_t handle) {
	if (!handle)
		return;
#ifdef _WIN32
	WaitForSingleObject((HANDLE)handle, INFINITE);
	CloseHandle((HANDLE)handle);
#else
	void* result = 0;
	pthread_join((pthread_t)handle, &result);
#endif
}

void
thread_sleep(int milliseconds) {
#ifdef _WIN32
	SleepEx(milliseconds, 1);
#else
	struct timespec ts;
	ts.tv_sec  = milliseconds / 1000;
	ts.tv_nsec = (long)(milliseconds % 1000) * 1000000L;
	nanosleep(&ts, 0);
#endif
}

void
thread_yield(void) {
#ifdef _WIN32
	Sleep(0);
	_ReadWriteBarrier();
#else
	sched_yield();
	__sync_synchronize();
#endif
}

void
thread_fence(void) {
#ifdef _WIN32
	_ReadWriteBarrier();
#else
	__sync_synchronize();
#endif
}
