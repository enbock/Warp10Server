/*
 * tEvent.cpp
 * Event tests.
 *  Created on: 09.04.2011
 *      Author: Endre
 */

#include <EventDispatcher>
#include <Callback>

namespace u
{
	void programExit();

	class tEvent : public EventDispatcher
	{
		public:
			tEvent(Vector<String> arg) : EventDispatcher()
			{
				trace("Hello " + toString());
				addEventListener(Event::COMPLETE, Callback(
					this, cb_cast(&tEvent::onEvent)
				));
				addEventListener(Event::PROGRESS, Callback(
					this, cb_cast(&tEvent::onEvent2)
				));

				dispatchEvent(new Event(Event::COMPLETE))->destroy();
				//dispatchEvent(new Event(Event::PROGRESS))->destroy();

				//programExit();
			}

			void onEvent(Object *arg)
			{
				Event *event = (Event*)arg;
				//trace("onEvent="+event->toString());
				event->destroy();

				static int64 cnt = 0;
				static Mutex cntMutex;
				cntMutex.lock();
				cnt++;
				trace("\n"+int2string(cnt)+"\n\033[A\033[A\033[A");
				cntMutex.unlock();

				if(cnt >= 25000)
				{
					if(cnt == 25000)
					{
						trace("----------------EXIT------------------");
						programExit();
					}
					removeEventListener(Event::COMPLETE, Callback(
						this, cb_cast(&tEvent::onEvent)
					));
					removeEventListener(Event::PROGRESS, Callback(
						this, cb_cast(&tEvent::onEvent2)
					));
				}

				dispatchEvent(new Event(Event::COMPLETE))->destroy();
				dispatchEvent(new Event(Event::PROGRESS))->destroy();
				dispatchEvent(new Event(Event::COMPLETE))->destroy();
			}

			void onEvent2(Object *arg)
			{
				Event *event = (Event*)arg;
				//trace("onEvent2="+event->toString());
				event->destroy();
				dispatchEvent(new Event(Event::COMPLETE))->destroy();
				dispatchEvent(new Event(Event::PROGRESS))->destroy();
				dispatchEvent(new Event(Event::COMPLETE))->destroy();
			}

			String className() { return "u::tEvent"; }
	};
}

#define APPLICATION tEvent
#include <Application>
