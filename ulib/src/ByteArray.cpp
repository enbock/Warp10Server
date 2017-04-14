/**
* ByteArray.
* $Id: $
*/

#include <ByteArray>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <string.h>
#include <algorithm>
#include "zlib.h"
#include "zconf.h"

using namespace u;

/**
* ByteArray
*/
ByteArray::ByteArray() : DataArray()
{
	createMemory();
}

void ByteArray::createMemory()
{
	uint64 i;
	_size     = BYTE_ARRAY_CHUNK;
	_buffer   = (char*) malloc(_size);
	_position = 0;
	_length   = 0;
	if(_buffer == null) error("ByteArray::ByteArray(): Out of memory.");
}

/**
* ~ByteArray
*/
ByteArray::~ByteArray()
{
	free(_buffer);
}

/**
* copy constructor
*/
ByteArray::ByteArray(const ByteArray& value)
{
	createMemory();
	cloneFrom(value);
}

/**
* cloneFrom
*/
void ByteArray::cloneFrom(const ByteArray& value)
{
	size(value._length);
	_position = 0;
	writeBytes(value._buffer, value._length, 0);
	_length   = value._length;
	_position = value._position;
}

/**
* equal operator
*/
const ByteArray& ByteArray::operator=(const ByteArray& value)
{
//	trace("op=!!!" << ptr2string(value._buffer) << "|" << ptr2string(_buffer));
	cloneFrom(value);

	return value;
}

/**
* toString
*/
String ByteArray::toString()
{
	String str = "[ByteArray size=";
	str += int2string(_size);
	str += " pointer=" + int2string(_position);
	str += " length=" + int2string(_length);
	str += " memory=" + int2string(_size);
	str += "\n";

	static int  lineLength = 16;
	static char hex[17]    = "0123456789ABCDEF";

	uint64        i;
	uint          c;
	char          o[lineLength + 1];
	char          l[3];
	unsigned char v;
	char          d[10], f[20];

	c     = 0;
	sprintf(f, " 0x%%%02dx:", (int) sizeof(uint64));
	sprintf(d, f, 0/*+_buffer*/);
	str += d;
	for(i = 0; i < _length and i < 1024 * 1024 * 10/*cap on 10MB*/; i++)
	{
		if(c == 0) str += " ";
		v = _buffer[i];
		l[0]     = hex[v >> 4]; // >>4 is identical of /16
		l[1]     = hex[v - ((v >> 4) << 4)]; // <<4 is dientical of *16
		l[2]     = 0;

		str += l;
		if(c % 2 == 1) str += " ";
		if(v > 31 && v < 127)
		{
			o[c] = _buffer[i];
		}
		else
		{
			o[c] = *".";
		}
		o[c + 1] = 0;

		do
		{
			c++;
			if(c == lineLength)
			{
				o[c] = 0;
				str += "  ";
				str += o;
				str += "\n";

				sprintf(d, f, i + 1);
				if(i + 1 < _length)
				{
					str += d;
				}
				c    = 0;
			}
			if(c % 8 == 0 && c > 2) str += " ";
			if(i + 1 == _length)
			{
				if(c) str += "  ";
				if(c % 2 == 1) str += " ";
			}
		} while(i + 1 == _length && c != 0);
	}

	str += "]";
	return str;
}

/**
* writeBinary
*/
void ByteArray::writeBinary(const ByteArray& data)
{
	writeUInt(data._length);
	writeBytes(data._buffer, data._length, 0);
}

/**
* set length
*/
void ByteArray::length(int64 value)
{
	size(value);
}

/**
* setSize
*/
void ByteArray::size(uint64 value)
{
	uint64 newLength, i;
	char* newBuffer;

	_length = value;

	newLength = value + (BYTE_ARRAY_CHUNK - (value % BYTE_ARRAY_CHUNK));
	if(newLength == _size) return;

	newBuffer = (char*) realloc(_buffer, newLength);
	if(_buffer == null) error("ByteArray::setSize(): Out of memory.");

	_size   = newLength;
	_buffer = newBuffer;
	position(_position); // correct pointer if needed
}

/**
* size
*/
uint64 ByteArray::size() const
{
	return _size;
}

/**
* writeByte
*/
void ByteArray::writeByte(const unsigned char value)
{
	// extend the memory
	if(_size <= _position + 1) size(_position + 1);
	_buffer[_position++] = value;
	if(_length < _position) _length = _position;
}

/**
* writeInt
*/
void ByteArray::writeInt(const int64 value, int64 min, bool noHeader)
{
	char   type;
	uint64 val;

	if(value < 0)
	{
		type = TYPE_NEGATIVE_INT;
		val  = (uint64) (value * (uint64) -1);
	}
	else
	{
		type = TYPE_POSITIVE_INT;
		val  = (uint64) value;
	}
	writeUInt(val, min, noHeader, type);
}

/**
* writeUInt
*/
void
ByteArray::writeUInt(const uint64 value, int64 min, bool noHeader, int8 type)
{
	char maxExp, byte;
	int  i;

	maxExp = min;

	while((value >> (8 * maxExp)) > 0 && maxExp < sizeof(uint64))
	{ maxExp++; }
	//maxExp = (double)(log((double)value)/log((double)256));

	if(noHeader == false)
	{
		writeByte(maxExp);
		writeByte(type);
	}

	for(i = maxExp - 1; i >= 0; i--)
	{
		byte = (value >> (8 * i)) & 0xFF;
		writeByte(byte);
	}
}

/**
* readBinary
*/
ByteArray ByteArray::readBinary()
{
	ByteArray dest;
	uint64    length = this->readInt();
	dest.size(length);
	readBytes(dest._buffer, length, 0);
	return dest;
}

