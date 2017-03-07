#include <Network/Manager>

using namespace u;

/**
* Constructor.
*/
u::Network::Manager::Manager() : EventRoom()
{
	addEventListener(
		Network::Event::REGISTER_NETWORK
		, Callback(this, cb_cast(&Network::Manager::onRegisterNetwork))
	);
	addEventListener(
		Network::Event::REQUEST_LISTENER
		, Callback(this, cb_cast(&Network::Manager::onRequestListener))
	);
}

/**
* Destructor.
*/
u::Network::Manager::~Manager()
{
	/** future
	if (_listeners.size > 0) {
		error(className()+"::~Manager: Listeners must be closed on destroy.");
	}
	*/
	
	removeEventListener(
		Network::Event::REGISTER_NETWORK
		, Callback(this, cb_cast(&Network::Manager::onRegisterNetwork))
	);
	removeEventListener(
		Network::Event::REQUEST_LISTENER
		, Callback(this, cb_cast(&Network::Manager::onRequestListener))
	);
}

/**
* Destroyer.
*/
void u::Network::Manager::destroy()
{
	delete (Network::Manager*)this;
}

/**
* Get the class name.
*/
String u::Network::Manager::className()
{
	return "u::Network::Manager";
}

/**
* Register a new network.
*/
void Network::Manager::onRegisterNetwork(Object *arg)
{
	Network::Event* event = ((Network::Event*) arg);

	lock();
	if (_builder.hasKey(event->networkType) == false) {
		_builder[event->networkType] = event->builder;
		Network::Event response(
			Network::Event::NETWORK_REGISTERED
			, event->networkType
		);
		dispatchEvent(&response);
	}
	unlock();

	event->destroy();
}


/**
* Requesting a listener.
*/
void Network::Manager::onRequestListener(Object *arg)
{
	Network::Event* event = ((Network::Event*) arg);

	lock();
	if (_builder.hasKey(event->networkType) == true) 
	{
		/** TODO Next implementation ...
		Network::Listener* listener;
		listener = _builder[event->networkType]->createListener();
		_listeners.push(listener);
		listener->listen();

		listener.addEventListener(
			Network::Event::CLOSED
			, Callback(this, cb_cast(&Network::Manager:onListenerClosed))
		);

		Network::Event response(
			Network::Event::LISTENER_CREATED, listener
		);
		dispatchEvent(&response);
		*/
	}
	unlock();

	event->destroy();
}