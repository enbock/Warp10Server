/*
* NetworkConnection
*
*  Created on: 17.07.2011
*      Author: Endre Bock
*/

#include <NetworkConnection>
#include <NetEvent>
#include <NetTransferEvent>

using namespace u;

u::NetworkConnection::NetworkConnection
(
		Socket* socket
		, NetworkDecoder* plugin
)
	: RoomOwner()
{
	timeout  = NETWORK_CONNECT_DEFAULT_TIMEOUT;
	_isClose = false;
	_socket  = socket;
	_decoder = plugin;
	_decoder->connection(this);
	room()->addEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkConnection::onClose))
	);
	room()->addEventListener(
		NetTransferEvent::INIT
		, Callback(this, cb_cast(&NetworkConnection::onTransferInit))
	);
}

u::NetworkConnection::~NetworkConnection()
{
	lock();
	room()->removeEventListener(
		NetTransferEvent::INIT
		, Callback(this, cb_cast(&NetworkConnection::onTransferInit))
	);
	room()->removeEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkConnection::onClose))
	);
	_decoder = null;
	_socket  = null;
	_isClose = true;
	unlock();
}

String u::NetworkConnection::address()
{
	return _socket->hostname();
}

int64 u::NetworkConnection::port()
{
	return _socket->port();
}

void u::NetworkConnection::onClose(Object* signal)
{
	signal->destroy();
	lock();
	_room.removeEventListener(
		NetEvent::CLOSE
		, Callback(this, cb_cast(&NetworkConnection::onClose))
	);
	_isClose = true;
	unlock();
}

void u::NetworkConnection::doClosed()
{
	NetEvent *signal = new NetEvent(
		NetEvent::CLOSED, address(), port(), room()
	);
	signal->closee = this;
	lock();
	_isClose = true;
	bool send = true;
	// I don't create that objects. Really destroy? Yes, the network plugin will not do that.
	if(_decoder != null && valid(_decoder))
	{
		_decoder->destroy();
		_decoder = null;
	}
	else
		send = false;

	if(_socket != null && valid(_socket))
	{
		_socket->destroy();
		_socket  = null;
	}
	else
		send = false;
	if(send == true) _room.dispatchEvent(signal);
	unlock();
	signal->destroy();
}

String u::NetworkConnection::className()
{
	return "u::NetworkConnection";
}

bool u::NetworkConnection::isClose()
{
	return _isClose;
}
