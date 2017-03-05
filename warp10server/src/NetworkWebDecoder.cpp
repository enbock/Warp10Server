#include <NetworkWebDecoder>
#include <NetTransferEvent>
#include <NetworkWebConnection>

using namespace u;

/**
 * Standard constructor.
 */
u::NetworkWebDecoder::NetworkWebDecoder() : NetworkDecoder()
{
	_lastLength = 0;
}

/**
 * Standard destructor.
 */
u::NetworkWebDecoder::~NetworkWebDecoder()
{
}

/**
 * Destroy the decoder.
 */
void u::NetworkWebDecoder::destroy()
{
	delete (NetworkWebDecoder*)this;
}

/**
 * Classname of the object.
 */
String u::NetworkWebDecoder::className()
{
	return "u::NetworkWebDecoder";
}

/**
 * Decode incomming data.
 * Is running in locked state. @see u::NetworkDecoder::doReadData
 */
void u::NetworkWebDecoder::decodeData(ByteArray *buffer)
{
	if (buffer->length() != _lastLength) {
		_lastLength = buffer->length();
		return;
	}

	_stopReading = true;
	buffer->position(0);
	_receivedData.writeBytes(*buffer);

	buffer->size(0); // reset buffer

	trace(className()+"::decodeData: " + _receivedData.toString());
	Callback cb(this, cb_cast(&NetworkWebDecoder::checkReceivedData));
	ThreadSystem::create(&cb);
}


/**
 * Encode response data.
 */
void u::NetworkWebDecoder::encodeResponse(ByteArray *buffer)
{
	buffer->position(0);
	ByteArray responseData;
	String header(
		String("HTTP/1.1\nContent-Type: text/plain\n")
		+ "Content-Length: " + int2string(buffer->available()) + "\n"
		+ "Connection: close\n\n"
	);
	responseData.writeBytes(header.c_str(), header.length());
	responseData.writeBytes(*buffer, buffer->available());

	send(&responseData);

	// close after send
	Callback cb(this, cb_cast(&NetworkWebDecoder::doClosed));
	ThreadSystem::create(&cb);
}

/**
* Start close procedure.
*/
void u::NetworkWebDecoder::sendClose()
{
	Callback cb(this, cb_cast(&NetworkWebDecoder::doClosed));
	ThreadSystem::create(&cb);
}


/**
* Send DATA event when connection is ready.
* The function is called from codeData and from connection ready event.
*/
void u::NetworkWebDecoder::checkReceivedData(Object *arg)
{
	_con->lock();
	if(((NetworkWebConnection*)_con)->isReady == false) {
		trace(
			className() + "::checkReceivedData: Not ready to send event yet."
		);
		_con->unlock();
		return;
	}
	_con->unlock();
	lock();
	if(_receivedData.length() == 0) {
		trace(
			className()
			+ "::checkReceivedData: Connect ready, but no data received yet."
		);
		unlock();
		return;
	}
	trace(
		className()
		+ "::checkReceivedData: Connect ready and data received: Send event."
	);
	NetTransferEvent data(
		NetTransferEvent::DATA, 0, &_receivedData
	);
	_con->room()->dispatchEvent(&data);
	unlock();
}