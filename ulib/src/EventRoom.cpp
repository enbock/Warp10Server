/*
* EventRoom.cpp
*
*  Created on: 13.06.2011
*      Author: Endre Bock
*/

#include <EventRoom>

using namespace u;

u::EventRoom::EventRoom() : EventDispatcher()
{

}

void u::EventRoom::destroy()
{
	delete (EventRoom*)this;
}



u::EventRoom::~EventRoom()
{
	//trace(className()+": Destroy");
}


String u::EventRoom::className()
{
	return "u::EventRoom";

}

String u::EventRoom::toString()
{
	String out="["+className()+"<"+ptr2string(this)+"> Listeners:";
	int64 i,l;
	for(i=0, l=_eventList.length(); i < l; i++)
	{
		out+=" "+*(_eventList.at(i).type)+"(";
		int64 vi, vl;
		for(vi=0, vl=_eventList.at(i).list.length(); vi < vl; vi++)
		{
			out+=_eventList.at(i).list.at(vi).target->className();
			out+="<"+ptr2string(_eventList.at(i).list.at(vi).target)+">";
			if(vi+1 != vl) out+=" and ";
		}
		out+=")";
	}
	out+="]";
	return out;
}
