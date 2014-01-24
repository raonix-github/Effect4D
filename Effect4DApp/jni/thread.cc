// 2013.03.01 bygomma
// this source from http://www.joinc.co.kr/modules/moniwiki/wiki.php/Site/Thread/Advanced/ThreadClass


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include "thread.h"

using namespace std;

long Thread::_id = 0;

pthread_attr_t Thread::_attr;

void* Thread::threadHandler(void* arg)
{
	signal(SIGUSR1, interruptHandler);
	Thread* thread = (Thread*) arg;
	try
	{
		thread->run();
	}
	catch (InterruptedException& e)
	{
		thread->_isInterrupted = true;
	}
	catch (...)
	{
	}
	thread->_state = -1;
	if (thread->_joinning)
	{
		pthread_mutex_lock(&thread->_mutex);
		pthread_cond_broadcast(&thread->_cond);
		pthread_mutex_unlock(&thread->_mutex);
	}
	return NULL;
}

void Thread::interruptHandler(int sigInt)
{
	throw InterruptedException("Thread interrupted");	
}

Thread::Thread()
{
	char name[32];
	sprintf(name, "Thread-%ld", _id);
	init(name);
}

Thread::Thread(const string& name)
{
	init(name);
}

Thread::~Thread()
{
	pthread_attr_destroy(&_attr);
	pthread_mutex_destroy(&_mutex);
	pthread_cond_destroy(&_cond);
}

void Thread::init(const string& name)
{
	_name = name;
	_joinning = false;
	_isInterrupted = false;
	pthread_mutex_init(&_mutex, NULL);
	pthread_cond_init(&_cond, NULL);
	_state = 0;
	pthread_mutex_lock(&_mutex);
	if (_id == 0)
	{
		pthread_attr_init(&_attr);
		pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
	}
	_id++;	
	pthread_mutex_unlock(&_mutex);
}

void Thread::start()
{
	if (_state == 1)
	{
		throw IllegalThreadStateException("Thread already started");
	}
	else if (_state == 0)
	{
		_state = 1;
		pthread_create(&_thread, &_attr, threadHandler, this);
	}
	else if (_state == -1)
	{
		throw IllegalThreadStateException("Thread had been started");
	}
}

void Thread::join()
{
	if (_state == 1)
	{
		pthread_mutex_lock(&_mutex);
		_joinning = true;
		pthread_cond_wait(&_cond, &_mutex);
		_joinning = false;
		pthread_mutex_unlock(&_mutex);
	}
}

void Thread::join(unsigned long time)
{
	if (_state == 1)
	{
		struct timeval now;
		struct timespec timeout;
		gettimeofday(&now, NULL);
		ldiv_t t = ldiv(time * 1000000, 1000000000);
		timeout.tv_sec = now.tv_sec + t.quot;
		timeout.tv_nsec = now.tv_usec * 1000 + t.rem;
		pthread_mutex_lock(&_mutex);
		_joinning = true;
		pthread_cond_timedwait(&_cond, &_mutex, &timeout);
		_joinning = false;
		pthread_mutex_unlock(&_mutex);
	}
}

void Thread::interrupt()
{
	pthread_kill(_thread, SIGUSR1);
}

bool Thread::isInterrupted()
{
	return _isInterrupted;
}

bool Thread::isAlive()
{
	return _state == 1;
}

const string& Thread::getName()
{
	return _name;
}

void Thread::setName(const string& name)
{
	_name = name;
}
