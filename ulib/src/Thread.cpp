/*
* Thread.cpp
*
*  Created on: 06.04.2011
*      Author: Endre Bock
*/

#include <Thread>

using namespace u;

bool ThreadSystem::isTerminating = false;
int64 ThreadSystem::threadLimit = MAXTHREADS;
Mutex ThreadSystem::_access;
Vector<Thread*> ThreadSystem::_build;
Vector<Thread*> ThreadSystem::_run;
#ifndef NDEBUG
int64 ThreadSystem::finish = 0;
int64 ThreadSystem::peakRun = 0;
#endif

Thread::Thread(Callback *cb)
{
	_thread = 0;
	_call = *cb; //copy
	pthread_attr_init(&_attr);
	pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
}

Thread::~Thread()
{
	//pthread_detach(_thread);
	pthread_attr_destroy(&_attr);
}

void Thread::run()
{
	pthread_create(&_thread, &_attr, execute, (void *)this);
}

void* Thread::execute(void *thread)
{
	Thread *t = (Thread*)thread;
	t->_call.call();
	ThreadSystem::destroy(t);
	return 0;
}

bool ThreadSystem::create(Callback *cb)
{
	if(isTerminating) return false;

	Thread *t = new Thread(cb);

	_access.lock();
	_build.push_back(t);
	_access.unlock();

	return true;
}

void ThreadSystem::destroy(Thread *thread)
{
	int64 i,l;
	_access.lock();
	l = _run.size();
	for(i=0; i<l; i++)
	{
		if(_run.at(i) == thread)
		{
			_run.erase(i);
#ifndef NDEBUG
			finish++;
			if(ThreadSystem::isTerminating)
				trace("Run queue="+Object::int2string(_build.size())
				+ "\trun="+Object::int2string(_run.size())
				+ "(peak:"+Object::int2string(peakRun)+")"
				+ "\tfinished="+Object::int2string(finish)
				);
#endif
			_access.unlock();
			delete thread;
			return;
		}
	}
	_access.unlock();
#ifndef NDEBUG
	error("A deleting thread was not found in the run queue.");
	assert(false==true);
#endif
	//delete thread;
}

void ThreadSystem::destroy()
{
	Thread *t;
	while(!_build.empty())
	{
		t = _build.at(0);
		_build.erase((int64)0);
		delete t;
	}
	while(!_run.empty())
	{
		t = _run.at(0);
		_run.erase((int64)0);
		delete t;
	}
}

int64 ThreadSystem::numThreads()
{
	_access.lock();
	int64 num = _build.size();
	num += _run.size();
	_access.unlock();
	return num;
}

int64 ThreadSystem::numQueuedThreads()
{
	_access.lock();
	int64 num = _build.size();
	_access.unlock();
	return num;
}

void ThreadSystem::run(int64 id)
{
	_access.lock();

#ifndef NDEBUG
	if(peakRun < _run.size()) peakRun = _run.size();
	/*if(ThreadSystem::isTerminating)
		trace(String("                                                                                        ")
		+ "\n\033[A"
		+ "\033[33mRun queue="+Object::int2string(_build.size())
		+ "\trun="+Object::int2string(_run.size())
		+ "("+Object::int2string(peakRun)+")"
		+ "\tfinished="+Object::int2string(finish)
		+ "\033[A"
		);*/
#endif
	Thread *thread = _build.at(id);
	_build.erase(id);
	_run.push_back(thread);
	_access.unlock();
	thread->run();
}

void ThreadSystem::tick()
{
	_access.lock();

	while(threadLimit > _run.size() && _build.empty() == false)
	{
		_access.unlock();
		run(0);
		_access.lock();
	}

	_access.unlock();
}

void ThreadSystem::shutdown()
{
	_access.lock();
	isTerminating = true;
	_access.unlock();
}

void ThreadSystem::init()
{
	// ups...nothing to do?..ok, function for later stuff ;)
}

