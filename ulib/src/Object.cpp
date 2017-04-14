/*
* Object.cpp
*
*  Created on: 21.02.2011
*      Author: Endre Bock
*/

#include <algorithm>
#include "../include/Object"
#include "../include/Mutex"

using namespace u;

Object::Object() : Mutex()
{
	if(!__Object_mx_validObjects) __Object_mx_validObjects = new Mutex();
	if(!__Object_validObjects)
	{
		__Object_validObjects = new std::vector<
				void const*
		>();
	}

	__Object_mx_validObjects->lock();
	__Object_validObjects->push_back((void const*) this);
	__Object_mx_validObjects->unlock();
}

Object::~Object()
{
	lock();
	bool del = false;
	__Object_mx_validObjects->lock();
	int64 l, i;
	l     = __Object_validObjects->size();
	for(i = 0; i < l; i++)
	{
		if(__Object_validObjects->at(i) == (void const*) this)
		{
			__Object_validObjects->erase(__Object_validObjects->begin() + i);
			break;
		}
	}
	del   = __Object_validObjects->size() == 0;
	__Object_mx_validObjects->unlock();
	if(del)
	{
		delete __Object_validObjects;
		delete __Object_mx_validObjects;
		__Object_validObjects    = null;
		__Object_mx_validObjects = null;
	}
	unlock();
}

Object::Object(Object& value) : Mutex()
{
	if(!__Object_mx_validObjects) __Object_mx_validObjects = new Mutex();
	if(!__Object_validObjects)
	{
		__Object_validObjects = new std::vector<
				void const*
		>();
	}
	__Object_mx_validObjects->lock();
	__Object_validObjects->push_back((void const*) this);
	__Object_mx_validObjects->unlock();
}

String Object::int2string(int value)
{
	std::ostringstream v;
	v << value;
	return String(v.str());
}

String Object::ptr2string(Object* value)
{
	return ptr2string((void const*) value);
}

String Object::ptr2string(void const* value)
{
	std::ostringstream v;
	v << value;
	return String(v.str());
}

String Object::ptr2string(void(Object::*func)(Object*))
{
	std::ostringstream v;
	v << func;
	return String(v.str());
}

void Object::destroy()
{
	delete (Object*) this;
}


String::String()
		: Object()
		  , std::string()  // call of constructors is not needed, because always default constructor. First is we need argument to transport, we have to call this
{
};

String::String(const char* value) : Object(), std::string(value)
{
};

String::String(std::string value) : Object(), std::string(value)
{
};

String::String(size_t value) : Object(), std::string(int2string(value))
{
}

String::String(int value) : Object(), std::string(int2string(value))
{
}

String::String(const String& value)
		: Object(), std::string((std::string) value)
{
}

String::String(String& value) : Object(), std::string(value)
{
}

String::String(Object* value)
		: Object(), std::string(*dynamic_cast<String*>(value))
{
}


void String::replaceThisString(std::string value)
{
	std::string* str = this;
	*str = value;
}

/*String::~String()
{
};*/

String String::toString()
{
	return *this;
}

size_t String::length() const
{
	size_t
			l = ((std::string*)
			this
	)->length();
	return l;
}

String& String::operator=(const String& value)
{
	replaceThisString(value);
	return *this;
}

const char& String::operator[](size_t pos) const
{
	const std::string* str = this;
	return (*str)[pos];
};

char& String::operator[](size_t pos)
{
	std::string* str = this;
	return (*str)[pos];
};

int64 String::lastIndexOf(
		String
		val, int64 startIndex
)
{
	if(startIndex >

	   length()

			)
	{
		startIndex = length() - 1;
	}
	return
			find_last_of(
					val, startIndex
			);
}

String String::substr(int64 startIndex, int64 len)
{
	if(len > length() - startIndex) len = length() - startIndex;
	String ret = std::string::substr(startIndex, len);
	return ret;
}

String String::trimRight(const std::string& t)
{
	std::string            d(*this);
	std::string::size_type i(d.find_last_not_of(t));
	if(i == std::string::npos)
	{
		return String("");
	}
	else
	{
		return String(d.erase(d.find_last_not_of(t) + 1));
	}
}

String String::trimLeft(const std::string& t)
{
	std::string d(*this);
	return String(d.erase(0, ((std::string) *this).find_first_not_of(t)));
}

String String::trim(const std::string& t)
{
	return String((*this).trimRight(t).trimLeft(t));
}

void String::replace(
		String
		search, String replace
)
{
	std::string* s = this;
	while(s->
			       find(search)
	      != std::string::npos)
	{
		s->
				 replace(
				s
						->
								find(search)
				, search
						.

								length()
				, (

						const std::string&) replace
		);
	}
}

/**
* Explode string to list.
*/
Vector<String>* String::explode(
		String
		delimiter
)
{
	std::string* s = this;
	size_t position = 0;
	Vector<String>* result = new Vector<String>;
	while(position < length())
	{
		size_t found = s->find(delimiter, position);
		if(found == std::string::npos)
		{
			found = s->size();
		}
		result->
				      push(String(s->substr(position, found - position)));
		position = found + delimiter.length();
	}
	return
			result;
}

String Object::toString()
{
	String str = "[" + className() + "]";
	return str;
}

void String::destroy()
{
	delete (String*) this;
}

std::ostream& operator<<(std::ostream& os, Object& o)
{
	return os << (std::string) o.toString();
}

String Object::className()
{
	return "u::Object";
}

String String::className()
{
	return "u::String";
}

bool u::Object::valid(void const* ptr)
{
	__Object_mx_validObjects->lock();
	bool  ret = false;
	int64 l, i;
	l     = __Object_validObjects->size();
	for(i = 0; i < l; i++)
	{
		if(__Object_validObjects->at(i) == ptr)
		{
			ret = true;
			/*std::cout
				<< "\033[34;1mu::Object::valid(): " << ptr
				<< "\033[0m"
				<< std::endl
			;*/
			break;
		}
	}
	__Object_mx_validObjects->unlock();
	return ret;
}

String u::String::lowerCase()
{
	String output = *this;
	std::transform(output.begin(), output.end(), output.begin(), ::tolower);
	return output;
}

String u::String::upperCase()
{
	String output = *this;
	std::transform(output.begin(), output.end(), output.begin(), ::toupper);
	return output;
}
