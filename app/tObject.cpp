/*
* tObject.cpp
* First testfile.
*  Created on: 21.02.2011
*      Author: Endre Bock
*/

#include <Object>
#include <Callback>

namespace u
{
	void programExit();

	class tObject : public Object
	{
		public:
			tObject(Vector<String> arg)
			{
				int64 i, l = arg.size();
				trace("Hello " + this->toString() + "!");
				for(i=0; i<l; i++)
				{
					trace("Argument " + int2string(i) + ": " + arg[i]);
				}
				error("Error test");

				Callback cb(this, cb_cast(&tObject::callMe), new String("Hello Call!"));
				trace(cb.toString());
				cb.call();

				Object cnO;
				Object *cnS = new String("Hello");
				Mutex cnM;
				trace("Class of object: " + cnO.className());
				trace("Class of String: " + cnS->className());
				trace("Class of Mutex : " + cnM.className());
				cnS->destroy();

				trace();
				programExit();
			}

			void callMe(Object *arg)
			{
				trace("You called me width: " +arg->toString());
				arg->destroy();
			}

			String className() { return "tObject"; }
	};
}

#define APPLICATION tObject
#include <Application>
