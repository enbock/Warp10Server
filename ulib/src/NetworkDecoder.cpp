/*
* NetworkDecoder.cpp
*
*  Created on: 06.08.2011
*      Author: Endre Bock
*/

#include <NetworkDecoder>
#include <Thread>
#include <NetworkConnection>
#include <NetEvent>

using namespace u;
#ifndef FPS
#define FPS 40
#endif

uint16 u::NetworkDecoder::_version = 0x0100;
String u::NetworkDecoder::wordHandshake("HANDSHAKE");
String u::NetworkDecoder::wordClose("CLOSE");
String u::NetworkDecoder::wordCloseConfirmed("CLOSE_CONFIRMED");

u::NetworkDecoder::NetworkDecoder()
{
	_con = null;
	_stopReading = false;
	_isReading = false;
}

u::NetworkDecoder::~NetworkDecoder()
{
	// stop and wait socket reading
	if(_isReading == true)
	{
		lock();
		_stopReading = true;

		while(_isReading == true)
		{
			unlock();
			usleep(1000000/FPS);
			lock();
		}

		unlock();
	}
}

void u::NetworkDecoder::connection(NetworkConnection* con)
{
	_con = con;
	Callback cb(this, cb_cast(&NetworkDecoder::doReadData));
	ThreadSystem::create(&cb);
}

void u::NetworkDecoder::destroy()
{
	delete (NetworkDecoder*)this;
}

String u::NetworkDecoder::className()
{
	return "u::NetworkDecoder";
}

void u::NetworkDecoder::doReadData(Object* arg)
{
	ByteArray* buffer = new ByteArray();
	lock();
	if(_isReading == true)
	{
		unlock();
		return;
	}

	_isReading = true;
	_stopReading = false;
	unlock();
	while(_stopReading == false)
	{
		//trace(className()+"::doReadData(): Start reading.");
		int64 l;
		lock();
		_con->_socket->lock();
		while((l = _con->_socket->read(buffer)) == 0 && _stopReading == false
				&& buffer->length() == 0
		)
		{
			//trace("Nothing to receive.");
			_con->_socket->unlock();
			unlock();
			usleep(1000000/FPS);
			lock();
			_con->_socket->lock();
		};
		_con->_socket->unlock();

		if(_stopReading == false)
		{
			//trace(className()+"::doReadData(): Start decoding.");
			decodeData(buffer);
			//trace ("Rest in buffer:" + int2string(buffer->available()));
			//trace ("Position in buffer:" + int2string(buffer->position()));
		}

		//trace(className()+"::doReadData(): Done. (Stop? "+(_stopReading==true?"Yes":"No")+")");
		unlock();
	}
	lock();
	_isReading = false;
	unlock();
	buffer->destroy();
}

void u::NetworkDecoder::send(ByteArray* data)
{
	int64 l;
	data->position(0);
	_con->_socket->lock();
	while(data->available())
	{
		l = _con->_socket->send(data);
		if(l == -1)
		{
			_con->_socket->unlock();
			error(className()+"::send() Unable to send data.");
			Callback cb(this, cb_cast(&NetworkDecoder::doClosed));
			ThreadSystem::create(&cb);
			return;
		}
	}
	_con->_socket->unlock();
}

void u::NetworkDecoder::doClosed(Object* arg)
{
	NetworkConnection* con;
	lock();
	con = _con;
	unlock();

	con->doClosed();
}

void u::NetworkDecoder::removeTimeout(Object* arg)
{
	Timeout* t = (Timeout*)(Object*)_timeoutList[*((const String*)arg)];
	delete &(_timeoutList[*((const String*)arg)]);
	t->destroy();
}
