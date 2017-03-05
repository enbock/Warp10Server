/*
* NetworkListener.cpp
*
*  Created on: 17.07.2011
*      Author: Endre Bock
*/

#include <NetworkListener>
#include <NetEvent>

using namespace u;

u::NetworkListener::NetworkListener() : RoomOwner()
{
	_room.addEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkListener::onClose))
	);
	_socket = null;
}

bool u::NetworkListener::listen()
{
	_socket = createSocket();
	bool ret = _socket->listen();

	if(ret == false) return false;

	// start incoming connections
	Callback cb(this, cb_cast(&NetworkListener::doListening));
	ThreadSystem::create(&cb);

	return true;
}

u::NetworkListener::~NetworkListener()
{
	_room.removeEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkListener::onClose))
	);
	trace(className()+"::~: Destroyed");
}

void u::NetworkListener::onClose(Object* arg)
{
	arg->destroy();
	// wait for possible other actions
	lock();

	// first stop listening
	if(_socket != null)
	{
		_socket->close();
		_socket->destroy();
	}

	// destroy/interrupt active connections
	int64 i,l;
	for(i=0, l=_connections.length(); i<l; i++)
	{
		NetworkConnection* nc = _connections.at(i);
		NetEvent closeEvent(NetEvent::CLOSE);
		nc->room()->dispatchEvent(&closeEvent);
	}

	// Wait for destroing connections
	while(_connections.length())
	{
		trace(
			className() + "::onClose: Waiting for "
			+ int2string(_connections.length()) + " connections."
		);
		unlock();
		usleep(1000000/FPS);
		lock();
	}

	// send CLOSED event
	NetEvent* event = new NetEvent(NetEvent::CLOSED);
	event->closee = this;
	_room.dispatchEvent(event);
	event->destroy();

	unlock();
}

void u::NetworkListener::doListening(Object* arg)
{
	lock();
	Socket* listener = _socket;
	unlock();

	if(!valid(listener)) return;
	Socket* con = listener->accept();

	lock();
	if(con == null)
	{
		// listening interrupted...socket closed?
		/**
		* Note: Maybe we need to create some exception handling or validating?
		* No, that's on close normal.
		* ...Or not!
		*/
		if(valid(_socket) && _socket->isConnected())
		{
			error(className()+": Error on listening.");
		}
		else
		{
			unlock();
			return;
		}
	}
	else
	{
		NetworkConnection *nc = createIncomingConnection(con);
		if(nc != null)
		{
			_connections.push(nc);

			nc->room()->addEventListener(
				NetEvent::CLOSED
				, Callback(this, cb_cast(&NetworkListener::onConnectionDestroy))
			);

			_room.dispatchEvent(
				new NetEvent(
					NetEvent::NEW_CONNECTION
					, nc->address()
					, nc->port()
					, nc->room()
				)
			)->destroy();
		}
		else
		{
			error(className()+"::doListening() Incomming connection not etablishable.");
			con->destroy();
		}
	}

	//self start ;)
	Callback cb(this, cb_cast(&NetworkListener::doListening));
	ThreadSystem::create(&cb);

	unlock();
}

String u::NetworkListener::className()
{
	return "u::NetworkListener";
}

void u::NetworkListener::onConnectionDestroy(Object* arg)
{
	NetEvent *event = (NetEvent*)arg;
	NetworkConnection *nc = (NetworkConnection*)event->closee;

	nc->room()->removeEventListener(NetEvent::CLOSED
		, Callback(this, cb_cast( &NetworkListener::onConnectionDestroy))
	);

	lock();
	_connections.erase(nc);
	unlock();
	event->destroy();

	nc->destroy();
}

String u::NetworkListener::address()
{
	return _socket->hostname();
}

int64 u::NetworkListener::port()
{
	return _socket->port();
}


