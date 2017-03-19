#include <Network/WebListener>
#include <Network/Event>

using namespace u;
using namespace u::Network;
using namespace Warp10::Network;

/**
* Constructor.
*/
WebListener::WebListener(IBuilder* builder, Socket* socket) : IListener()
{
	_builder = ((WebBuilder*)builder);
	_socket  = socket;

	addEventListener(
		u::Network::Event::CLOSE,
		Callback(this, cb_cast(&WebListener::onClose))
	);
}

/**
* Destructor.
*/
WebListener::~WebListener()
{
	_socket->destroy();
	removeEventListener(
		u::Network::Event::CLOSE,
		Callback(this, cb_cast(&WebListener::onClose))
	);
	trace(className() + "::~WebListener: Down.");
}

/**
* Destroy this object.
*/
void WebListener::destroy()
{
	delete (WebListener*)this;
}

/**
* The class name.
*/
String WebListener::className()
{
	return "Warp10::Network::WebListener";
}

/**
* Start listening for new connections.
*/
void WebListener::listen()
{
	bool ret = _socket->listen();

	if(ret == false) {
		error(className()+"::listen: Can not etablish connection.");
		u::Network::Event close(
			u::Network::Event::CLOSE
		);
		dispatchEvent(&close);
		return;
	}

	// start incoming connections
	Callback cb(this, cb_cast(&WebListener::listenSocket));
	ThreadSystem::create(&cb);

	return;
}

/**
* Listener was requested to close.
*/
void WebListener::onClose(Object* arg)
{
	checkClosed();

	/**
	 * Close socket AFTER check to synchronize the process streams.
	 * Otherwise could object destored while connection is in closing
	 * but not closed.
	 */
	_socket->close();

	arg->destroy();
}

/**
* Check that all connection closed.
*/
void WebListener::checkClosed()
{
	lock();
	int64 count      = _connections.length();
	bool isConnected = _socket->isConnected();
	unlock();
	trace(
		className() + "::checkClosed: " + int2string(count)
		+ " connection left and socket is " 
		+ (isConnected ? "connected" : "disconnected")
		+ "."
	);
	if(count == 0 && isConnected == false) {
		u::Network::Event closed(u::Network::Event::CLOSED);
		dispatchEvent(&closed);
	}
}

/**
* Listen for incomming connections.
*/
void WebListener::listenSocket(Object* arg)
{
	lock();
	Socket* listener = _socket;
	unlock();

	if(!valid(listener)) return;
	trace(className() + "::listenSocket: Wait for connection...");
	Socket* newSocket = listener->accept();

	lock();
	if(newSocket == null)
	{
		// listening interrupted...socket closed?
		if(valid(_socket) && _socket->isConnected())
		{
			error(className()+"::listenSocket: Error on getting connection.");
		}
		else
		{
			trace(className() + "::listenSocket: Stop listening.");
			unlock();
			checkClosed();
			return;
		}
	}
	else
	{
		trace(className()+"::listenSocket: Incomming connection.");
		IConnection* newConnection = _builder->createConnection(newSocket);
		if(newConnection != null)
		{
			_connections.push(newConnection);

			newConnection->addEventListener(
				u::Network::Event::CLOSED
				, Callback(
					this, cb_cast(&WebListener::onConnectionClosed)
				)
			);

			u::Network::Event connectionCreated(
				u::Network::Event::CONNECTION_CREATED,
				_builder->networkType,
				newConnection
			);
			unlock();
			dispatchEvent(&connectionCreated);
			lock();
		}
		else
		{
			error(
				className()
				+ "::listenSocket: Can not etablish incomming conection"
			);
			newSocket->destroy();
		}
	}

	//self start ;)
	Callback cb(this, cb_cast(&WebListener::listenSocket));
	ThreadSystem::create(&cb);

	unlock();
}

/**
* Connection was closed.
*/
void WebListener::onConnectionClosed(Object *arg)
{
	error(
		className() + "::onConnectionClosed: TODO"
	);

	arg->destroy();
}