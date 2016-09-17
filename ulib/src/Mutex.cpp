/*
* Mutex.cpp.
* $Id: $
*/

#include <Object>
#ifndef NDEBUG
#include <assert.h>
#endif

using namespace u;

Mutex::Mutex()
{
	_isLocked = false;
	int ret = pthread_mutex_init(&_mutex, NULL);
#ifndef NDEBUG
	assert(ret == 0);
#endif
}

Mutex::~Mutex()
{
	if(_isLocked == true) unlock();
	int ret = pthread_mutex_destroy(&_mutex);
#ifndef NDEBUG
	assert(ret == 0);
#endif
}

void Mutex::destroy()
{
	delete (Mutex*)this;
}

bool Mutex::lock()
{
	int ret = pthread_mutex_lock(&_mutex);
	return ret == 0;
}

bool Mutex::tryLock()
{
	int ret = pthread_mutex_trylock(&_mutex);
	if(ret == 0) _isLocked = true;
	return ret == 0;
}

void Mutex::unlock()
{
	int ret = pthread_mutex_unlock(&_mutex);
#ifndef NDEBUG
	assert(ret == 0);
#endif
}

String Mutex::className()
{
	return "u::Mutex";
}

void u::Mutex::doDestruct()
{
}
