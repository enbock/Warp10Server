#include <Network/Manager>
#include <Network/IBuilder>
#include <Network/IListener>

using namespace u;
using namespace Network;

/**
* Constructor.
*/
Manager::Manager() : EventRoom()
{
	_isClosing = false;
	addEventListener(
		Event::REGISTER_NETWORK
		, Callback(this, cb_cast(&Network::Manager::onRegisterNetwork))
	);
	addEventListener(
		Event::REQUEST_LISTENER
		, Callback(this, cb_cast(&Network::Manager::onRequestListener))
	);
	addEventListener(
		Event::CLOSE
		, Callback(this, cb_cast(&Network::Manager::onCloseRequest))
	);
}

/**
* Destructor.
*/
Manager::~Manager()
{
	if (_listener.size() > 0) {
		error(
			className()
			+ "::~Manager: Listeners must be closed before destroy."
		);
	}
	
	removeEventListener(
		Event::REGISTER_NETWORK
		, Callback(this, cb_cast(&Network::Manager::onRegisterNetwork))
	);
	removeEventListener(
		Event::REQUEST_LISTENER
		, Callback(this, cb_cast(&Network::Manager::onRequestListener))
	);
	removeEventListener(
		Event::CLOSE
		, Callback(this, cb_cast(&Network::Manager::onCloseRequest))
	);

	trace(className() + "::~Manager: Down.");
}

/**
* Destroyer.
*/
void Manager::destroy()
{
	delete (Network::Manager*)this;
}

/**
* Get the class name.
*/
String Manager::className()
{
	return "u::Network::Manager";
}

/**
* Register a new network.
*/
void Manager::onRegisterNetwork(Object *arg)
{
	Event* event = ((Event*) arg);

	lock();
	if (_builder.hasKey(event->networkType) == false) {
		_builder[event->networkType] = event->builder;
		Event response(
			Event::NETWORK_REGISTERED
			, event->networkType
		);
		unlock();
		dispatchEvent(&response);
	} else {
		unlock();
	}

	event->destroy();
}

/**
* Requesting a listener.
*/
void Manager::onRequestListener(Object *arg)
{
	Event* event = ((Event*) arg);

	lock();
	if (_builder.hasKey(event->networkType) == true && _isClosing == false) 
	{
		Network::IListener* listener;
		listener = ((IBuilder*)((Object *)_builder[event->networkType]))
			->createListener();
		trace(
			className() + "::onRequestListener: Create " + listener->toString()
		);
		_listener.push(listener);

		listener->addEventListener(
			Event::CLOSED
			, Callback(this, cb_cast(&Network::Manager::onListenerClosed))
		);

		Event response(
			Event::LISTENER_CREATED, event->networkType, listener
		);
		unlock();
		dispatchEvent(&response);
	} else {
		unlock();
	}

	event->destroy();
}

/**
* Listener was closed.
*/
void Manager::onListenerClosed(Object *arg)
{
	Event* event = ((Event*) arg);
	lock();
	Network::IListener* listener = ((Network::IListener*) event->target());
	if (_listener.erase(listener) != -1) {
		// Destroy only when owned.
		listener->removeEventListener(
			Event::CLOSED
			, Callback(this, cb_cast(&Network::Manager::onListenerClosed))
		);
		listener->destroy();

		unlock();
		checkForCloseState();
	} else {
		unlock();
	}
	event->destroy();
}

/**
* Request to close all listeners.
*/
void Manager::onCloseRequest(Object *arg)
{
	trace(className() + ": Start close procedure.");
	arg->destroy();
	lock();
	if(_isClosing == false) {
		uint64 i, length = _listener.length();
		Event close(Event::CLOSE);
		for (i = 0; i < length; i++) {
			_listener[i]->dispatchEvent(&close);
		}
	}
	_isClosing = true;
	unlock();
	checkForCloseState();
}

/**
* Checking if all listeners removed.
*/
void Manager::checkForCloseState()
{
	lock();
	if (_listener.length() == 0) {
		trace(className() + ": All closed.");
		Event closed(Event::CLOSED);
		unlock();
		dispatchEvent(&closed);
	} else {
		unlock();
	}
}