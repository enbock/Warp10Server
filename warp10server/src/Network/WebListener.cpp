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
	//Callback cb(this, cb_cast(&NetworkListener::doListening));
	//ThreadSystem::create(&cb);

	return;
}

/**
* Listener was requested to close.
*/
void WebListener::onClose(Object* arg)
{
	_socket->close();
	checkClosed();

	arg->destroy();
}

/**
* Check that all connection closed.
*/
void WebListener::checkClosed()
{
	trace(className() + "::checkClosed: x connection left.");
	u::Network::Event closed(u::Network::Event::CLOSED);
	dispatchEvent(&closed);
}