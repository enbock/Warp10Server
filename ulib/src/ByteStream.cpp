

#include <ByteStream>

using namespace u;

void ByteStream::position(int64 value)
{
	_buffer.position(value);
}

int64 ByteStream::position() const
{
	return _buffer.position();
}

uint64 ByteStream::available() const
{
	return _buffer.available();
}

int64 ByteStream::length() const
{
	return _buffer.length();
}

void ByteStream::length(int64 value)
{
	_buffer.length(value);
}

void ByteStream::destroy()
{
	delete (ByteStream*) this;
}

bool ByteStream::splice(int64 offset, int64 length)
{
	return false;
}

ByteStream::ByteStream(uint64 size) : DataArray()
{
	_buffer.size(size);
	_startPosition = 0;
	_used          = 0;
}

String ByteStream::className()
{
	return "u::ByteStream";
}

ByteStream::~ByteStream()
{
}

ByteStream::ByteStream(const ByteStream& src) : DataArray()
{
	_buffer        = src._buffer;
	_startPosition = src._startPosition;
	_used          = src._used;
}

ByteStream::ByteStream() : ByteStream(BYTESTREAM_DEFAULT_SIZE)
{

}

int64 ByteStream::writeData(DataArray& bytes, int64 length)
{
	return writeData((ByteArray&) bytes, length);
}

int64 ByteStream::writeData(ByteArray& bytes, int64 length = 0)
{
	int64  writtenBytes  = 0
	,      lengthToWrite = length;
	uint64 startAt       = _startPosition + _used;

	if(lengthToWrite == 0 || lengthToWrite > free())
	{
		lengthToWrite = free();
	}
	if(lengthToWrite == 0)
	{
		return 0;
	}
	if(lengthToWrite > bytes.available())
	{
		lengthToWrite = bytes.available();
	}
	if(startAt > _buffer.length())
	{
		startAt -= _buffer.length();
	}
	_buffer.position(startAt);
	// If write target beyond size, then write piece to end of size and rewind.
	if(startAt + lengthToWrite > _buffer.length())
	{
		writtenBytes = _buffer.writeData(
				bytes, _buffer.length() - startAt
		);
		lengthToWrite -= writtenBytes;
		_buffer.position(0);
		// move source pointer
		bytes.position(bytes.position() + writtenBytes);
	}
	// write data to buffer.
	int64 rest = _buffer.writeBytes(bytes, lengthToWrite);
	writtenBytes += rest;
	_used += writtenBytes;

	// move source pointer
	bytes.position(bytes.position() + rest);

	return writtenBytes;
}

int64 ByteStream::readData(ByteArray* bytes, int64 length)
{
	int64 readBytes = 0, lengthToRead = length;

	if(lengthToRead == 0 || lengthToRead > _used)
	{
		lengthToRead = _used;
	}
	if(lengthToRead == 0)
	{
		return 0;
	}
	_buffer.position(_startPosition);
	// If read target beyond length of buffer, then read piece and rewind.
	if(_startPosition + lengthToRead > _buffer.length())
	{
		readBytes = _buffer.readBytes(bytes, _buffer.length() - _startPosition);
		lengthToRead -= readBytes;
		_buffer.position(0);
	}
	readBytes += _buffer.readBytes(bytes, lengthToRead);
	_startPosition += readBytes;
	if(_startPosition > _buffer.length())
	{
		_startPosition -= _buffer.length();
	}
	_used -= readBytes;

	return readBytes;
}

uint64 ByteStream::free()
{
	return _buffer.length() - _used;
}

String ByteStream::toString()
{
	return "["
	       + className()
	       + ": start="
	       + int2string(_startPosition)
	       + " used="
	       + int2string(_used)
	       + " "
	       + _buffer.toString()
	       + "]";
}

int64 ByteStream::writeData(ByteArray& bytes)
{
	return writeData(bytes, 0);
}

int64 ByteStream::readData(ByteArray* bytes)
{
	return readData(bytes, 0);
}
