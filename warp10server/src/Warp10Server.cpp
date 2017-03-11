/*
 * Warp10Server.cpp
 *
 *  Created on: 11.06.2011
 *      Author: Endre Bock
 */

#include <ServerEvent>
#include <Warp10Server>
#include <signal.h>
#include <Network/Event>

using namespace u;

u::Warp10Server::Warp10Server(Vector<String> arg) : RoomOwner()
{
	// connect Ctrl+C with shutdown
	signal((int)SIGINT, OSEventHandler);
	signal((int)SIGKILL, OSEventHandler);
	signal((int)SIGABRT, OSEventHandler);
	signal((int)SIGTERM, OSEventHandler);

	_isShuttingDown = false;

	_room.addEventListener(
		ServerEvent::SHUTDOWN, 
		Callback(this, cb_cast(&Warp10Server::onShutdown))
	);

	_manager.addEventListener(
		Network::Event::CLOSED, 
		Callback(this, cb_cast(&Warp10Server::onManagerClosed))
	);

	trace(className() + ": Server is running.");
}

u::Warp10Server::~Warp10Server()
{
	_room.removeEventListener(
		ServerEvent::SHUTDOWN, 
		Callback(this, cb_cast(&Warp10Server::onShutdown))
	);

	_manager.removeEventListener(
		Network::Event::CLOSED, 
		Callback(this, cb_cast(&Warp10Server::onManagerClosed))
	);

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

	trace(className() + ": Server shutting down.");
	lock();
	_isShuttingDown = true;
	unlock();

	_manager.dispatchEvent(
		new Network::Event(Network::Event::CLOSE)
	)->destroy();
}

void u::Warp10Server::programExit()
{
	lock();	unlock(); // wait for other progress(es)
	trace(ThreadSystem::toString()+"\n" + className() + ": Exit programm.");
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
	trace("OSEventHandler: Interrupt received.");
	((Warp10Server *)__main__)->shutdown();
}

/**
* Manager has closed all listeners.
*/
void Warp10Server::onManagerClosed(Object* arg)
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
