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

/**
* Event constructor.
* <p>Events:<ul>
* <li> NEW_CONNECTION
* </ul>
* </p>
*/
WebEvent::WebEvent(String const& type, WebConnection* connection)
	: u::Event(type)
{
	WebEvent::connection = connection;
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