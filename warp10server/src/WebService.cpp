#include <WebService>
#include <ServerEvent>
#include <NetTransferEvent>
#include <ServerNetEvent>
#include <EventDispatcher>

using namespace u;


String u::WebService::toString()
{
	return "[" + className() + "]";
}

u::WebService::WebService() : NetworkService()
{
}

u::WebService::~WebService()
{
	if(_plugin.service()) {
		trace(this->className()+": Remove plugin from manager.");
		NetworkManager* srv = _plugin.service();
		srv->lock();
		srv->removeNetworkPlugin(&_plugin);
		srv->unlock();
	}
	removeListeners();
}

String u::WebService::className()
{
	return "u::WebService";
}

void u::WebService::destroy()
{
	delete (WebService*) this;
}

void u::WebService::addListeners()
{
	NetworkService::addListeners();
	_room->addEventListener(
		ServerEvent::NETWORK_PLUGIN_REGISTERED
		, Callback(this, cb_cast(&WebService::onPluginRegistered))
	);

	ServerEvent event(ServerEvent::REGISTER_NETWORK_PLUGIN);
	event.data = &_plugin;
	_room->dispatchEvent(&event);
}

void u::WebService::removeListeners()
{
	NetworkService::removeListeners();

	_room->removeEventListener(
		ServerEvent::NETWORK_PLUGIN_REGISTERED
		, Callback(this, cb_cast(&WebService::onPluginRegistered))
	);
}

/** Complete override */
void u::WebService::onPluginRegistered(Object* arg)
{
	ServerEvent* event = ((ServerEvent*) arg);
	if (event->data == &_plugin)
	{
		_room->dispatchEvent(
			new ServerNetEvent(
				ServerNetEvent::GET_WEB_LISTENER
				, "0.0.0.0"
				, 80
			)
		)->destroy();
	}
	
	event->destroy();
}

/** Complete override */
void u::WebService::onListenerFailed(Object* arg)
{
	ServerNetEvent *event = ((ServerNetEvent*) arg);
	if (!isMyListener(event))
	{
		event->destroy();
		return;
	}
	event->destroy();
	error("Can not entablish Web Service.");
}

void u::WebService::onNewData(Object* arg)
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

/**
 * Check if a incoming listener event really for me.
 */
bool u::WebService::isMyListener(NetEvent* event)
{
	// TODO: Address should be configured.
	// Is address a good way to identify the correct listener?
	return event->address() == "0.0.0.0"
		&& event->port() == 80
	;
}

/**
 * A new connection is comming in.
 */
void u::WebService::onNewConnection(Object *arg)
{
	NetEvent* event = ((NetEvent*) arg);
	String address  = event->address();
	int64 port      = event->port();
	EventRoom* room = event->room();
	NetworkService::onNewConnection(arg);

	NetEvent readySignal(
		NetEvent::CONNECTION_IS_READY, address, port
	);
	room->dispatchEvent(&readySignal);
}