/*
* EventDispatcher.cpp
*
*  Created on: 10.04.2011
*      Author: Endre Bock
*/

#include <EventDispatcher>

using namespace u;

EventDispatcher::_eventVector::_eventVector()
{
	type = null;
}

EventDispatcher::_eventVector::_eventVector(const _eventVector& value)
{
	type = value.type;
	list = value.list;
}


EventDispatcher::EventDispatcher() : Object()
{

}

EventDispatcher::EventDispatcher(EventDispatcher& value) : Object()
{
	operator=(value);
}

u::EventDispatcher::~EventDispatcher()
{
	int8 c = 0;
	lock();
	/*
	 * Be so long alive, so long any bindings exists.
	 * DO A CLEAN PROGRMMING!
	 */
	while(hasEventListener())
	{
#ifndef NDEBUG
		if(c == 0)
			error(
					className()
					+ ": Has "
					+ int2string(_eventList.length())
					+ " binder types."
			);
#endif
		int64 i, l;
		for(i = 0, l = _eventList.length(); i < l; i++)
		{
			_eventVector* vec = &_eventList.at(i);
#ifndef NDEBUG
			if(c == 0)
			{
				String
						out = className()
						      + ": Bind type "
						      + int2string(i)
						      + " is "
						      + (*vec->type)
						      + " with "
						      + int2string(vec->list.length())
						      + " binder"
						      + (vec->list.length() > 1 ? "s" : "")
						      + " on target";
#endif
				int64 vl, vi;
				for(vi = 0, vl = vec->list.length(); vi < vl; vi++)
				{
					// autoremove on invalid target
					if(!valid((Object*) &(vec->list.at(vi).target)))
					{
#ifndef NDEBUG
						trace(
								className()
								+ "::~EventDispatcher: Remove target "
								+ ptr2string(vec->list.at(vi).target)
								+ " from list "
								+ (*vec->type)
						);
#endif
						vec->list.erase(vi);
						vl = vec->list.length();
						if(vl > 0) vi--;
						if(vec->list.empty())
						{
							delEventVector(vec->type);
							l = _eventList.length();
							if(l > 0) i--;
							break;
						}
					}
#ifndef NDEBUG
					else
					{
						out += " "
						       + int2string(vi)
						       + ": "
						       + vec->list.at(vi).target->toString();
						if(vi + 1 != vl) out += " and";
					}
#endif
				}
#ifndef NDEBUG
				trace(out + ".");
			}
#endif
		}
		unlock();
#ifndef NDEBUG
		c++;
		if(c >= 90) c = 0;
#endif
		usleep(1000000 / FPS);
		lock();
	}
	unlock();
}

EventDispatcher& EventDispatcher::operator=(EventDispatcher& value)
{
	int64 i, l, j, m;
	_eventVector    * src;
	Vector<Callback>* dst;

	value.lock();
	lock();

	l     = value._eventList.size();
	for(i = 0; i < l; i++)
	{
		src   = &value._eventList.at(i);
		dst   = getEventVector(src->type);
		if(dst == null)
		{
			dst = addEventVector(src->type);
		}
		m     = src->list.size();
		for(j = 0; j < m; j++)
		{
			dst->push_back(src->list.at(j));
		}
	}

	unlock();
	value.unlock();
	return *this;
}

void EventDispatcher::addEventListener(
		const String& type
		, Callback callback
)
{
	int i, l;
	Vector<Callback>* vec;

	lock();

	vec = getEventVector(&type);
	if(vec == null)
	{
		vec = addEventVector(&type);
	}

	l = vec->size();

	for(i = 0; i < l; i++)
	{
		if(vec->at(i).target == callback.target)
		{
			unlock();
			return;
		}
	}

	vec->push_back(callback);
	unlock();
}

void EventDispatcher::removeEventListener(
		const String& type
		, Callback callback
)
{
	int i, l;
	Vector<Callback>* vec;

	//if(hasEventListener(type) == false) return;

	lock();

	vec = getEventVector(&type);
	if(vec == null)
	{
		unlock();
		return;
	}

	l = vec->size();

	for(i = 0; i < l; i++)
	{
		if(vec->at(i) == callback)
		{
			vec->erase(i);
			break;
		}
	}

	if(vec->empty()) delEventVector(&type);

	unlock();
}

bool EventDispatcher::hasEventListener(const String& type)
{
	lock();
	bool ret = getEventVector(&type) != null;
	unlock();

	return ret;
}

Event* EventDispatcher::dispatchEvent(Event* event)
{
	lock();

	Vector<Callback>* pVec = getEventVector(event->_type);
	if(pVec != null)
	{
		Vector<Callback> vec = *pVec;
		unlock();

		int64 i = 0, l = vec.size();
		for(i = 0; i < l; i++)
		{
			Callback cb = vec.at(i);
			trace(
					"DispatchEvent: " + *(event->_type)
					+ "\033[36m -> " + cb.toString()
			);
			Event* ev = event->clone();
			ev->_target = this;
			cb.arg      = ev;

			if(ThreadSystem::create(&cb) == false)
			{
				// thread system is terminating
				ev->destroy();
			}
		}

		return event;
	}
	else
	{
		trace(
				"\033[32mNo listener for " + *event->type()
				+ " on target " + toString() + "."
		);
	}
	unlock();
	return event;
}

Vector<Callback>* EventDispatcher::getEventVector(const String* type)
{
	int64 i, l;

	l     = _eventList.size();
	for(i = 0; i < l; i++)
	{
		_eventVector* entry = &_eventList.at(i);
		if(entry->type == type) return &(entry->list);
	}
	return null;
}

Vector<Callback>* EventDispatcher::addEventVector(const String* type)
{
	_eventVector vec;
	int64        i;

	vec.type = type;
	i = _eventList.size();
	_eventList.push_back(vec);
	return &(_eventList.at(i).list);
}

bool u::EventDispatcher::hasEventListener()
{
	return _eventList.length() > 0;
}

void EventDispatcher::delEventVector(const String* type)
{
	int64 i, l;

	l     = _eventList.size();
	for(i = 0; i < l; i++)
	{
		_eventVector* entry = &_eventList.at(i);
		if(entry->type == type)
		{
			_eventList.erase(i);
			return;
		}
	}
}

String EventDispatcher::className()
{
	return "u::EventDispatcher";
}

String EventDispatcher::toString()
{
	String str = "[" + className() + "]";
	return str;
}

void EventDispatcher::destroy()
{
	delete (EventDispatcher*) this;
}
