#include <Network/WebBuilder>
#include <Network/WebListener>

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
	return new WebListener(this, new Socket(AF_INET, _address, _port));
}

/**
* Setup listener connection.
*/
void WebBuilder::setupListener(String address, int64 port)
{
	_address = address;
	_port    = port;
}

/**
* Create a web network connection.
*/
IConnection* WebBuilder::createConnection(Socket*)
{

}