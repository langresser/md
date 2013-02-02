#pragma once
#include <config.h>
#include <mem/interface.h>
#include <logger/interface.h>
#include <assert.h>
#include <pthread.h>
#ifndef CONFIG_BASE_PS3
	#include <signal.h>
#endif

class ThreadPThread
{
public:
	bool running;

	constexpr ThreadPThread() : running(0), entry(0), arg(0), id(0) { }

	bool create(uint type, int (*entry)(void *), void *arg)
	{
		this->entry = entry;
		this->arg = arg;

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		if(type == 1)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		if(pthread_create(&id, &attr, wrapper, this) != 0)
		{
			logErr("error in pthread create");
			return 0;
		}
		logMsg("created pthread %d", (int)id);
		running = 1;
		return 1;
	}

	void join()
	{
		logMsg("joining pthread %d", (int)id);
		pthread_join(id, 0);
		running = 0;
	}

	void kill(int sig)
	{
		#ifndef CONFIG_BASE_PS3
			logMsg("sending signal %d to pthread %d", sig, (int)id);
			pthread_kill(id, sig);
		#else
			bug_exit("signals not supported");
		#endif
	}

	/*void cancel()
	{
		#ifndef CONFIG_BASE_PS3
		if(running)
		{
			logMsg("canceling pthread %d", (int)id);
			pthread_cancel(id);
			running = 0;
		}
		#else
			bug_exit("canceling thread not supported");
		#endif
	}*/
private:
	int (*entry)(void *);
	void *arg;
	pthread_t id;

	static void *wrapper(void *runData)
	{
		ThreadPThread *run = (ThreadPThread*)runData;
		//logMsg("running thread func %p", run->entry);
		run->entry(run->arg);
		run->running = 0;
		return 0;
	}
};

class MutexPThread
{
	bool init, locked;
public:
	pthread_mutex_t mutex; // TODO use accessor
	constexpr MutexPThread() : init(0), locked(0), mutex() { }
	bool create()
	{
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		//pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		if(pthread_mutex_init(&mutex, &attr) != 0 )
		{
			logErr("error in pthread mutex init");
			return 0;
		}
		init = 1;
		locked = 0;
		logMsg("init mutex %p", &mutex);
		return 1;
	}

	void destroy()
	{
		assert(!locked);
		if(init)
		{
			logMsg("destroy mutex %p", &mutex);
			pthread_mutex_destroy(&mutex);
			init = 0;
		}
	}

	bool lock()
	{
		assert(init);
		//logMsg("lock mutex %p", &mutex);
		if(pthread_mutex_lock(&mutex) < 0)
		{
			logErr("error in pthread_mutex_lock");
			return 0;
		}
		locked = 1;
		return 1;
	}

	bool unlock()
	{
		assert(init);
		//logMsg("unlock mutex %p", &mutex);
		if(pthread_mutex_unlock(&mutex) < 0)
		{
			logErr("error in pthread_mutex_unlock");
			return 0;
		}
		locked = 0;
		return 1;
	}
};

class CondVarPThread
{
	pthread_cond_t cond;
	pthread_mutex_t *mutex;
	bool init;
public:
	constexpr CondVarPThread() : cond(), mutex(0), init(0) { }
	bool create(MutexPThread *mutex = 0)
	{
		pthread_cond_init(&cond, 0);
		this->mutex = &mutex->mutex;
		init = 1;
		return 1;
	}

	void wait(MutexPThread *mutex = 0)
	{
		assert(mutex || this->mutex);
		pthread_mutex_t *waitMutex = mutex ? &mutex->mutex : this->mutex;
		pthread_cond_wait(&cond, waitMutex);
	}
};
