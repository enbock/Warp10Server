#include <Network/WebConnection>
#include <Network/Event>
#include <Network/WebRequest>
#include <Network/WebEvent>

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

	decodeBuffer(buffer);

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

/**
* Decode buffer and transform to request.
*/
void WebConnection::decodeBuffer(ByteArray* buffer)
{
	Vector<String> header;
	buffer->position(0);
	int64 startPosition = 0, position = 0;
	bool continueRead = true;
	// read header
	while (continueRead) {
		position++;
		buffer->position(position);
		char byte = buffer->readByte();
		if (byte == 0x0D || position + 1 == buffer->length()) {
			int64 length = position - startPosition ;
			if (length == 0) {
				// end of header
				continueRead = false;
				continue;
			}
			char* line = (char*)malloc(length + 1);
			if (!line) {
				error(
					className() + "::decodeBuffer: Out of memory."
				);
				return;
			}
			buffer->position(startPosition);
			buffer->readBytes(line, length, 0);
			line[length] = 0x00; // end byte
			String headerLine = String(line);
			headerLine.replace("\r", "_");
			header.push(headerLine);
			free(line);
			startPosition = position + 2;
			continueRead = startPosition < buffer->length();
		}
	}

#ifndef NDEBUG
	trace("");
	for(int64 i = 0, l = header.length(); i < l; i++) trace(header[i]);
	trace("");
#endif

	Vector<String>* commandParts = ((String)header[0]).explode(" ");

	if (commandParts->length() < 2) {
		error(className() + "::decodeBuffer: Invalid request.");
		
		u::Network::Event close(u::Network::Event::CLOSE);
		dispatchEvent(&close);
		delete commandParts;
		return;
	}
	
	WebRequest request;
	request.method = commandParts->at(0);
	request.resource = commandParts->at(1);
	if(commandParts->length() > 2)
		request.protocol = commandParts->at(2);
	delete commandParts;

	WebEvent requestEvent(WebEvent::REQUEST, this, request);
	dispatchEvent(&requestEvent);
}