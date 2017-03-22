/*
* Socket.cpp
*
*  Created on: 13.07.2011
*      Author: Endre Bock
*/

#include <Socket>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>

#undef __FD_SETSIZE
#define	__FD_SETSIZE		102400

using namespace u;

u::Socket::Socket(
	int32 domain
	, String hostname
	, int64 port
	, int32 type
	, int32 protocol
)	: Object()
{
	init(domain, hostname, port, type, protocol);
}

void u::Socket::init(
	int32 domain
	, String hostname
	, int64 port
	, int32 type
	, int32 protocol
)
{
	//trace("Max connections: " + int2string(FD_SETSIZE));
	char reuse = 1;

	_type = type;
	_domain = domain;
	_protocol = protocol;
	_con = -1;
	_hostname = hostname;
	_port = port;
	maxWaitingClients = SOCKET_MAX_CLIENTS;
	receiveBufferSize = SOCKET_RECV_BUFFER;
	_neverUsed = true;
	_isSending = false;

	switch(_domain)
	{
		case AF_UNIX:
		{
			//trace("Create file socket at "+_hostname);
			_addr = new (struct sockaddr_un);

			memset(_addr, 0, sizeof(struct sockaddr_un));

			((struct sockaddr_un *)_addr)->sun_family = _domain;
			strcpy(
				((struct sockaddr_un *)_addr)->sun_path
				, _hostname.c_str()
			);
			_addrLen = sizeof(*((struct sockaddr_un *)_addr));

		} break;
		default:
		{
			_addr = new (struct sockaddr_in);
			_addrLen = sizeof(struct sockaddr_in);
			memset(_addr, 0, _addrLen);
			((struct sockaddr_in *)_addr)->sin_family = _domain;
			((struct sockaddr_in *)_addr)->sin_port = htons(_port);
		} break;
	}

	_descriptor = ::socket(_domain, _type, _protocol);
	::setsockopt(_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

	if(_descriptor == -1) error(className()+": Socket creation error.");
}

u::Socket::Socket(Socket & value) : Object(value)
{
	_type = value._type;
	_domain = value._domain;
	_protocol = value._protocol;
	_con = -1;
	_hostname = value._hostname;
	_port = value._port;
	maxWaitingClients = SOCKET_MAX_CLIENTS;
	receiveBufferSize = SOCKET_RECV_BUFFER;
	_neverUsed = true;
	_addr = null;
	_addrLen = 0;
	_descriptor = -1;
	_isSending = false;
	init(
		value._domain, value._hostname, value._port, value._type
		, value._protocol
	);
}

u::Socket::~Socket()
{
	destructor();
}

int32 u::Socket::descriptor()
{
	return _descriptor;
}

void u::Socket::destructor()
{
	if(isConnected()) close();
	lock();

	if(_neverUsed && _descriptor != -1)
	{
		::shutdown(_descriptor, SHUT_RDWR);
		::close(_descriptor);
	}

	switch(_domain)
	{
		case AF_UNIX:
		{
			delete ((struct sockaddr_un *)_addr);
		} break;
		default:
		{
			delete ((struct sockaddr_in *)_addr);
		} break;
	}
	unlock();
}

void u::Socket::destroy()
{
	delete (Socket*)this;
}

Socket& u::Socket::operator =(Socket &value)
{
	destructor();
	Socket(
		value._domain, value._hostname, value._port, value._type
		, value._protocol
	);
	if(value.isConnected())
	{
		connect();
	}
	return *this;
}

String u::Socket::className()
{
	return "u::Socket";
}

int32 u::Socket::getProtocolByName(String name)
{
	protoent *data;

	data = getprotobyname(name.c_str());

	return data->p_proto;
}

bool u::Socket::connect()
{
	if(isConnected()) return false;

	lock();
	switch(_domain)
	{
		case AF_UNIX:
		{
			_con = ::connect(
				_descriptor
				, (const sockaddr*)_addr
				, _addrLen
			);
		} break;
		case AF_INET:
		case AF_INET6:
		default:
		{
			struct addrinfo hints, *servinfo, *p;
			memset(&hints, 0, sizeof hints);
			hints.ai_family = _domain;
			hints.ai_socktype = _type;

			if(::getaddrinfo(_hostname.c_str(), int2string((int)_port).c_str(), &hints, &servinfo) != 0)
			{
				error(className()+"::connect: Can't resolv hostname "+_hostname+":"
					+int2string(_port));
				return false;
			}

			for(p = servinfo; p != NULL; p = p->ai_next)
			{
				((struct sockaddr_in *)_addr)->sin_addr = ((struct sockaddr_in*)p->ai_addr)->sin_addr;
				_con = ::connect(_descriptor, (const struct sockaddr*)_addr, _addrLen);
				if(isConnected())
				{
					break;
				}
			}

			freeaddrinfo(servinfo);
		} break;
	}
	unlock();
	return isConnected();
}

bool u::Socket::isConnected()
{
	lock();
	bool result = _con != -1;
	unlock();
	return result;
}

void u::Socket::close()
{
	if(!isConnected()) return;
	lock();
	_neverUsed = false;
	::shutdown(_descriptor, SHUT_RDWR);
	::close(_descriptor);
	_con = -1;
	_descriptor = -1;
	unlock();
}

bool u::Socket::listen()
{
	if(isConnected()) return false;

	lock();
	_con = ::bind(_descriptor, (const struct sockaddr*)_addr, _addrLen);
	unlock();

	if(!isConnected())
	{
		error(className()+"::listen: Socket bind error. ["+int2string(_descriptor)+"]");
		return false; // bind error
	}

	lock();
	_con = ::listen(_descriptor, maxWaitingClients);
	unlock();
	if(!isConnected())
	{
		error(className()+"::listen: Socket listen error. ["+int2string(_descriptor)+"]");
		return false;
	}

	return true;
}

int64 u::Socket::send(ByteArray *data)
{
	int64 len				// ammount of byte to send
		, ret					// number of sent bytes
		, pos					// position of the array pointer before send
	;
	char *buffer;		// send buffer

	if(!isConnected()) return -1;
	lock();
	if(_isSending == true) {
		unlock();
		return 0;
	}
	_isSending = true;

	pos = data->position();
	len = data->bytesAvailable();
	buffer = new char[len];
	len = data->readBytes(buffer, len);
	// MSG_NOSIGNAL protected an illegal memory exception on interupted sockets
	ret = ::send(_descriptor, buffer, len, MSG_NOSIGNAL);

	if(ret == -1)
	{
		error(className()+"::send: Sending failed.");
		data->position(pos);
	}
	else
	{
		data->position(pos+ret);
	}

	delete[] buffer;
	_isSending = false;
	unlock();

	trace(
		className() + "::send: Sent " + int2string(ret) + " bytes of "
		+ int2string(len) + " bytes."
	);
	return ret;
}

int64 u::Socket::read(ByteArray *data)
{
	int64 ret				// ammount of received bytes.
		, len					// maximum of read bytes
	;
	char *buffer;		// read buffer

	if(!isConnected()) return -1;

	if(!hasIncomingData()) return 0;

	// set pointer to end for append
	data->position(data->length());

	lock();
	len = receiveBufferSize;
	buffer = new char[len];

	ret = ::recv(_descriptor, buffer, len, 0);
	unlock();

	if(ret > 0)
	{
		data->writeBytes(buffer, ret);
	}

	delete[] buffer;

	data->position(0); // set pointer to begin

	trace(className()+"::read: Received "+int2string(ret)+" bytes.");
	return ret;
}

bool u::Socket::hasIncomingData()
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO(&rfds);
	FD_SET(_descriptor, &rfds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	lock();
	retval = select(_descriptor+1, &rfds, NULL, NULL, &tv);
	unlock();

	return retval > 0;

}

String u::Socket::hostip()
{
	if(!isConnected()) return "unknown";
	switch(_domain)
	{
		case AF_UNIX:
		{
			return "127.0.0.1";
		} break;
		default:
		{
			return inet_ntoa(((struct sockaddr_in*)_addr)->sin_addr);
		} break;
	}

	return "";
}

String u::Socket::hostname()
{
	return _hostname;
}

int64 u::Socket::port()
{
	return _port;
}

u::Socket* u::Socket::accept()
{
	Socket* clientSocket; 		// client socket data

	lock();
	int32 srcDescriptor = _descriptor;
	int32 domain = _domain;
	unlock();

	if(srcDescriptor == -1) return null;

	clientSocket = new Socket(domain);
	::close(clientSocket->_descriptor); // try to destroy socket..not needed

	clientSocket->_descriptor = ::accept(
		srcDescriptor
		, (struct sockaddr *)(clientSocket->_addr)
		, &(clientSocket->_addrLen)
	);

	if(clientSocket->_descriptor != -1)
	{
		clientSocket->_con = 1; // set manually to connected status
		return clientSocket;
	}

	clientSocket->destroy();
	return null;
}
