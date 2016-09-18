/*
 * Warp10Admin.cpp
 *
 *  Created on: 11.06.2011
 *      Author: Endre Bock
 */

#include <Warp10Admin>
#include <NetworkServiceControlPlugin>
#include <ServerNetEvent>
#include <signal.h>
#include <NetTransferEvent>

using namespace u;

u::Warp10Admin::Warp10Admin(Vector<String> arg)
		: RoomOwner()
{
	_connectionIsReady = false;
	_handler = null;

	// register ctrl+c
	signal((int) SIGINT, Warp10Admin_OSEventHandler);

	_network.room(room());
	_room.addEventListener(
		NetEvent::NEW_CONNECTION
		, Callback(this, cb_cast(&Warp10Admin::onConnection))
	);

	_room.addEventListener(
		NetEvent::CONNECTION_FAILED
		, Callback(this, cb_cast(&Warp10Admin::onConnectionFailed))
	);

	_network.registerNetworkPlugin(new NetworkServiceControlPlugin());

	_room.dispatchEvent(
		new ServerNetEvent(
			ServerNetEvent::GET_CTRL_CONNECTION
			, arg.length() > 1 ? arg[1] : "/tmp/w10s_socket"
		)
	)->destroy();
}

u::Warp10Admin::~Warp10Admin()
{
	_room.removeEventListener(
		NetEvent::NEW_CONNECTION
		, Callback(this, cb_cast(&Warp10Admin::onConnection))
	);
	_room.removeEventListener(
		NetEvent::CONNECTION_FAILED
		, Callback(this, cb_cast(&Warp10Admin::onConnectionFailed))
	);

	if (_handler != null) _handler->destroy();

	if(_connection && valid(_connection))
	{
		_connection->destroy();
	}
}

void u::Warp10Admin::onConnection(Object* arg)
{
	NetEvent* event = (NetEvent*) arg;
	NetworkConnection* con = event->connection();
	_connection = con;

	con->room()->addEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&Warp10Admin::onClosed))
	);

	con->room()->addEventListener(
		NetEvent::CONNECTION_IS_READY
		, Callback(this, cb_cast(&Warp10Admin::onReadyConnection))
	);

	con->room()->addEventListener(
		NetEvent::CLOSE_REQUIRED
		, Callback(this, cb_cast(&Warp10Admin::onServerClose))
	);

	event->destroy();

	lock();
	//if(_connectionIsReady == false)
	{
		con->room()->dispatchEvent(
			new NetEvent(NetEvent::IS_CONNECTION_READY)
		)->destroy();
	}
	unlock();
}

void u::Warp10Admin::onClosed(Object* arg)
{
	NetEvent* event = (NetEvent*)arg;

	if(event->closee == null)
	{
		error(className()+":onClose(): No closee in event.");
		return;
	}

	lock();
	EventRoom* ncRoom = ((NetworkConnection*)event->closee)->room();
	ncRoom->removeEventListener(
		NetEvent::CLOSED
		, Callback(this, cb_cast(&Warp10Admin::onClosed))
	);
	ncRoom->removeEventListener(
		NetEvent::CONNECTION_IS_READY
		, Callback(this, cb_cast(&Warp10Admin::onReadyConnection))
	);
	ncRoom->removeEventListener(
		NetEvent::CLOSE_REQUIRED
		, Callback(this, cb_cast(&Warp10Admin::onServerClose))
	);
	_connection->destroy();
	_connection = null;

	event->destroy();

	unlock();
	while(ThreadSystem::numThreads()> 1)
	{
		trace("Waiting for threads: "+int2string(ThreadSystem::numThreads()));
		usleep(1000000/(FPS*10));
	}
	programExit();
}

void u::Warp10Admin_OSEventHandler(int signalNumber)
{
	switch (signalNumber)
	{
		case SIGINT:
		{
			((Warp10Admin*) __main__)->shutdown();
		}
		break;
	}
}

void u::Warp10Admin::shutdown()
{
	lock();
	if(valid(_connection))
	{
		_connection->room()->dispatchEvent(
			new NetEvent(NetEvent::CLOSE)
		)->destroy();
		unlock();
	}
	else
	{
		unlock();
		programExit();
	}
}

void u::Warp10Admin::programExit()
{
	if(ThreadSystem::isTerminating != true)
	{
		trace(className() + ": Exit program.");
		u::programExit();
	}
}

String u::Warp10Admin::className()
{
	return "u::Warp10Admin";
}

void u::Warp10Admin::destroy()
{
	delete (Warp10Admin*) this;
}

String u::Warp10Admin::toString()
{
	return "["+className()+" Connection:"+_connection->toString()+"]";
}

void u::Warp10Admin::onConnectionFailed(Object* arg)
{
	arg->destroy();
	shutdown();
}

void u::Warp10Admin::onReadyConnection(Object* arg)
{
	NetEvent* event = (NetEvent*) arg;
	EventRoom* room = event->connection()->room();
	event->destroy();
	lock();
	if(_connectionIsReady == true) {
		// double
		unlock();
		return;
	}
	_connectionIsReady = true;
	unlock();

	room->removeEventListener(
		NetEvent::CONNECTION_IS_READY
		, Callback(this, cb_cast(&Warp10Admin::onReadyConnection))
	);
	trace("Connection is ready.");

	startCLI();
}

void u::Warp10Admin::onServerClose(Object *arg)
{
	NetEvent* event = (NetEvent*) arg;

	// Some close actions?

	event->room()->dispatchEvent(
		new NetEvent(NetEvent::CLOSE_CONFIRMED)
	)->destroy();

	event->destroy();
}

void u::Warp10Admin::startCLI()
{
	Vector<String> cmd;
	cmd.push("close");
	cmd.push("shutdown");
	cmd.push("help");

	bool end = false;
	String input("help");
	while(end == false)
	{
		switch(cmd.indexOf(input.lowerCase()))
		{
			case 0:
			{
				shutdown();
				end = true;
			} break;
			case 1:
			{
				shutdownServer();
				end = true;
			} break;
			case 2:
			{
				std::cout << "Available commands:" << std::endl;
				int64 i, l;
				for(i=0, l = cmd.length(); i < l; i++)
				{
					std::cout << "\t" << cmd[i] << std::endl;
				}
			} break;
			default:
			{
				std::cerr << "\033[31;1mUnknown command " << input << "\033[0m" << std::endl;
			}
		}
		if(end == false)
		{
			std::cout << "W10S> ";
			input = "";
			std::cin >> input;
		}
		lock();
		end = end == true || !valid(_connection) || _connection->isClose();
		unlock();
	}
}

void u::Warp10Admin::shutdownServer()
{
	ByteArray data;
	String command = "shutdown";

	data.writeString(command);

	_handler = new NetworkTransferHandler(_connection, &data);
}