/**
* readByte
*/
unsigned char ByteArray::readByte()
{
	if(_position >= _length) return 0;
	return _buffer[_position++];
}

/**
* readInt
*/
uint64 ByteArray::readInt(int64 min, int8* type)
{
	int    t, i;
	uint64 len, out;
	int64  lout;

	len = min;
	t   = TYPE_UINT;
	if(len == 0)
	{
		len = (uint64) readByte();
		t   = (int) readByte();
	}

	out   = 0;
	for(i = len - 1; i >= 0; i--)
	{
		out += (uint64) readByte() << (8 * i);
	}

	if(type != null) *type = t;

	switch(t)
	{
		case TYPE_NEGATIVE_INT:
		{
			lout = (int64) out;
			lout *= -1;
			return lout;
		}
			break;
		case TYPE_POSITIVE_INT:
		{
			return (int64) out;
		}
			break;
	}

	return out;
}

/**
* writeString
*/
void ByteArray::writeString(const String& value)
{
	size_t length = value.length();
	writeInt(length);
	writeBytes(value.data(), length);
}

/**
* writeBytes
*/
int64 ByteArray::writeBytes(const char* bytes, int64 length, int64 offset)
{
	int64 ret = 0;
	if(_position + length >= _size) size(_position + length);
	memcpy(_buffer + _position, bytes + offset, length);
	_position += length;
	if(_length < _position) _length = _position;
	return length;
}

int64 ByteArray::writeBytes(ByteArray& value, int64 length)
{
	int64 offset = value.position();
	return writeBytes(
			value._buffer, (uint64) (
					length
					? length
					: value.available()), offset
	);
}

/**
* readString
*/
String ByteArray::readString()
{
	size_t length;
	char* str;
	length = readInt();
	str    = (char*) malloc(length + 1);
	if(str)
	{
		readBytes(str, length);
		str[length] = 0;
		String out(str);
		free(str);
		return out;
	}
	return (String) "";
}

/**
* readBytes
*/
int64 ByteArray::readBytes(char* bytes, int64 length, int64 offset)
{
	size_t len = length;
	if(len + _position > _length) len = _length - _position;
	memcpy(bytes + offset, _buffer + _position, len);
	_position += len;
	return len;
}

/**
* writeFloat
*/
void ByteArray::writeFloat(float value)
{
	size_t l = sizeof(float);
	char   bin[l];
	memcpy(&bin, &value, l);
	std::reverse(bin, bin + l);
	if(_position + l >= _size) size(_position + l);
	writeBytes(bin, l);
}

/**
* readFloat
*/
float ByteArray::readFloat()
{
	size_t l = sizeof(float);
	char   bin[l];
	float  out;
	readBytes(bin, l);
	std::reverse(bin, bin + l);
	memcpy(&out, &bin, l);
	return out;
}

/**
* compress
*/
void ByteArray::compress()
{
	Bytef* compr;
	ulong cl;
	int   err;
	ByteArray* ba = new ByteArray();
	ba->writeUInt(_length);

	cl    = compressBound(_length);
	compr = (Bytef*) malloc(cl + ba->length());
	ba->position(0);
	ba->readBytes((char*) compr, ba->_length, 0);
	err = compress2(
			compr + ba->length()
			, &cl
			, (const Bytef*) _buffer
			, _length
			, Z_BEST_COMPRESSION
	);

	if(err != Z_OK)
	{
		assert(err == Z_OK && "ByteArray::compress(): Compression error.");
		free(compr);
		return;
	}
	size(cl);
	_length = 0;
	position(0);
	writeBytes((const char*) compr, cl + ba->length(), 0);
	delete ba;
	free(compr);
}

/**
* compress
*/
void ByteArray::uncompress()
{
	char* ucompr;
	ulong  cl;
	uint64 off, sl;
	int    err;

	_position = 0;
	cl        = (uint64) readInt();
	off       = _position;
	sl        = _length - off;
	ucompr    = (char*) malloc(cl);
	err       = ::uncompress((Bytef*) ucompr
	                         , (uLongf*) &cl
	                         , (const Bytef*) _buffer + off
	                         , (uLong) sl
	);
	if(err != Z_OK)
	{
		assert(err == Z_OK && "ByteArray::uncompress(): Decompression error.");
		free(ucompr);
		return;
	}
	size(cl);
	position(0);
	_length = 0;
	writeBytes((const char*) ucompr, cl, 0);
	free(ucompr);
}

uint64 ByteArray::bytesAvailable() const
{
	return available();
}

int64 ByteArray::writeData(DataArray& bytes, int64 length)
{
	return writeBytes((ByteArray&) bytes, length);
}


/**
* setPosition
*/
void ByteArray::position(int64 value)
{
	if(value < _length)
	{ _position = value; }
	else
	{ _position = _length; }
}

/**
* getPosition
*/
int64 ByteArray::position() const
{
	return _position;
}


uint64 ByteArray::available() const
{
	return _length - _position;
}

/**
* getLength
*/
int64 ByteArray::length() const
{
	return _length;
}

int64 ByteArray::readBytes(ByteArray* bytes, int64 length)
{
	int64 l = bytes->writeBytes(*this, length);
	position(_position + l);
	return l;
}

bool u::ByteArray::splice(int64 offset, int64 length)
{
	int64 oldSize = this->length();
	position(offset);
	int64 i;
	for(i = 0; i < oldSize - length - offset; i++)
	{
		if(offset + i + length < oldSize)
		{
			_buffer[offset + i] = _buffer[offset + i + length];
		}
		else
		{
			break;
		} // end of data block before offset+length
	}
	int64 newSize = this->length() - length;
	if(newSize < 0) newSize = 0;
	size((uint64) newSize);
	return true;
}

String u::ByteArray::className()
{
	return "u::ByteArray";
}
