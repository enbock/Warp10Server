/*
* NetworkServicePlugin.cpp
*
*  Created on: 16.07.2011
*      Author: Endre Bock
*/

#include <NetworkServicePlugin>

using namespace u;

u::NetworkServicePlugin::NetworkServicePlugin() : RoomClient()
{
	_srv = null;
}

u::NetworkServicePlugin::~NetworkServicePlugin()
{
	//trace(className()+": Destroy");
}

String u::NetworkServicePlugin::className()
{
	return "u::NetworkServicePlugin";
}

void u::NetworkServicePlugin::service(NetworkService* value)
{
	_srv = value;
}

NetworkConnection* u::NetworkServicePlugin::createIncomingConnection(
	Socket* socket
)
{
#ifndef NDEBUG
	assert(true == false &&
		"This function need to override complete in a subclass.");
#endif
	return null;
}

NetworkService* u::NetworkServicePlugin::service()
{
	return _srv;
}



