/*
* NetworkTransferHandler.cpp
*
*  Created on: 04.05.2013
*      Author: Endre Bock
*/

#include <NetworkTransferHandler>
#include <NetEvent>

using namespace u;

uint64 u::NetworkTransferHandler::nextTransferId = 0;
Mutex u::NetworkTransferHandler::transferMutex;

u::NetworkTransferHandler::NetworkTransferHandler(NetworkConnection *con, ByteArray* data)
{
	_con = con;
	_data = *data;
	_isBound = false;
	transferMutex.lock();
	_id = nextTransferId++;
	transferMutex.unlock();
	_data.position(0);

	addEvents();

	NetTransferEvent init(NetTransferEvent::INIT, _id);
	_con->room()->dispatchEvent(&init);

}


u::NetworkTransferHandler::~NetworkTransferHandler()
{
	removeEvents();
}

void u::NetworkTransferHandler::addEvents()
{
	lock();
	if (_isBound == true)
	{
		unlock();
		return;
	}
	_con->room()->addEventListener(
		NetTransferEvent::READY,
		Callback(this, cb_cast(&NetworkTransferHandler::onTransferReady))
	);
	_con->room()->addEventListener(
		NetEvent::CLOSED, 
		Callback(this, cb_cast(&NetworkTransferHandler::onConnectionClosed))
	);
	_isBound = true;
	unlock();
}

void u::NetworkTransferHandler::removeEvents()
{
	lock();
	if (_isBound == false)
	{
		unlock();
		return;
	}
	_con->room()->removeEventListener(
		NetTransferEvent::READY,
		Callback(this, cb_cast(&NetworkTransferHandler::onTransferReady))
	);
	_con->room()->removeEventListener(
		NetEvent::CLOSED,
		Callback(this, cb_cast(&NetworkTransferHandler::onConnectionClosed))
	);
	_isBound = false;
	unlock();
}


String u::NetworkTransferHandler::className()
{
	return "u::NetworkTransferHandler";
}


void u::NetworkTransferHandler::destroy()
{
	delete (NetworkTransferHandler*)this;
}


void u::NetworkTransferHandler::onTransferReady(Object *event)
{
	trace(className()+":onTransferReady");
	uint64 id = ((NetTransferEvent*)event)->id();
	event->destroy();

	if(id != _id) return; // not for me

	lock();
	sendNextBlock();
	unlock();
}

void u::NetworkTransferHandler::onConnectionClosed(Object* event)
{
	event->destroy();
	removeEvents();
}

void u::NetworkTransferHandler::sendNextBlock()
{
	NetTransferEvent* event;
	if(_data.available() > 0)
	{
		// especially send to control data. No chunking needed.
		event = new NetTransferEvent(NetTransferEvent::DATA, _id, &_data);
		_data.position(_data.length());
	}
	else
	{
		event = new NetTransferEvent(NetTransferEvent::EOT, _id);
	}

	_con->room()->dispatchEvent(event);
	event->destroy();
}
