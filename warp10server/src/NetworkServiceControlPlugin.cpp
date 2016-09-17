/*
 * NetworkServiceControlPlugin.cpp
 *
 *  Created on: 18.07.2011
 *      Author: Endre Bock
 */

#include <NetworkServiceControlPlugin>
#include <NetworkControlListener>
#include <NetworkControlConnection>
#include <NetworkStandardDecoder>
#include <ServerNetEvent>
#include <sys/poll.h>

using namespace u;

u::NetworkServiceControlPlugin::NetworkServiceControlPlugin()
		: NetworkServicePlugin()
{
}

u::NetworkServiceControlPlugin::~NetworkServiceControlPlugin()
{
	doDestruct();
}

String u::NetworkServiceControlPlugin::className()
{
	return "u::NetworkServiceControlPlugin";
}

void u::NetworkServiceControlPlugin::destroy()
{
	delete (NetworkServiceControlPlugin*) this;
}

NetworkListener* u::NetworkServiceControlPlugin::createNetworkListener(
	String address, int64 port)
{
	NetworkControlListener *listener = new NetworkControlListener();
	listener->socketFile = &address;
	return listener;
}

NetworkConnection*
u::NetworkServiceControlPlugin::createIncomingConnection(Socket* socket)
{
	if (socket->descriptor() == -1)
	{
		error("u::NetworkServiceControlPlugin::createIncomingConnection() failed.");
		return null;
	}
	NetworkStandardDecoder* decoder = new NetworkStandardDecoder();
	NetworkConnection* con = new NetworkControlConnection(socket, decoder);
	return con;
}

NetworkConnection*
u::NetworkServiceControlPlugin::createConnection(String address, int64 port)
{
	Socket *socket = new Socket(PF_LOCAL, address);
	if (socket->connect() == false)
	{
		error(className()+"::createConnection() failed.");
		socket->destroy();
		return null;
	}

	NetworkStandardDecoder* decoder = new NetworkStandardDecoder();
	NetworkConnection *con = new NetworkControlConnection(socket, decoder);
	//Is it a good idea to start handshake direct on decoder?
	// Yes, only the server side has to start the handshake.
	Callback cb(decoder, cb_cast(&NetworkStandardDecoder::startHandshake));
	ThreadSystem::create(&cb);

	return con;
}

void u::NetworkServiceControlPlugin::addListeners()
{
	//trace(className()+": addListeners()");
	_room->addEventListener(ServerNetEvent::GET_CTRL_CONNECTION,
		Callback(this, cb_cast(&NetworkServiceControlPlugin::onGetCtrlConnection))
	);
	_room->addEventListener(ServerNetEvent::GET_CTRL_LISTENER,
		Callback(this, cb_cast(&NetworkServiceControlPlugin::onGetCtrlListener))
	);
}

void u::NetworkServiceControlPlugin::removeListeners()
{
	//trace(className()+": removeListeners()");
	_room->removeEventListener(ServerNetEvent::GET_CTRL_CONNECTION,
		Callback(this, cb_cast(&NetworkServiceControlPlugin::onGetCtrlConnection))
	);
	_room->removeEventListener(ServerNetEvent::GET_CTRL_LISTENER,
		Callback(this, cb_cast(&NetworkServiceControlPlugin::onGetCtrlListener))
	);
}

void u::NetworkServiceControlPlugin::onGetCtrlConnection(Object* arg)
{
	ServerNetEvent * signal = (ServerNetEvent*) arg;
	_srv->doGetConnection(signal, this);
	signal->destroy();
}

void u::NetworkServiceControlPlugin::onGetCtrlListener(Object* arg)
{
	ServerNetEvent * signal = (ServerNetEvent*) arg;
	_srv->doGetListener(signal, this);
	signal->destroy();
}

