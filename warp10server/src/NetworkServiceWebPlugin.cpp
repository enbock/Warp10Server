
#include <NetworkServiceWebPlugin>
#include <NetworkWebDecoder>
#include <NetworkWebListener>
#include <NetworkWebConnection>
#include <ServerNetEvent>

using namespace u;

u::NetworkServiceWebPlugin::NetworkServiceWebPlugin()
		: NetworkServicePlugin()
{
}

u::NetworkServiceWebPlugin::~NetworkServiceWebPlugin()
{
	doDestruct();
}

String u::NetworkServiceWebPlugin::className()
{
	return "u::NetworkServiceWebPlugin";
}

void u::NetworkServiceWebPlugin::destroy()
{
	delete (NetworkServiceWebPlugin*) this;
}

NetworkListener* u::NetworkServiceWebPlugin::createNetworkListener(
	String address, int64 port)
{
	NetworkWebListener *listener = new NetworkWebListener();
	listener->socketAddress = &address;
	listener->socketPort = port;
	return listener;
}

NetworkConnection*
u::NetworkServiceWebPlugin::createIncomingConnection(Socket* socket)
{
	if (socket->descriptor() == -1)
	{
		error("u::NetworkServiceWebPlugin::createIncomingConnection() failed.");
		return null;
	}
	NetworkWebDecoder* decoder = new NetworkWebDecoder();
	NetworkConnection* con = new NetworkWebConnection(socket, decoder);
	return con;
}

NetworkConnection*
u::NetworkServiceWebPlugin::createConnection(String address, int64 port)
{
	Socket *socket = new Socket(AF_INET, address, port);
	if (socket->connect() == false)
	{
		error(className()+"::createConnection() failed.");
		socket->destroy();
		return null;
	}

	NetworkWebDecoder* decoder = new NetworkWebDecoder();
	NetworkConnection *con = new NetworkWebConnection(socket, decoder);
	return con;
}

void u::NetworkServiceWebPlugin::addListeners()
{
	//trace(className()+": addListeners()");
	_room->addEventListener(ServerNetEvent::GET_WEB_CONNECTION,
		Callback(this, cb_cast(&NetworkServiceWebPlugin::onGetWebConnection))
	);
	_room->addEventListener(ServerNetEvent::GET_WEB_LISTENER,
		Callback(this, cb_cast(&NetworkServiceWebPlugin::onGetWebListener))
	);
}

void u::NetworkServiceWebPlugin::removeListeners()
{
	//trace(className()+": removeListeners()");
	_room->removeEventListener(ServerNetEvent::GET_WEB_CONNECTION,
		Callback(this, cb_cast(&NetworkServiceWebPlugin::onGetWebConnection))
	);
	_room->removeEventListener(ServerNetEvent::GET_WEB_LISTENER,
		Callback(this, cb_cast(&NetworkServiceWebPlugin::onGetWebListener))
	);
}

void u::NetworkServiceWebPlugin::onGetWebConnection(Object* arg)
{
	ServerNetEvent * event = (ServerNetEvent*) arg;
	_srv->doGetConnection(event, this);
	event->destroy();
}

void u::NetworkServiceWebPlugin::onGetWebListener(Object* arg)
{
	ServerNetEvent * event = (ServerNetEvent*) arg;
	_srv->doGetListener(event, this);
	event->destroy();
}

