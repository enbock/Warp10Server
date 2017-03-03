
#include <NetworkService>
#include <NetTransferEvent>

using namespace u;

String u::NetworkService::toString()
{
	return "[" + className() + "]";
}

u::NetworkService::NetworkService() : RoomClient()
{
	_listenerRoom = null;
}

u::NetworkService::~NetworkService()
{
	/**
	* @dev: Remember: On destructing this object, the derived object is already
	*       gone. You can not use this to indeirect call removeListeners() on
	*       derived classes.
	*/
	room(null);
}

String u::NetworkService::className()
{
	return "u::NetworkService";
}

void u::NetworkService::destroy()
{
	delete (NetworkService*) this;
}


void u::NetworkService::onListener(Object* arg)
{
	NetEvent* event = (NetEvent*)arg;
	EventRoom* room = event->room();

	lock();
	_listenerRoom = room;
	unlock();

	room->addEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&NetworkService::onListenerClosed))
	);

	room->addEventListener(
		NetEvent::NEW_CONNECTION
		, Callback(this, cb_cast(&NetworkService::onNewConnection))
	);

	event->destroy();
}

void u::NetworkService::onListenerClosed(Object* arg)
{
	arg->destroy();
	lock();
	_listenerRoom->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&NetworkService::onListenerClosed))
	);

	_listenerRoom->removeEventListener(
		NetEvent::NEW_CONNECTION
		, Callback(this, cb_cast(&NetworkService::onNewConnection))
	);

	_listenerRoom = null;
	unlock();
}

void u::NetworkService::addListeners()
{
	_room->addEventListener(
		NetEvent::LISTENER
		, Callback(this, cb_cast(&NetworkService::onListener))
	);

	_room->addEventListener(
		NetEvent::LISTENER_FAILED
		, Callback(this, cb_cast(&NetworkService::onListenerFailed))
	);

	_room->addEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&NetworkService::onNetworkClosed))
	);
}

void u::NetworkService::removeListeners()
{
	_room->removeEventListener(
		NetEvent::LISTENER
		, Callback(this, cb_cast(&NetworkService::onListener))
	);

	_room->removeEventListener(
		NetEvent::LISTENER_FAILED
		, Callback(this, cb_cast(&NetworkService::onListenerFailed))
	);

	_room->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&NetworkService::onNetworkClosed))
	);
}

void u::NetworkService::onNetworkClosed(Object* arg)
{
	arg->destroy();
	_room->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&NetworkService::onNetworkClosed))
	);
}

void u::NetworkService::onNewConnection(Object *arg)
{
	NetEvent* event = (NetEvent *) arg;
	// Its possible, that connection is already deleted. (To slow event...)
	if(valid(event->room()))
	{
		event->room()->addEventListener(
			NetEvent::CLOSE_REQUIRED
			, Callback(this, cb_cast(&NetworkService::onConnectionClose))
		);

		event->room()->addEventListener(
			NetEvent::CLOSED
			, Callback(this, cb_cast(&NetworkService::onRemoveConnection))
		);

		event->room()->addEventListener(
			NetTransferEvent::DATA
			, Callback(this, cb_cast(&NetworkService::onNewData))
		);
	}
	event->destroy();
}

void u::NetworkService::onConnectionClose(Object* arg)
{
	NetEvent* event = (NetEvent *) arg;

	//Todo: Something to clean?

	event->room()->dispatchEvent(
		new NetEvent(NetEvent::CLOSE_CONFIRMED)
	)->destroy();

	event->destroy();
}

void u::NetworkService::onRemoveConnection(Object* arg)
{
	NetEvent* event = (NetEvent *) arg;
	EventRoom* room  = event->room();
	room->removeEventListener(
		NetEvent::CLOSE_REQUIRED
		, Callback(this, cb_cast(&NetworkService::onConnectionClose))
	);
	room->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&NetworkService::onRemoveConnection))
	);
	room->removeEventListener(
		NetTransferEvent::DATA
		, Callback(this, cb_cast(&NetworkService::onNewData))
	);
	event->destroy();
}

void u::NetworkService::onPluginRegistered(Object* arg)
{
	arg->destroy();
}

void u::NetworkService::onListenerFailed(Object* arg)
{
	arg->destroy();
}