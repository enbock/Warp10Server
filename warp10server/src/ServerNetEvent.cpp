/*
* ServerNetEvent.cpp
*
*  Created on: 15.07.2011
*      Author: Endre Bock
*/

#include <ServerNetEvent>

using namespace u;

const String u::ServerNetEvent::GET_CTRL_CONNECTION(
	"u::ServerNetEvent::get_control_connection"
);
const String u::ServerNetEvent::GET_WEB_CONNECTION(
	"u::ServerNetEvent::get_web_connection"
);
const String u::ServerNetEvent::GET_CTRL_LISTENER(
	"u::ServerNetEvent::get_control_listener"
);
const String u::ServerNetEvent::GET_WEB_LISTENER (
	"u::ServerNetEvent::get_web_listener"
);

u::ServerNetEvent::ServerNetEvent(String const& type, String address
	, int64 port, EventRoom* room) : NetEvent(type, address, port, room)
{

}

u::ServerNetEvent::~ServerNetEvent()
{

}

String u::ServerNetEvent::className()
{
	return "u::ServerNetEvent";
}

void u::ServerNetEvent::destroy()
{
	delete (ServerNetEvent*)this;
}

Event* u::ServerNetEvent::clone()
{
	return new ServerNetEvent(*_type, _addr, _port, _room);
}
