/*
* NetTransferEvent.cpp
*
*  Created on: 04.05.2013
*      Author: Endre Bock
*/

#include <NetTransferEvent>


using namespace u;

const String u::NetTransferEvent::INIT("u::NetTransferEvent::init");
const String u::NetTransferEvent::READY("u::NetTransferEvent::ready");
const String u::NetTransferEvent::DATA("u::NetTransferEvent::data");
const String u::NetTransferEvent::EOT("u::NetTransferEvent::end_of_transfer");

u::NetTransferEvent::NetTransferEvent(
	String const &type, uint64 id, ByteArray *data
)	: Event(type)
{
	_id = id;
	if(data != null)
	{
		data->lock();
		_data = *data;
		_data.position(0);
		data->unlock();
	}
}

u::NetTransferEvent::~NetTransferEvent()
{
}

String u::NetTransferEvent::className()
{
	return "u::NetTransferEvent";
}

void u::NetTransferEvent::destroy()
{
	delete (NetTransferEvent*)this;
}

uint64 u::NetTransferEvent::id()
{
	return _id;
}

Event* u::NetTransferEvent::clone()
{
	return new NetTransferEvent(*_type, _id, _data.length() > 0 ? &_data : null);
}

ByteArray* u::NetTransferEvent::data()
{
	return &_data;
}

String u::NetTransferEvent::toString()
{
	String str = "["+className()+" ";
	str += "Event:"+(*_type)+ " ";
	str += "Data:"+_data.toString() + "]";
	return str;
}
