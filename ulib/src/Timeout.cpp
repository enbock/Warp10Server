/*
* Timeout.cpp
*
*  Created on: 04.01.2013
*      Author: Endre Bock
*/

#include <Timeout>
#include <Thread>
#include <sys/time.h>

using namespace u;

void u::Timeout::init(uint64 time, Callback* function, Callback* abort)
{
	_function = new Callback(*function);
	_abort    = null;
	_time     = getCurrentTime() + time;
	_run      = true;

	if(abort != null)
	{
		_abort = new Callback(*abort);
	}

	createThread(this, Timeout::tick);
}

u::Timeout::Timeout(uint64 time, Callback* function, Callback* abort)
{
	init(time, function, abort);
}

u::Timeout::Timeout(uint64 time, Callback* function)
{
	init(time, function, null);
}

uint64 u::Timeout::getCurrentTime()
{
	struct timeval tv;
	::gettimeofday(&tv, NULL);
	uint64 time = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	return time;
}

u::Timeout::~Timeout()
{
	lock();
	if(_abort != null) _abort->destroy();
	_abort = null;
	while(_run != false)
	{
		unlock();
		stop();
		usleep(150); // wait for possible thread
		lock();
	}
	unlock();
	stop();
	lock();
	_time = 0;
	if(_function != null) _function->destroy();
	_function = null;
	unlock();
}

void u::Timeout::destroy()
{
	delete (Timeout*)this;
}

String u::Timeout::toString()
{
	return "["+className()+":"
		+ " timeout=" + int2string(_time)
		+ " is running=" + (String)(_run == true ? "yes" : "no")
		+ "]"
	;
}

void u::Timeout::stop()
{
	lock();
	if(_run == false)
	{
		unlock();
		return;
	}
	_run = false;
	if(_abort != null)
	{
		ThreadSystem::create(_abort);
	}
	unlock();
}

void u::Timeout::tick(Object *arg)
{
	lock();
	if(_run == false)
	{
		unlock();
		return;
	}

	if(getCurrentTime() >= _time) {
		_run = false;
		unlock();
		ThreadSystem::create(_function);
		return;
	}
	unlock();
	usleep(100);
	createThread(this, Timeout::tick);
}

String u::Timeout::className()
{
	return "u::Timeout";
}
