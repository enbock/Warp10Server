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
#include <NetworkServiceControlPlugin>

using namespace u;

String u::ControlService::toString()
{
	return "[" + className() + "]";
}

u::ControlService::ControlService() : NetworkService()
{
	_plugin = new NetworkServiceControlPlugin();
}

u::ControlService::~ControlService()
{
	if(_plugin->service()) {
		trace(this->className()+": Remove plugin from manager.");
		NetworkManager* srv = _plugin->service();
		srv->lock();
		srv->removeNetworkPlugin(_plugin);
		srv->unlock();
	}
	delete (NetworkServiceControlPlugin*)_plugin;
	removeListeners();
}

String u::ControlService::className()
{
	return "u::ControlService";
}

void u::ControlService::destroy()
{
	delete (ControlService*) this;
}

void u::ControlService::addListeners()
{
	NetworkService::addListeners();
	_room->addEventListener(
		ServerEvent::NETWORK_PLUGIN_REGISTERED
		, Callback(this, cb_cast(&ControlService::onPluginRegistered))
	);

	ServerEvent event(ServerEvent::REGISTER_NETWORK_PLUGIN);
	event.data = _plugin;
	_room->dispatchEvent(&event);
}

void u::ControlService::removeListeners()
{
	NetworkService::removeListeners();

	_room->removeEventListener(
		ServerEvent::NETWORK_PLUGIN_REGISTERED
		, Callback(this, cb_cast(&ControlService::onPluginRegistered))
	);
}

/** Complete override */
void u::ControlService::onPluginRegistered(Object* arg)
{
	ServerEvent *event = (ServerEvent*) arg;
	if (event->data == _plugin) {
		_room->dispatchEvent(
			new ServerNetEvent(
				ServerNetEvent::GET_CTRL_LISTENER
				, "/tmp/w10s_socket"
			)
		)->destroy();
	}

	event->destroy();
}

/** Complete override */
void u::ControlService::onListenerFailed(Object* arg)
{
	ServerNetEvent *event = ((ServerNetEvent*) arg);
	if (!isMyListener(event))
	{
		event->destroy();
		return;
	}
	event->destroy();
	_room->dispatchEvent(
			new ServerEvent(ServerEvent::SHUTDOWN)
	)->destroy();
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

/**
 * Check if a incoming listener event really for me.
 */
bool u::ControlService::isMyListener(NetEvent* event)
{
	// TODO: Address should be configured.
	// Is address a good way to identify the correct listener?
	return event->address() == "/tmp/w10s_socket"
		&& event->port() == 0
	;
}