/*
 * RoomClient.cpp
 *
 *  Created on: 15.07.2011
 *      Author: Endre Bocl
 */

#include <RoomClient>

using namespace u;

u::RoomClient::RoomClient()
		: Object()
{
	_room = null;
}

u::RoomClient::RoomClient(RoomClient& value)
		: Object(value)
{
	operator=(value);
}

u::RoomClient::~RoomClient()
{
	doDestruct();
}

RoomClient& u::RoomClient::operator =(RoomClient& value)
{
	room(value._room);
	return *this;
}

void u::RoomClient::destroy()
{
	delete (RoomClient*) this;
}

String u::RoomClient::className()
{
	return "u::RoomClient";
}

void u::RoomClient::room(EventRoom* value)
{
	//trace(className()+"("+ptr2string(this)+"):room("
	//	+ptr2string(value)+") old: "+ptr2string(_room));
	if (value != _room)
	{
		if (_room != null) removeListeners();
		_room = value;
		if (_room != null) addListeners();
	}
}

void u::RoomClient::doDestruct()
{
	//Object::doDestruct();
	room(null);
}
