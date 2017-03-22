#include <Network/WebEvent>

using namespace u;
using namespace Warp10::Network;

const String WebEvent::INCOMMING_CONNECTION(
	"Warp10::Network::WebEvent::incommingConnection"
);
const String WebEvent::WILL_CLOSE(
	"Warp10::Network::WebEvent::willClose"
);
const String WebEvent::CAN_CLOSE(
	"Warp10::Network::WebEvent::canClose"
);
const String WebEvent::REQUEST(
	"Warp10::Network::WebEvent::request"
);

/**
* Event constructor.
*/
WebEvent::WebEvent(String const& type, WebConnection* connection)
	: u::Event(type)
{
	WebEvent::connection = connection;
}

/**
* Event constructor.
*/
WebEvent::WebEvent(String const& type, WebConnection* connection, WebRequest request)
	: WebEvent(type, connection)
{
	WebEvent::request = request;
}

/**
* Default event constructor.
*/
WebEvent::WebEvent(String const& type)
	: u::Event(type)
{
	WebEvent::connection = null;
}

/**
* Copy constructor.
*/
WebEvent::WebEvent(WebEvent& source)
	: u::Event(*source.type())
{
	WebEvent::connection = source.connection;
	WebEvent::request = source.request;
}

/**
* Destructor.
*/
WebEvent::~WebEvent() {}

/**
* Clone the event.
*/
u::Event* WebEvent::clone()
{
	return new WebEvent(*this);
}

/**
* Destroy this object.
*/
void WebEvent::destroy()
{
	delete (WebEvent*)this;
}

/**
* The class name.
*/
String WebEvent::className()
{
	return "Warp10::Network::WebEvent";
}