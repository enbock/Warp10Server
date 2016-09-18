/*
* ServerEvent.cpp
*
*  Created on: 13.06.2011
*      Author: Endre Bock
*/


#include <ServerEvent>

using namespace u;

const String u::ServerEvent::SHUTDOWN("u::ServerEvent::shutdown");
const String u::ServerEvent::REGISTER_NETWORK_PLUGIN(
	"u::ServerEvent::registerNetworkPlugin"
);
const String u::ServerEvent::NETWORK_PLUGIN_REGISTERED(
	"u::ServerEvent::networkPluginRegistered"
);

u::ServerEvent::ServerEvent(String const& type) : Event(type)
{
	data = null;
}

u::ServerEvent::~ServerEvent()
{

}

Event *u::ServerEvent::clone()
{
	ServerEvent* event = new ServerEvent(*_type);
	event->data = data;
	return event;
}



void u::ServerEvent::destroy()
{
	delete (ServerEvent*)this;
}



String u::ServerEvent::className()
{
	return "u::ServerEvent";
}


