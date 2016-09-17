/*
 * tDataBlock.cpp
 * Datablock tests.
 *  Created on: 31.03.2011
 *      Author: User
 */



#include <Thread>

namespace u
{
	void programExit();

	class tThread : public Object
	{
		public:
			int64 cnt;
			tThread(Vector<String> arg)
			{
				trace("Hello " + toString());
				cnt = 0;
				trace("Create thread now...");
				Callback cb(this, cb_cast(&tThread::newThread));
				ThreadSystem::create(&cb);
				//programExit();
			}

			void newThread(Object *arg)
			{
				int64 num;
				lock();
				num = cnt++;
				unlock();
				trace("This is thread nr. " + int2string(num));
				Callback cb(this, cb_cast(&tThread::newThread));
				ThreadSystem::create(&cb);
				ThreadSystem::create(&cb);
				if(num == 10000)
				{
					trace("-----------------END PRG----------------------");
					programExit();
				}
			}

			String className() { return "u::tThread"; }
	};
}

#define APPLICATION tThread
#include <Application>
