
#include <NetworkWebListener>
#include <NetworkServiceWebPlugin>

using namespace u;

u::NetworkWebListener::NetworkWebListener()
	: NetworkListener()
{
	socketPort = 80;
}

u::NetworkWebListener::~NetworkWebListener()
{
	socketAddress = "";
	trace(className()+"::~: Destroyed");
}

String u::NetworkWebListener::className()
{
	return "u::NetworkWebListener";
}

void u::NetworkWebListener::destroy()
{
	delete (NetworkWebListener*)this;
}

NetworkConnection* u::NetworkWebListener::createIncomingConnection(
		Socket* connection
)
{
	return NetworkServiceWebPlugin::createIncomingConnection(connection);
}

Socket* u::NetworkWebListener::createSocket()
{
	if(socketAddress.length() == 0) return null;

	Socket* sock = new Socket(AF_INET, (String)socketAddress, socketPort);

	return sock;
}
