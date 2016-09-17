/*
 * NetworkService.cpp
 *
 *  Created on: 15.07.2011
 *      Author: Endre Bock
 */

#include <NetworkService>
#include <NetworkServicePlugin>
#include <NetEvent>

#ifndef FPS
#define FPS 40
#endif

using namespace u;

u::NetworkService::NetworkService()
		: RoomClient()
{

}

u::NetworkService::~NetworkService()
{
	doDestruct();
}

void u::NetworkService::doDestruct()
{
	lock();

	RoomClient::doDestruct();

	while (_plugins.length() > 0)
	{
		_plugins.pop()->destroy();
	}
	unlock();

	//RoomClient::doDestruct();

}

void u::NetworkService::destroy()
{
	delete (NetworkService*) this;
}

String u::NetworkService::className()
{
	return "u::NetworkService";
}

void u::NetworkService::addListeners()
{
	//trace(className()+": addListeners()");
	_room->addEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkService::onClose))
	);
}

void u::NetworkService::removeListeners()
{
	//trace(className()+": removeListeners()");
	_room->removeEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkService::onClose))
	);
}

void u::NetworkService::room(EventRoom* value)
{
	int64 i, l;

	RoomClient::room(value);

	l = _plugins.length();
	for (i = 0; i < l; i++)
		_plugins.at(i)->room(value);

}

void u::NetworkService::registerNetworkPlugin(NetworkServicePlugin* plugin)
{
	int64 i, l;


	// check, if already exists
	l = _plugins.length();
	for (i = 0; i < l; i++)
	{
		if (_plugins.at(i)->className() == plugin->className())
		{
			return;
		}
	}

	// add plugin
	_plugins.push(plugin);
	plugin->service(this);
	plugin->room(_room);
}

void u::NetworkService::removeNetworkPlugin(NetworkServicePlugin* plugin)
{
	int64 i, l;

	//trace(className()+": removeNetworkPlugin()");

	// check if exists
	l = _plugins.length();
	for (i = 0; i < l; i++)
	{
		if (_plugins.at(i) == plugin)
		{
			_plugins.erase(i);
			plugin->room(null);
			plugin->service(null);
		}
	}
}

void u::NetworkService::doGetListener(NetEvent *signal,
	NetworkServicePlugin* plugin)
{
	NetworkListener* listener = plugin->createNetworkListener(signal->address(),
		signal->port());

	lock();
	_listeners.push(listener);
	listener->room()->addEventListener(NetEvent::CLOSED,
		Callback(this, cb_cast(&NetworkService::onListenerDestroy))
	);
	unlock();

	if(listener->listen() == false)
	{
		listener->room()->dispatchEvent(new NetEvent(NetEvent::CLOSE))->destroy();
		_room->dispatchEvent(
			new NetEvent(NetEvent::LISTENER_FAILED, listener->address()
				, listener->port(), listener->room())
		)->destroy();
		return;
	}

	_room->dispatchEvent(
		new NetEvent(NetEvent::LISTENER, listener->address(), listener->port(),
			listener->room())
	)->destroy();

}

void u::NetworkService::onListenerDestroy(Object* arg)
{
	NetEvent *signal = (NetEvent*) arg;
	NetworkListener* listener = (NetworkListener*) signal->closee;
	lock();
	_listeners.erase(listener);
	unlock();


	listener->room()->removeEventListener(NetEvent::CLOSED,
		Callback(this, cb_cast(&NetworkService::onListenerDestroy))
	);
	signal->destroy();
	listener->destroy();
}

NetworkConnection*
u::NetworkService::doGetConnection(NetEvent* signal,
	NetworkServicePlugin* plugin)
{
	NetworkConnection* con = plugin->createConnection(signal->address(),
		signal->port());

	if (con != null) _room->dispatchEvent(
			new NetEvent(NetEvent::NEW_CONNECTION, con)
		)->destroy();
	else
		_room->dispatchEvent(
			new NetEvent(NetEvent::CONNECTION_FAILED)
		)->destroy();

	return con;
}

void u::NetworkService::onClose(Object* arg)
{
	int64 i,l;
	arg->destroy();
	trace(className()+"::onClose(): Shutdown listeners.");
	lock();
	for(i=0, l=_listeners.length(); i < l; i++)
	{
		_listeners.at(i)->room()->dispatchEvent(
			new NetEvent(NetEvent::CLOSE)
		)->destroy();
	}

	while(_listeners.length() > 0)
	{
		unlock();
		usleep(1000000 / FPS);
		lock();
	}
	unlock();

	NetEvent* signal = new NetEvent(NetEvent::CLOSED);
	signal->closee = this;
	_room->dispatchEvent(signal);
	signal->destroy();
}
