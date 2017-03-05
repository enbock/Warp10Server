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
	trace(className() + "::~NetworkManager: Network is down.");
}

void u::NetworkManager::doDestruct()
{
	lock();

	RoomClient::doDestruct();

	while (_plugins.length() > 0)
	{
		trace(
			className() +"::doDestruct: Remove plugin "
			+ _plugins.at(0)->toString()
		);
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
	//trace(className()+": removeListeners()"+ptr2string(_room));
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
	NetworkListener* listener = plugin->createNetworkListener(
		event->address(), event->port()
	);

	lock();
	_listeners.push(listener);
	listener->room()->addEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&NetworkManager::onListenerDestroy))
	);
	unlock();

	if(listener->listen() == false)
	{
		listener->room()->dispatchEvent(new NetEvent(NetEvent::CLOSE))->destroy();
		_room->dispatchEvent(
			new NetEvent(
				NetEvent::LISTENER_FAILED
				, listener->address()
				, listener->port()
				, listener->room())
		)->destroy();
		return;
	}

	_room->dispatchEvent(
		new NetEvent(
			NetEvent::LISTENER
			, listener->address()
			, listener->port()
			, listener->room()
		)
	)->destroy();

}

void u::NetworkManager::onListenerDestroy(Object* arg)
{
	NetEvent *event = (NetEvent*) arg;
	NetworkListener* listener = (NetworkListener*) event->closee;
	event->destroy();

	String listenerClass = listener->className();
	trace(
		className() + "::onListenerDestroy: Remove " 
		+ listenerClass
		+ "("+int2string(_listeners.size())+")"
	);
	lock();
	int64 length = _listeners.size(), i;
	bool foundListener = false;

	for (i = 0; i < length; i++) {
		if (_listeners.at(i) == listener) {
			foundListener = true;
		}
	}

	if (foundListener == false) {
		trace(
			className() + "::onListenerDestroy: " 
			+ listenerClass + " already removed!"
		);
		unlock();
		return;
	}

	_listeners.erase(listener);
	int64 listenerSize = _listeners.size();
	listener->room()->removeEventListener(
		NetEvent::CLOSED,
		Callback(this, cb_cast(&NetworkManager::onListenerDestroy))
	);
	unlock();
	
	listener->destroy();
	trace(
		className() + "::onListenerDestroy: Removed " 
		+ listenerClass
		+ "("+int2string(listenerSize)+")"
	);
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
	trace(className()+"::onClose: Shutdown listeners.");
	lock();
	for(i=0, l=_listeners.length(); i < l; i++)
	{
		_listeners.at(i)->room()->dispatchEvent(
			new NetEvent(NetEvent::CLOSE)
		)->destroy();
	}

	while(_listeners.length() > 0)
	{
		trace(
			className() + "::onClose: Waiting for "
			+ int2string(_listeners.length()) + " listeners."
		);
		unlock();
		usleep(1000000 / FPS);
		lock();
	}
	unlock();

	trace(className() + "::onClose: Finalize manager closing.");
	NetEvent* event = new NetEvent(NetEvent::CLOSED);
	event->closee = this;
	_room->dispatchEvent(event)->destroy();
}
