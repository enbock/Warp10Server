/*
* ControlService.cpp
*
*  Created on: 28.10.2012
*      Author: Endre Bock
*/

#include <ControlService>
#include <ServerEvent>
#include <ServerNetEvent>

using namespace u;


void u::ControlService::onListener(Object* arg)
{
	NetEvent* signal = (NetEvent*)arg;
	EventRoom* room = signal->room();

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

	signal->destroy();
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

	ServerEvent signal(ServerEvent::REGISTER_NETWORK_PLUGIN);
	signal.data = &_plugin;
	_room->dispatchEvent(&signal);
}

void u::ControlService::removeListeners()
{
	if(_plugin.service()) {
		NetworkService* srv = _plugin.service();
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
	NetEvent* signal = (NetEvent *) arg;
	// Its possible, that connection is already deleted. (To slow signal...)
	if(valid(signal->room()))
	{
		signal->room()->addEventListener(
			NetEvent::CLOSE_REQUIRED
			, Callback(this, cb_cast(&ControlService::onConnectionClose))
		);

		signal->room()->addEventListener(
			NetEvent::CLOSED
			, Callback(this, cb_cast(&ControlService::onRemoveConnection))
		);
	}
	signal->destroy();
}

void u::ControlService::onConnectionClose(Object* arg)
{
	NetEvent* signal = (NetEvent *) arg;

	//Todo: Something to clean?

	signal->room()->dispatchEvent(
		new NetEvent(NetEvent::CLOSE_CONFIRMED)
	)->destroy();

	signal->destroy();
}

void u::ControlService::onRemoveConnection(Object* arg)
{
	NetEvent* signal = (NetEvent *) arg;
	EventRoom* room  = signal->room();
	room->removeEventListener(
		NetEvent::CLOSE_REQUIRED
		, Callback(this, cb_cast(&ControlService::onConnectionClose))
	);
	room->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&ControlService::onRemoveConnection))
	);
	signal->destroy();
}
