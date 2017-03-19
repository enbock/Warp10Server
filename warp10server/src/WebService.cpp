#include <WebService>

using namespace u;
using namespace Warp10;

String WebService::type = "http";

WebService::WebService(Manager* manager, String address, int64 port) : Object()
{
	_manager = manager;
	_address = address;
	_port   = port;

	_builder.setupListener(_address, _port);

	_manager->addEventListener(
		u::Network::Event::NETWORK_REGISTERED,
		Callback(this, cb_cast(&WebService::onNetworkRegistered))
	);
	_manager->addEventListener(
		u::Network::Event::LISTENER_CREATED,
		Callback(this, cb_cast(&WebService::onListenerCreated))
	);
}

WebService::~WebService()
{
	_manager->removeEventListener(
		u::Network::Event::NETWORK_REGISTERED,
		Callback(this, cb_cast(&WebService::onNetworkRegistered))
	);
	_manager->removeEventListener(
		u::Network::Event::LISTENER_CREATED,
		Callback(this, cb_cast(&WebService::onListenerCreated))
	);
}

String WebService::className()
{
	return "Warp10::WebService";
}

void WebService::destroy()
{
	delete (WebService*) this;
}

/**
* Network manager has registered builder.
*/
void WebService::onNetworkRegistered(Object* arg)
{
	u::Network::Event* event = ((u::Network::Event*)arg);
	if (event->networkType == type) {
		u::Network::Event requestListener(
			u::Network::Event::REQUEST_LISTENER, type
		);
		_manager->dispatchEvent(&requestListener);
	}
	event->destroy();
}

/**
* Network manager the listener created.
*/
void  WebService::onListenerCreated(Object* arg)
{
	u::Network::Event* event = ((u::Network::Event*)arg);
	if (event->networkType == type) {
		trace(className() + ": Listener ready.");
	}
	event->destroy();
}

void WebService::registerNetwork()
{
	u::Network::Event registerNetwork(
		u::Network::Event::REGISTER_NETWORK, type, &_builder
	);
	_manager->dispatchEvent(&registerNetwork);
}

/*
void WebService::onNewData(Object* arg)
{
	NetTransferEvent* event = ((NetTransferEvent *) arg);
	EventDispatcher* sourceDispatcher = event->target();
	
	trace(this->className()+": Incoming data " + event->data()->toString());

	String dummy("TODO answer\n");
	
	ByteArray responseData;
	responseData.writeBytes(dummy.c_str(), dummy.length());

	NetTransferEvent responseEvent(NetTransferEvent::EOT, 0, &responseData);
	sourceDispatcher->dispatchEvent(&responseEvent);

	event->destroy();
}
*/