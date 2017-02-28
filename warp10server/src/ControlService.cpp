/*
* ControlService.cpp
*
*  Created on: 28.10.2012
*      Author: Endre Bock
*/

#include <ControlService>
#include <ServerEvent>
#include <ServerNetEvent>
#include <NetTransferEvent>

using namespace u;


void u::ControlService::onListener(Object* arg)
{
	NetEvent* event = (NetEvent*)arg;
	EventRoom* room = event->room();

	lock();
	_listenerRoom = room;
	unlock();

	room->addEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&ControlService::onListenerClosed))
	);

	room->addEventListener(
		NetEvent::NEW_CONNECTION
		, Callback(this, cb_cast(&ControlService::onNewConnection))
	);

	event->destroy();
}

void u::ControlService::onListenerClosed(Object* arg)
{
	arg->destroy();
	lock();
	_listenerRoom->removeEventListener(NetEvent::CLOSED
		, Callback(this, cb_cast(&ControlService::onListenerClosed))
	);

	_listenerRoom->removeEventListener(
		NetEvent::NEW_CONNECTION
		, Callback(this, cb_cast(&ControlService::onNewConnection))
	);

	_listenerRoom = null;
	unlock();
}

String u::ControlService::toString()
{
	return "[" + className() + "]";
}

u::ControlService::ControlService() : RoomClient()
{
	_listenerRoom = null;
}

u::ControlService::~ControlService()
{
	room(null);
}

void u::ControlService::addListeners()
{
	_room->addEventListener(
		ServerEvent::NETWORK_PLUGIN_REGISTERED
		, Callback(this, cb_cast(&ControlService::onPluginRegistered))
	);

	_room->addEventListener(
		NetEvent::LISTENER
		, Callback(this, cb_cast(&ControlService::onListener))
	);

	_room->addEventListener(
		NetEvent::LISTENER_FAILED
		, Callback(this, cb_cast(&ControlService::onListenerFailed))
	);

	_room->addEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&ControlService::onNetworkClosed))
	);

	ServerEvent event(ServerEvent::REGISTER_NETWORK_PLUGIN);
	event.data = &_plugin;
	_room->dispatchEvent(&event);
}

void u::ControlService::removeListeners()
{
	if(_plugin.service()) {
		NetworkManager* srv = _plugin.service();
		srv->lock();
		srv->removeNetworkPlugin(&_plugin);
		srv->unlock();
	}

	_room->removeEventListener(
		ServerEvent::NETWORK_PLUGIN_REGISTERED
		, Callback(this, cb_cast(&ControlService::onPluginRegistered))
	);

	_room->removeEventListener(
		NetEvent::LISTENER
		, Callback(this, cb_cast(&ControlService::onListener))
	);

	_room->removeEventListener(
		NetEvent::LISTENER_FAILED
		, Callback(this, cb_cast(&ControlService::onListenerFailed))
	);

	_room->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&ControlService::onNetworkClosed))
	);
}

void u::ControlService::onPluginRegistered(Object* arg)
{
	arg->destroy();
	_room->dispatchEvent(
		new ServerNetEvent(
			ServerNetEvent::GET_CTRL_LISTENER
			, "/tmp/w10s_socket"
		)
	)->destroy();
}

String u::ControlService::className()
{
	return "u::ControlService";
}

void u::ControlService::destroy()
{
	delete (ControlService*) this;
}

void u::ControlService::onNetworkClosed(Object* arg)
{
	arg->destroy();
	_room->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&ControlService::onNetworkClosed))
	);
}

void u::ControlService::onListenerFailed(Object* arg)
{
	arg->destroy();
	_room->dispatchEvent(
			new ServerEvent(ServerEvent::SHUTDOWN)
	)->destroy();
}

void u::ControlService::onNewConnection(Object *arg)
{
	NetEvent* event = (NetEvent *) arg;
	// Its possible, that connection is already deleted. (To slow event...)
	if(valid(event->room()))
	{
		event->room()->addEventListener(
			NetEvent::CLOSE_REQUIRED
			, Callback(this, cb_cast(&ControlService::onConnectionClose))
		);

		event->room()->addEventListener(
			NetEvent::CLOSED
			, Callback(this, cb_cast(&ControlService::onRemoveConnection))
		);

		event->room()->addEventListener(
			NetTransferEvent::DATA
			, Callback(this, cb_cast(&ControlService::onNewData))
		);
	}
	event->destroy();
}

void u::ControlService::onConnectionClose(Object* arg)
{
	NetEvent* event = (NetEvent *) arg;

	//Todo: Something to clean?

	event->room()->dispatchEvent(
		new NetEvent(NetEvent::CLOSE_CONFIRMED)
	)->destroy();

	event->destroy();
}

void u::ControlService::onRemoveConnection(Object* arg)
{
	NetEvent* event = (NetEvent *) arg;
	EventRoom* room  = event->room();
	room->removeEventListener(
		NetEvent::CLOSE_REQUIRED
		, Callback(this, cb_cast(&ControlService::onConnectionClose))
	);
	room->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&ControlService::onRemoveConnection))
	);
	room->removeEventListener(
		NetTransferEvent::DATA
		, Callback(this, cb_cast(&ControlService::onNewData))
	);
	event->destroy();
}

void u::ControlService::onNewData(Object* arg)
{
	NetTransferEvent* event = (NetTransferEvent *) arg;
	if(event->data()->readString() == "shutdown")
	{
		_room->dispatchEvent(
			new ServerEvent(ServerEvent::SHUTDOWN)
		)->destroy();
	}
	event->destroy();
}