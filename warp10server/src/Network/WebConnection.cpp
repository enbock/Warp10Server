#include <Network/WebConnection>
#include <Network/Event>

using namespace u;
using namespace u::Network;
using namespace Warp10::Network;

/**
* Default constructor.
*/
WebConnection::WebConnection(Socket* socket) : IConnection()
{
	_isReading = false;
	_socket    = socket;
	_isClosed  = false;

	addEventListener(
		u::Network::Event::CLOSE
		, Callback(this, cb_cast(&WebConnection::onClose))
	);
}

/**
* Destructor.
*/
WebConnection::~WebConnection()
{
	removeEventListener(
		u::Network::Event::CLOSE
		, Callback(this, cb_cast(&WebConnection::onClose))
	);
	_socket->destroy();
}

/**
* Destroy this object.
*/
void WebConnection::destroy()
{
	delete (WebConnection*)this;
}

/**
* The class name.
*/
String WebConnection::className()
{
	return "Warp10::Network::WebConnection";
}

/**
* Create read thread.
*/
void WebConnection::read()
{
	Callback call(this, cb_cast(&WebConnection::read));
	ThreadSystem::create(&call);
}

/**
* Threaded read data.
*/
void WebConnection::read(Object* arg)
{
	lock();
	if(_isReading == true)
	{
		unlock();
		return;
	}

	ByteArray* buffer = new ByteArray();
	_isReading = true;
	Socket* socket = _socket;
	unlock();

	trace(className() + ":read: Start reading.");
	int64 l;
	do {
		l = _socket->read(buffer);
	} while(l != 0);
	trace(className() + "::read: Done.");

	lock();
	_isReading = false;
	unlock();

	error(className() + "::read: TODO event " + buffer->toString());

	buffer->destroy();
}

void WebConnection::send(ByteArray* data)
{
	lock();
	Socket* socket = _socket;
	unlock();

	data->position(0);
	while(data->available())
	{
		if(socket->send(data) == -1)
		{
			error(className()+"::send() Unable to send data.");
			error("TODO CLOSE");
			/**
			Callback cb(this, cb_cast(&NetworkDecoder::doClosed));
			ThreadSystem::create(&cb);
			*/
			return;
		}
	}
}

/**
* Close request received.
*/
void WebConnection::onClose(Object *arg)
{
	u::Network::Event* event = ((u::Network::Event*)arg);
	event->destroy();

	lock();
	if (_isClosed == false) {
		_socket->close();
	} else {
		// avoid double close
		unlock();
		return;
	}
	_isClosed  = true;
	unlock();

	u::Network::Event closed(u::Network::Event::CLOSED);
	dispatchEvent(&closed);
}