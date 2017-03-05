
#include <NetworkWebConnection>
#include <NetTransferEvent>
#include <NetworkWebDecoder>
#include <NetEvent>

using namespace u;


u::NetworkWebConnection::NetworkWebConnection(Socket* socket, NetworkWebDecoder* decoder)
	: NetworkConnection(socket, (NetworkDecoder*) decoder)
{
	isReady = false;

	_room.addEventListener(
		NetTransferEvent::EOT
		, Callback(this, cb_cast(&NetworkWebConnection::onEOT))
	);
	_room.addEventListener(
		NetEvent::CLOSE_CONFIRMED
		, Callback(this, cb_cast(&NetworkWebConnection::onCloseConfirmed))
	);
	_room.addEventListener(
		NetEvent::CONNECTION_IS_READY
		, Callback(this, cb_cast(&NetworkWebConnection::onTransferReady))
	);
}

u::NetworkWebConnection::~NetworkWebConnection()
{
	lock();
	_room.removeEventListener(
		NetTransferEvent::EOT
		, Callback(this, cb_cast(&NetworkWebConnection::onEOT))
	);
	_room.removeEventListener(
		NetEvent::CLOSE_CONFIRMED
		, Callback(this, cb_cast(&NetworkWebConnection::onCloseConfirmed))
	);
	_room.removeEventListener(
		NetEvent::CONNECTION_IS_READY
		, Callback(this, cb_cast(&NetworkWebConnection::onTransferReady))
	);
	unlock();
}

String u::NetworkWebConnection::className()
{
	return "u::NetworkWebConnection";
}

String u::NetworkWebConnection::toString()
{
	return "[" + className() + "]";
}

/**
* Default destroyer.
*/
void u::NetworkWebConnection::destroy()
{
	delete (NetworkWebConnection*)this;
}


/**
* Event API: Data transfer initialized[1].
*/
void u::NetworkWebConnection::onTransferInit(Object *event)
{
}

/**
* Transfer begin event received[2].
*/
void u::NetworkWebConnection::doTransferInit(uint64 id)
{}

/**
* Event API: Ready for receiving of data[3].
*/
void u::NetworkWebConnection::onTransferReady(Object *event)
{
	event->destroy();
	doReady();
}

/**
* Transfer ready package was received[4].
*/
void u::NetworkWebConnection::doTransfer(uint64 id)
{}

/**
* Event API: Ready for sending of data[5].
*/
void u::NetworkWebConnection::onSendDataRequest(Object *event)
{}

/**
* Event API: Data transport[6].
*/
void u::NetworkWebConnection::onSendData(Object *event)
{}

/**
* A datablock was received[7].
*/
void u::NetworkWebConnection::doReceivedData(uint64 id, ByteArray* data)
{
	NetTransferEvent event(NetTransferEvent::DATA, id, data);
	room()->dispatchEvent(&event);
}

/**
* Event API: Get the received data[8].
*/
void u::NetworkWebConnection::onGetDataRequest(Object *event)
{}

/**
* Event API: Complete saved[9].
*/
void u::NetworkWebConnection::onDataComplete(Object event)
{}

/**
* Event API: End of transaction[10].
*/
void u::NetworkWebConnection::onEOT(Object *event)
{
	NetTransferEvent* netEvent = ((NetTransferEvent*)event);
	((NetworkWebDecoder*)_decoder)->encodeResponse(netEvent->data());
	netEvent->destroy();
}

/**
* End of transfer received[11].
*/
void u::NetworkWebConnection::doEOT(uint64 id)
{}


/**
* Send event for a etablished connection.
*/
void u::NetworkWebConnection::doReady()
{
	lock();
	isReady = true;
	unlock();
	((NetworkWebDecoder*)_decoder)->checkReceivedData(null);
}

/**
* Close begin procedure was received.
*/
void u::NetworkWebConnection::doCloseRequired()
{}

void u::NetworkWebConnection::onClose(Object* event)
{
	NetworkConnection::onClose(event);
	lock();
	if(valid(_decoder)) ((NetworkWebDecoder*)_decoder)->sendClose();
	unlock();
}

/**
* Event API: Close is confirmed.
*/
void u::NetworkWebConnection::onCloseConfirmed(Object* event)
{
	event->destroy();
	lock();
	if(valid(_decoder)) ((NetworkWebDecoder*)_decoder)->sendClose();
	unlock();
}