/*
* Event.cpp
*
*  Created on: 10.04.2011
*      Author: Endre Bock
*/

#include <Event>
#include <EventDispatcher>

using namespace u;

const String u::Event::COMPLETE("u::Event::complete");
const String u::Event::PROGRESS("u::Event::progress");

u::Event::Event(String const& type) : Object()
{
	_type = &type;
	_target = null;
}

u::Event::~Event()
{
	_target = null;
	_type = null;
}

EventDispatcher* u::Event::target()
{
	return _target;
}

const String* u::Event::type()
{
	return _type;
}

Event* u::Event::clone()
{
	return new Event(*_type);
}

String u::Event::toString()
{
	String str = "[" + className();
	str += " type=" + *_type;
	str += " target=" + (_target!=null?_target->toString():"null");
	str += "]";
	return str;
}

void u::Event::destroy()
{
	delete (Event*)this;
}

String u::Event::className()
{
	return String("u::Event");
}
