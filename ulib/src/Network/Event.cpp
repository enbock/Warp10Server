#include <Network/Event>

using namespace u;
using namespace Network;

const String Network::Event::REGISTER_NETWORK(
		"u::Network::Event::registerNetwork"
);
const String Network::Event::REQUEST_LISTENER(
		"u::Network::Event::requestListener"
);
const String Network::Event::NETWORK_REGISTERED(
		"u::Network::Event::networkRegistered"
);
const String Network::Event::LISTENER_CREATED(
		"u::Network::Event::listenerCreated"
);
const String Network::Event::CLOSE("u::Network::Event::close");
const String Network::Event::CLOSED("u::Network::Event::closed");
const String Network::Event::CONNECTION_CREATED(
		"u::Network::Event::connectionCreated"
);

/**
* Event constructor with builder reference.
*/
Network::Event::Event(String const& type, String networkType, IBuilder* builder)
		: u::Event(type)
{
	Event::networkType = networkType;
	Event::builder     = builder;
	Event::listener    = null;
}


/**
* Event constructor with listener reference.
*/
Network::Event::Event(
		String const& type
		, String networkType
		, IListener* listener
)
		: u::Event(type)
{
	Event::networkType = networkType;
	Event::builder     = null;
	Event::listener    = listener;
}

/**
* Event constructor with connection reference.
*/
Network::Event::Event(
		String const& type
		, String networkType
		, IConnection* connection
)
		: u::Event(type)
{
	Event::networkType = networkType;
	Event::builder     = null;
	Event::connection  = connection;
}

/**
* Event constructor.
*/
Network::Event::Event(String const& type, String networkType)
		: u::Event(type)
{
	Event::networkType = networkType;
	Event::builder     = null;
	Event::listener    = null;
}

/**
* Default event constructor.
*/
Network::Event::Event(String const& type)
		: u::Event(type)
{
	Event::networkType = "";
	Event::builder     = null;
	Event::listener    = null;
}

/**
* Copy constructor.
*/
Network::Event::Event(Event& source)
		: u::Event(*source.type())
{
	Event::networkType = source.networkType;
	Event::builder     = source.builder;
	Event::listener    = source.listener;
}

/**
* Destructor.
*/
Network::Event::~Event()
{}

/**
* Clone the event.
*/
u::Event* Network::Event::clone()
{
	return new Network::Event(*this);
}

/**
* Destroy this object.
*/
void Network::Event::destroy()
{
	delete (Network::Event*) this;
}

/**
* The class name.
*/
String Network::Event::className()
{
	return "u::Network::Event";
}