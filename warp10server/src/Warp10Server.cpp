/*
 * Warp10Server.cpp
 *
 *  Created on: 11.06.2011
 *      Author: Endre Bock
 */

#include <ControlService>
#include <NetworkServicePlugin>
#include <ServerNetEvent>
#include <Warp10Server>
#include <signal.h>

using namespace u;

u::Warp10Server::Warp10Server(Vector<String> arg) : RoomOwner()
{
	// connect Ctrl+C with shutdown
	signal((int)SIGINT, OSEventHandler);
	signal((int)SIGKILL, OSEventHandler);
	signal((int)SIGABRT, OSEventHandler);
	signal((int)SIGTERM, OSEventHandler);

	_room.addEventListener(
		ServerEvent::SHUTDOWN, 
		Callback(this, cb_cast(&Warp10Server::onShutdown))
	);

	_room.addEventListener(
		ServerEvent::REGISTER_NETWORK_PLUGIN, 
		Callback(this, cb_cast(&Warp10Server::onRegisterNetworkPlugin))
	);

	// network setup
	_network.room(room());

	trace(className() + ": Server is running.");

	/*
	 * Controller
	 */
	ControlService *ctrl;
	ctrl = new ControlService();
	ctrl->room(room());
	_modules.push(ctrl);
}

u::Warp10Server::~Warp10Server()
{
	_room.removeEventListener(
		ServerEvent::SHUTDOWN, 
		Callback(this, cb_cast(&Warp10Server::onShutdown))
	);

	_room.removeEventListener(
		ServerEvent::REGISTER_NETWORK_PLUGIN, 
		Callback(this, cb_cast(&Warp10Server::onRegisterNetworkPlugin))
	);

	while(_modules.length())
	{
		Object *module;
		module = (Object *)_modules.pop();
		module->destroy();
	}

	_network.room(null);

	trace(className() + ": Server is down.");
}

void u::Warp10Server::shutdown()
{
	// tell the network for shutdown
	_room.dispatchEvent(
		new ServerEvent(ServerEvent::SHUTDOWN)
	)->destroy();
}

void u::Warp10Server::onShutdown(Object *arg)
{
	arg->destroy();

	_room.removeEventListener(
		ServerEvent::SHUTDOWN,
		Callback(this, cb_cast(&Warp10Server::onShutdown))
	);

	trace(className() + ": Server shutting down.");
	_room.dispatchEvent(
			new NetEvent(NetEvent::CLOSE))
	->destroy();

	// Waiting for other thread (1 is me:)
	while (ThreadSystem::numThreads() > 1)
	{
		trace("Waiting for threads: " + int2string(ThreadSystem::numThreads()));
		usleep(1000000/(FPS*10));
	}
	programExit();
}

void u::Warp10Server::programExit()
{
	trace(className() + ": Exit programm.");
	u::programExit();
}

String u::Warp10Server::className()
{
	return "u::Warp10Server";
}

void u::Warp10Server::destroy()
{
	delete (Warp10Server *)this;
}

String u::Warp10Server::toString()
{
	return "[" + className() + "]";
}

void u::OSEventHandler(int signalNumber)
{
	/*switch (signalNumber)
	{
		case SIGINT:
		{*/
			((Warp10Server *)__main__)->shutdown();
		/*}
		break;
	}*/
}

void u::Warp10Server::onRegisterNetworkPlugin(Object *arg)
{
	ServerEvent *event = (ServerEvent *)arg;
	_network.lock();
	_network.registerNetworkPlugin((NetworkServicePlugin *)event->data);
	_network.unlock();

	ServerEvent sendEvent(ServerEvent::NETWORK_PLUGIN_REGISTERED);
	sendEvent.data = event->data;

	_room.dispatchEvent(&sendEvent);

	event->destroy();
}
