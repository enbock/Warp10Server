/*
 * Warp10Server.cpp
 *
 *  Created on: 11.06.2011
 *      Author: Endre Bock
 */

#include <ServerEvent>
#include <Server>
#include <signal.h>
#include <Network/Event>

using namespace u;
using namespace Warp10;

Server::Server(Vector<String> arg) : RoomOwner()
{
	// connect Ctrl+C with shutdown
	signal((int)SIGINT, OSEventHandler);
	signal((int)SIGKILL, OSEventHandler);
	signal((int)SIGABRT, OSEventHandler);
	signal((int)SIGTERM, OSEventHandler);

	_isShuttingDown = false;

	_room.addEventListener(
		ServerEvent::SHUTDOWN, 
		Callback(this, cb_cast(&Server::onShutdown))
	);

	_manager.addEventListener(
		u::Network::Event::CLOSED, 
		Callback(this, cb_cast(&Server::onManagerClosed))
	);

	trace(className() + ": Server is running.");

	_webService = new WebService(&_manager, "0.0.0.0", 80);
	_webService->registerNetwork();
}

Server::~Server()
{
	_webService->destroy();
	
	_room.removeEventListener(
		ServerEvent::SHUTDOWN, 
		Callback(this, cb_cast(&Server::onShutdown))
	);

	_manager.removeEventListener(
		u::Network::Event::CLOSED, 
		Callback(this, cb_cast(&Server::onManagerClosed))
	);

	trace(className() + ": Server is down.");
}

void Server::shutdown()
{
	// tell the network for shutdown
	_room.dispatchEvent(
		new ServerEvent(ServerEvent::SHUTDOWN)
	)->destroy();
}

void Server::onShutdown(Object *arg)
{
	arg->destroy();

	trace(className() + ": Server shutting down.");
	lock();
	_isShuttingDown = true;
	unlock();

	_manager.dispatchEvent(
		new u::Network::Event(u::Network::Event::CLOSE)
	)->destroy();
}

void Server::programExit()
{
	lock();	unlock(); // wait for other progress(es)
	trace(
		//ThreadSystem::toString()+"\n" + 
		className() + ": Exit programm."
	);
	u::programExit();
}

String Server::className()
{
	return "Warp10::Server";
}

void Server::destroy()
{
	delete (Server *)this;
}

String Server::toString()
{
	return "[" + className() + "]";
}

void Warp10::OSEventHandler(int signalNumber)
{
	trace("OSEventHandler: Interrupt received.");
	((Server *)__main__)->shutdown();
}

/**
* Manager has closed all listeners.
*/
void Server::onManagerClosed(Object* arg)
{
	arg->destroy();
	lock();
	bool shutdown = _isShuttingDown == true;
	unlock();
	if (shutdown) {
		trace(className() + ": All closed. Exit program.");
		programExit();
	} else trace(className() + ": All closed. Continue program.");
}
