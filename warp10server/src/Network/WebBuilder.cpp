#include <Network/WebBuilder>
#include <Network/WebListener>
#include <Network/WebConnection>

using namespace u;
using namespace u::Network;
using namespace Warp10::Network;

/**
* Default constructor.
*/
WebBuilder::WebBuilder() : IBuilder()
{

}

/**
* Destructor.
*/
WebBuilder::~WebBuilder()
{

}

/**
* Destroy this object.
*/
void WebBuilder::destroy()
{
	delete (WebBuilder*)this;
}

/**
* The class name.
*/
String WebBuilder::className()
{
	return "Warp10::Network::WebBuilder";
}

/**
* Create a listener for the network type.
*/
IListener* WebBuilder::createListener()
{
	trace(
		className() + "::createListener: Create socket on " + _address
		+ " " + int2string(_port)
	);
	lock();
	WebListener* listener = new WebListener(
		this, new Socket(AF_INET, _address, _port)
	);
	unlock();
	return listener;
}

/**
* Setup listener connection.
*/
void WebBuilder::setupListener(String address, int64 port)
{
	lock();
	_address = address;
	_port    = port;
	unlock();
}

/**
* Create a web network connection.
*/
IConnection* WebBuilder::createConnection(Socket* socket)
{
	return new WebConnection(socket);
}