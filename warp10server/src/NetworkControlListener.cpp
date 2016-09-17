/*
* NetworkControlListener
*
*  Created on: 06.08.2011
*      Author: Endre
*/

#include <NetworkControlListener>
#include <NetworkServiceControlPlugin>
#include <sys/poll.h>

using namespace u;

u::NetworkControlListener::NetworkControlListener()
	: NetworkListener()
{
}

u::NetworkControlListener::~NetworkControlListener()
{
	if(socketFile.length() > 0) unlink(socketFile.c_str());
}

String u::NetworkControlListener::className()
{
	return "u::NetworkControlListener";
}

void u::NetworkControlListener::destroy()
{
	delete (NetworkControlListener*)this;
}

NetworkConnection* u::NetworkControlListener::createIncomingConnection(
		Socket* connection
)
{
	return NetworkServiceControlPlugin::createIncomingConnection(connection);
}

Socket* u::NetworkControlListener::createSocket()
{
	if(socketFile.length() == 0) return null;

	Socket* sock = new Socket(PF_LOCAL, (String)socketFile);

	return sock;
}
