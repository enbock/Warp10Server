/*
 * Datablock.cpp
 *
 *  Created on: 31.03.2011
 *      Author: Endre Bock
 */

#include <Datablock>

using namespace u;

Datablock::Datablock() : Object()
{

}

Datablock::Datablock(Datablock & value)
{
	*this = value;
}

Datablock& Datablock::operator=(Datablock& value)
{
	deleteAttributes();

	int64 i, l;
	l = value._list.size();
	for(i=0; i<l; i++)
	{
		saveAttribute(value._list[i]);
	}
	return *this;
}

void Datablock::destroy()
{
	delete (Datablock*) this;
}

Datablock::~Datablock()
{
	deleteAttributes();
}

void Datablock::deleteAttributes()
{
	_attribute * entry;
	while(!_list.empty())
	{
		//trace("Remove a entry.");
		entry = _list.back();
		deleteAttribute(entry);
		entry->destroy();
	}
}

String Datablock::className()
{
	return "u::Datablock";
}

Datablock::_attribute & Datablock::operator[](String name)
{
	int64 i,l;
	_attribute *entry;
	l = _list.size();
	for(i=0; i<l; i++)
	{
		entry = _list[i];
		if(entry->name == name) return *entry;
	}
	entry = new _attribute(name);

	saveAttribute(entry);

	return *entry;
}

void Datablock::saveAttribute(Datablock::_attribute *entry)
{
	entry->ref.push_back(this);
	_list.push_back(entry);
}

void Datablock::deleteAttribute(Datablock::_attribute *entry)
{
	int64 i,l;

	l=_list.size();
	for(i=0; i<l; i++)
	{
		if(_list[i] == entry)
		{
			_list.erase(i);
			break;
		}
	}

	l = entry->ref.size();
	for(i=0; i<l; i++)
	{
		if(entry->ref[i] == this)
		{
			entry->ref.erase(i);
			break;
		}
	}
}

String Datablock::toString()
{
	String str = "[" + className();
	str += " length=" + int2string(_list.size());
	str += "]";
	return str;
}

Datablock::_attribute::operator Object*()
{
	return (Object*)value;
}

Datablock::_attribute::operator String*()
{
	return (String*)value;
}

Datablock::_attribute::operator Datablock*()
{
	return (Datablock*)value;
}

Datablock::_attribute::operator int64()
{
	return *((int64*)value);
}

Object *Datablock::_attribute::operator =(Object *value)
{
	destroyValue();
	isObject = true;
	this->value = value;
	//trace("Save as Object.");
	return value;
}

int64 Datablock::_attribute::operator =(int64 value)
{
	destroyValue();
	isObject = false;
	this->value = new int64();
	*((int64*)this->value) = value;

	return value;
}

void Datablock::_attribute::destroy()
{
	if(ref.empty()) destroyValue();
	//else trace("Ref not empty.");
}

void Datablock::_attribute::destroyValue()
{
	if(value != null)
	{
		if(isObject == true)
		{
			//trace("Destroy object.");
			if(valid(value)) ((Object*)value)->destroy();
		}
		else
		{
			//trace("Destroy a int.");
			delete (int64*)value;
		}
		value = null;
		if(_noDelete == false) delete (_attribute*)this;
	}
}

u::Datablock::_attribute::_attribute(String keyName)
{
	_noDelete = false;
	value = null;
	isObject = false;
	name = keyName;
}

u::Datablock::_attribute::~_attribute()
{
	_noDelete = true;
	while(!ref.empty())
	{
		ref[0]->deleteAttribute(this);
	}
}

bool u::Datablock::hasKey(String key)
{
	int64 l = _list.length(), i;
	for(i = 0; i < l; i++)
	{
		if(_list[i]->name == key) return true;
	}

	return false;
}

/**
* Get the list of keys.
*/
Vector<String> u::Datablock::getKeys()
{
	Vector<String> result;
	int length = _list.length(), i;
	for(i = 0; i < length; i++)
	{
		result.push(_list[i]->name);
	}

	return result;
}

/**
* Delete a key, if the key exists.
*/
void u::Datablock::remove(String key)
{
	int64 l = _list.length(), i;
	for(i = 0; i < l; i++)
	{
		if(_list[i]->name == key) {
			delete (_attribute*)_list[i];
			return;
		}
	}
}