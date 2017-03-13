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
	return new WebListener(this);
}