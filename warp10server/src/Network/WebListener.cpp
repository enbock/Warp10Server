#include <Network/WebListener>
#include <Network/Event>
#include <Network/WebEvent>

using namespace u;
using namespace u::Network;
using namespace Warp10::Network;

/**
* Constructor.
*/
WebListener::WebListener(IBuilder* builder, Socket* socket) : IListener()
{
	_builder   = ((WebBuilder*) builder);
	_socket    = socket;
	_isClosed  = false;
	_closeSent = false;

	addEventListener(
			u::Network::Event::CLOSE, Callback(
					this
					, cb_cast(&WebListener::onClose))
	);
	addEventListener(
			WebEvent::CAN_CLOSE, Callback(
					this
					, cb_cast(&WebListener::onCanClose))
	);
}

/**
* Destructor.
*/
WebListener::~WebListener()
{
	_socket->destroy();
	removeEventListener(
			u::Network::Event::CLOSE, Callback(
					this
					, cb_cast(&WebListener::onClose))
	);
	removeEventListener(
			WebEvent::CAN_CLOSE, Callback(
					this
					, cb_cast(&WebListener::onCanClose))
	);
	trace(className() + "::~WebListener: Down.");
}

/**
* Destroy this object.
*/
void WebListener::destroy()
{
	delete (WebListener*) this;
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

	if(ret == false)
	{
		error(className() + "::listen: Can not etablish connection.");
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
	bool sendWillClose = false;
	arg->destroy();
	lock();
	if(_isClosed == false)
	{

		_socket->close();

		int64 i = 0, l = _connections.length();
		for(; i < l; i++)
		{
			u::Network::Event close(u::Network::Event::CLOSE);
			IConnection* connection = _connections[i];
			connection->dispatchEvent(&close);
		}

		sendWillClose = true;
	}
	_isClosed          = true;
	unlock();

	checkClosed();

	if(sendWillClose == true)
	{
		WebEvent willClose(WebEvent::WILL_CLOSE);
		dispatchEvent(&willClose);
	}
}

/**
* Controller allow close.
*/
void WebListener::onCanClose(Object* arg)
{
	arg->destroy();
	checkClosed();
}

/**
* Check that all connection closed.
*/
void WebListener::checkClosed()
{
	lock();
	int64 count    = _connections.length();
	bool  isClosed = _isClosed;
	unlock();
	trace(
			className() + "::checkClosed: " + int2string(count)
			+ " connection left and socket is "
			+ (isClosed == false ? "connected" : "disconnected")
			+ "."
	);
	if(
			count == 0 && isClosed == true
			&& hasEventListener(WebEvent::WILL_CLOSE) == false
			)
	{
		lock(); // sync canClose and connection closed here
		bool sent = _closeSent;
		_closeSent = true;
		unlock();
		if(sent == false)
		{
			u::Network::Event closed(u::Network::Event::CLOSED);
			dispatchEvent(&closed);
		}
	}
}

/**
* Listen for incoming connections.
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
			error(className() + "::listenSocket: Error on getting connection.");
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
		trace(className() + "::listenSocket: Incoming connection.");
		IConnection* newConnection = _builder->createConnection(newSocket);
		if(newConnection != null && _isClosed == false)
		{
			_connections.push(newConnection);

			newConnection->addEventListener(
					u::Network::Event::CLOSED
					, Callback(
							this, cb_cast(&WebListener::onConnectionClosed)
					)
			);

			WebEvent connectionCreated(
					WebEvent::INCOMMING_CONNECTION
					, (WebConnection*) newConnection
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
	unlock();

	//self start ;)
	Callback cb(this, cb_cast(&WebListener::listenSocket));
	ThreadSystem::create(&cb);
}

/**
* Connection was closed.
*/
void WebListener::onConnectionClosed(Object* arg)
{
	u::Network::Event* event      = ((u::Network::Event*) arg);
	IConnection      * connection = ((IConnection*) event->target());
	lock();
	int64 index    = _connections.erase(connection);
	if(index != -1)
	{
		trace(
				className()
				+ "::onConnectionClosed: Removed connection from list."
		);
		connection->removeEventListener(
				u::Network::Event::CLOSED
				, Callback(this, cb_cast(&WebListener::onConnectionClosed))
		);
		connection->destroy();
	}
	bool  isClosed = _isClosed;
	unlock();
	if(isClosed == true)
	{
		checkClosed();
	}

	event->destroy();
}