#include <Network/WebListener>
#include <Network/Event>

using namespace u;
using namespace u::Network;
using namespace Warp10::Network;

/**
* Constructor.
*/
WebListener::WebListener(IBuilder* builder) : IListener()
{
	_builder = builder;

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
	removeEventListener(
		u::Network::Event::CLOSE,
		Callback(this, cb_cast(&WebListener::onClose))
	);
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
	error(className() + "::Listen: TODO to listening");
}

/**
* Listener was requested to close.
*/
void WebListener::onClose(Object* arg)
{
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