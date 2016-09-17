/*
* NetworkControlConnection.cpp
*
*  Created on: 06.08.2011
*      Author: Endre Bock
*/

#include <NetworkControlConnection>
#include <NetworkStandardDecoder>
#include <NetEvent>
#include <NetTransferEvent>

using namespace u;

u::NetworkControlConnection::NetworkControlConnection
(
		Socket* socket
		, NetworkDecoder* plugin
)
	: NetworkConnection(socket, plugin)
{
	_inAsk = false;
	_room.addEventListener(
		NetEvent::IS_CONNECTION_READY
		, Callback(this, cb_cast(&NetworkControlConnection::onReadyCheck))
	);
	_room.addEventListener(
		NetEvent::CLOSE_CONFIRMED
		, Callback(this, cb_cast(&NetworkControlConnection::onCloseConfirmed))
	);
	_room.addEventListener(
		NetTransferEvent::READY
		, Callback(this, cb_cast(&NetworkControlConnection::onTransferReady))
	);
	_room.addEventListener(
		NetTransferEvent::DATA
		, Callback(this, cb_cast(&NetworkControlConnection::onSendData))
	);
}

u::NetworkControlConnection::~NetworkControlConnection()
{
	lock();
	_room.removeEventListener(
		NetEvent::IS_CONNECTION_READY
		, Callback(this, cb_cast(&NetworkControlConnection::onReadyCheck))
	);
	_room.removeEventListener(
		NetEvent::CLOSE_CONFIRMED
		, Callback(this, cb_cast(&NetworkControlConnection::onCloseConfirmed))
	);
	_room.removeEventListener(
		NetTransferEvent::READY
		, Callback(this, cb_cast(&NetworkControlConnection::onTransferReady))
	);
	_room.removeEventListener(
		NetTransferEvent::DATA
		, Callback(this, cb_cast(&NetworkControlConnection::onSendData))
	);
	unlock();
}

void u::NetworkControlConnection::destroy()
{
	delete (NetworkControlConnection*)this;
}

String u::NetworkControlConnection::className()
{
	return "u::NetworkControlConnection";
}


void u::NetworkControlConnection::onClose(Object* signal)
{
	NetworkConnection::onClose(signal);
	lock();
	if(valid(_decoder)) ((NetworkStandardDecoder*)_decoder)->sendClose();
	unlock();
}

void u::NetworkControlConnection::doCloseRequired()
{
	lock();
	_isClose = true;
	unlock();

	_room.dispatchEvent(
		new NetEvent(
			NetEvent::CLOSE_REQUIRED
			, address()
			, port()
			, room()
		)
	)->destroy();
}

void u::NetworkControlConnection::onCloseConfirmed(Object* signal)
{
	lock();
	bool canSend = _isClose != false;
	unlock();
	if(canSend) ((NetworkStandardDecoder*)_decoder)->sendCloseConfirmed();

	signal->destroy();
}

void u::NetworkControlConnection::onTransferInit(Object* signal)
{
	int64 id = ((NetTransferEvent *)signal)->id();
	signal->destroy();
	lock();
	((NetworkStandardDecoder*)_decoder)->sendInit(id);
	unlock();
}

void u::NetworkControlConnection::doTransferInit(uint64 id)
{
	trace(className()+"::doTransferInit("+int2string(id)+")");
	/**
	 * Dev-Note: The control service !is! the server application. So we don't
	 *           need to inform other system about incomming transfer like
	 *           HTTP(?).
	 */
	lock();
	transferStack.push(id);
	unlock();
	_room.dispatchEvent(
		new NetTransferEvent(NetTransferEvent::READY, id)
	)->destroy();
}

void u::NetworkControlConnection::onTransferReady(Object* signal)
{
	NetTransferEvent* netEvent = ((NetTransferEvent*)signal);
	uint64 id = netEvent->id();
	netEvent->destroy();

	lock();
	if(transferStack.indexOf(id) != -1) // I am receiver?
	{
		((NetworkStandardDecoder*)_decoder)->sendTransferReady(id);
	}
	unlock();
}

void u::NetworkControlConnection::doTransfer(uint64 id)
{
	NetTransferEvent signal(NetTransferEvent::READY, id);
	_room.dispatchEvent(&signal);
}

void u::NetworkControlConnection::onSendDataRequest(Object* signal)
{
	// TODO Remove step.
}

void u::NetworkControlConnection::onSendData(Object* signal)
{
	NetTransferEvent* netEvent = ((NetTransferEvent*)signal);
	uint64 id = netEvent->id();

	lock();
	if(transferStack.indexOf(id) == -1) // I am sender?
	{
		((NetworkStandardDecoder*)_decoder)->sendData(id, netEvent->data());
	}
	unlock();
	netEvent->destroy();
}

void u::NetworkControlConnection::doReceivedData(uint64 id, ByteArray* data)
{
	NetTransferEvent signal(NetTransferEvent::DATA, id, data);
	room()->dispatchEvent(&signal);
}

void u::NetworkControlConnection::onGetDataRequest(Object* signal)
{
}

void u::NetworkControlConnection::onDataComplete(Object signal)
{
}

void u::NetworkControlConnection::onEOT(Object* signal)
{
}

void u::NetworkControlConnection::doEOT(uint64 id)
{
	//transferStack.erase(id); //TODO implement it & correct place?
}

void u::NetworkControlConnection::doReady()
{
	// Message ready to room
	lock();
	_room.dispatchEvent(
		new NetEvent(NetEvent::IS_CONNECTION_READY)
	)->destroy();
	unlock();
}

void u::NetworkControlConnection::onReadyCheck(Object *signal)
{
	signal->destroy();

	// try to avoid, that connection is already deleted, while onReadyCheck
	// thread starts.
	if( _socket == null || _decoder == null || !valid(this)) return;

	lock();
	if( _socket != null && _decoder != null
			&& _socket->isConnected() == true
			&& ((NetworkStandardDecoder*)_decoder)->isReady() == true)
	{
		_room.dispatchEvent(
			new NetEvent(NetEvent::CONNECTION_IS_READY, this)
		)->destroy();
	}
	unlock();
}

