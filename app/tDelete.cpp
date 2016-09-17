/*
* tDelete.cpp
*
*  Created on: 04.01.2013
*      Author: Endre Bock
*/

#include <Object>
#include <Callback>

namespace u
{
	void programExit();

	class tDelete : public Object
	{
		public:
			tDelete(Vector<String> arg) : Object()
			{
				Object* o = new tDelete();

				Callback cb(o, cb_cast(&tDelete::callMe));

				o->destroy();
				//cb.call();
				trace("Alive? "+String(valid(o)?"Yes":"No"));

				trace();
				programExit();
			}

			tDelete() : Object()
			{
				trace("Test tDelete");
			}

			void callMe(Object *arg)
			{
				trace("You called me width: " +arg->toString());
				arg->destroy();
			}

			String className() { return "tDelete"; }
	};
}

#define APPLICATION tDelete
#include <Application>
