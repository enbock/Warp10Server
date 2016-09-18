/*
 * NetEvent.cpp
 *
 *  Created on: 15.07.2011
 *      Author: Endre Bock
 */

#include <NetEvent>

using namespace u;

const String u::NetEvent::NEW_CONNECTION("u::NetEvent::new_connection");
const String u::NetEvent::CONNECTION_FAILED("u::NetEvent::connection_failed");
const String u::NetEvent::LISTENER("u::NetEvent::listener");
const String u::NetEvent::LISTENER_FAILED("u::NetEvent::listener_failed");
const String u::NetEvent::CLOSE("u::NetEvent::close");
const String u::NetEvent::CLOSED("u::NetEvent::closed");
const String u::NetEvent::CLOSE_REQUIRED("u::NetEvent::close_required");
const String u::NetEvent::CLOSE_CONFIRMED("u::NetEvent::close_confirmed");
const String u::NetEvent::IS_CONNECTION_READY("u::NetEvent::is_connection_ready");
const String u::NetEvent::CONNECTION_IS_READY("u::NetEvent::connection_is_ready");

u::NetEvent::NetEvent(String const& type, String address, int64 port,
	EventRoom* room)
		: Event(type)
{
	init();
	_addr = address;
	_port = port;
	_room = room;
}

u::NetEvent::NetEvent(const String& type, NetworkConnection* con)
		: Event(type)
{
	init();
	_con = con;
	_addr = con->address();
	_port = con->port();
	_room = con->room();
}

void u::NetEvent::init()
{
	_addr = "";
	_port = 0;
	_room = null;
	_con = null;
	closee = null;
}

u::NetEvent::~NetEvent()
{

}

String u::NetEvent::className()
{
	return "u::NetEvent";
}

void u::NetEvent::destroy()
{
	delete (NetEvent*) this;
}

Event* u::NetEvent::clone()
{
	NetEvent* event = new NetEvent(*_type, _addr, _port, _room);
	event->closee = this->closee;
	event->_con = this->_con;
	return event;
}

EventRoom* u::NetEvent::room()
{
	return _room;
}

String u::NetEvent::address()
{
	return _addr;
}

int64 u::NetEvent::port()
{
	return _port;
}

NetworkConnection* u::NetEvent::connection()
{
	return _con;
}
