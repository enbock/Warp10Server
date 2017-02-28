/*
 * NetworkManager.cpp
 *
 *  Created on: 15.07.2011
 *      Author: Endre Bock
 */

#include <NetworkManager>
#include <NetworkServicePlugin>
#include <NetEvent>

#ifndef FPS
#define FPS 40
#endif

using namespace u;

u::NetworkManager::NetworkManager()
		: RoomClient()
{

}

u::NetworkManager::~NetworkManager()
{
	doDestruct();
}

void u::NetworkManager::doDestruct()
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

void u::NetworkManager::destroy()
{
	delete (NetworkManager*) this;
}

String u::NetworkManager::className()
{
	return "u::NetworkManager";
}

void u::NetworkManager::addListeners()
{
	//trace(className()+": addListeners()");
	_room->addEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkManager::onClose))
	);
}

void u::NetworkManager::removeListeners()
{
	//trace(className()+": removeListeners()");
	_room->removeEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkManager::onClose))
	);
}

void u::NetworkManager::room(EventRoom* value)
{
	int64 i, l;

	RoomClient::room(value);

	l = _plugins.length();
	for (i = 0; i < l; i++)
		_plugins.at(i)->room(value);

}

void u::NetworkManager::registerNetworkPlugin(NetworkServicePlugin* plugin)
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

void u::NetworkManager::removeNetworkPlugin(NetworkServicePlugin* plugin)
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

void u::NetworkManager::doGetListener(NetEvent *event,
	NetworkServicePlugin* plugin)
{
	NetworkListener* listener = plugin->createNetworkListener(event->address(),
		event->port());

	lock();
	_listeners.push(listener);
	listener->room()->addEventListener(NetEvent::CLOSED,
		Callback(this, cb_cast(&NetworkManager::onListenerDestroy))
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

void u::NetworkManager::onListenerDestroy(Object* arg)
{
	NetEvent *event = (NetEvent*) arg;
	NetworkListener* listener = (NetworkListener*) event->closee;
	lock();
	_listeners.erase(listener);
	unlock();


	listener->room()->removeEventListener(NetEvent::CLOSED,
		Callback(this, cb_cast(&NetworkManager::onListenerDestroy))
	);
	event->destroy();
	listener->destroy();
}

NetworkConnection*
u::NetworkManager::doGetConnection(NetEvent* event,
	NetworkServicePlugin* plugin)
{
	NetworkConnection* con = plugin->createConnection(event->address(),
		event->port());

	if (con != null) _room->dispatchEvent(
			new NetEvent(NetEvent::NEW_CONNECTION, con)
		)->destroy();
	else
		_room->dispatchEvent(
			new NetEvent(NetEvent::CONNECTION_FAILED)
		)->destroy();

	return con;
}

void u::NetworkManager::onClose(Object* arg)
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

	NetEvent* event = new NetEvent(NetEvent::CLOSED);
	event->closee = this;
	_room->dispatchEvent(event);
	event->destroy();
}
