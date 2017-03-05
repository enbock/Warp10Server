/*
 * Callback.cpp
 *
 *  Created on: 02.03.2011
 *      Author: Endre Bock
 */

#include <Callback>

using namespace u;

Callback::Callback(Object* obj, void(Object::*func)(Object*), Object *argument)
	: Object()
{
	target = obj;
	function = func;
	arg = argument;
}

Callback::Callback() : Object()
{
	target = null;
	function = null;
	arg = null;
}

/*
Callback& Callback::operator=(Callback& value)
{
	target = value.target;
	function = value.function;
	arg = value.arg;

	return value;
}
*/
Callback::Callback(const Callback& value)
{
	target = value.target;
	function = value.function;
	arg = value.arg;
}


void Callback::call(void)
{
	if(valid(target))
	{
		(target->*function)(arg);
	}
}

bool Callback::operator==(const Callback& other) const
{
	bool ret = true;
	if(other.target != this->target) ret = false;
	if(ret && other.function != this->function) ret = false;
	if(ret && other.arg != this->arg) ret = false;
	return ret;
}

void Callback::destroy()
{
	delete (Callback*)this;
}

String Callback::toString()
{
	String str = "[" + className() + " (" + ptr2string(this) + ")";
	str += " target=" + (target?target->toString():"<Defunc>");
	//str += " function=" + (function?ptr2string(&function):"<Defunc>");
	//str += " arg=" + (arg?arg->toString():"<No Argument>");
	str +="]";
	return str;
}

String Callback::className()
{
	return "Callback";
}
