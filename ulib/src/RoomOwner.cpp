/*
* RoomOwner.cpp
*
*  Created on: 13.06.2011
*      Author: Endre Bock
*/

#include <RoomOwner>

using namespace u;

u::RoomOwner::RoomOwner() : Object()
{

}

String u::RoomOwner::toString()
{
	String str = "[" + className() + "<" + ptr2string(this) + "> Listeners:";
	str += _room.toString();
	str += "]";
	return str;

}


u::RoomOwner::~RoomOwner()
{
	//trace(className()+": Destroy...");
}


RoomOwner& u::RoomOwner::operator=(RoomOwner& value)
{
	_room = value._room;
	return *this;
}


EventRoom* u::RoomOwner::room()
{
	return &_room;
}


u::RoomOwner::RoomOwner(RoomOwner& value)
{
	operator=(value);
}


String u::RoomOwner::className()
{
	return "u::RoomOwner";
}


void u::RoomOwner::destroy()
{
	delete (RoomOwner*) this;
}


