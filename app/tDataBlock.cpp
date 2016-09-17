/*
 * tDataBlock.cpp
 * Datablock tests.
 *  Created on: 31.03.2011
 *      Author: User
 */



#include <Datablock>

namespace u
{
	void programExit();

	class tDataBlock : public Object
	{
		public:
			tDataBlock(Vector<String> arg)
			{
				trace("Hello " + toString());

				Datablock db;
				db["hello"] = new String("Hello Datablock!");
				db[0] = new String("Save via int index.");
				trace(db.toString());

				Datablock db2 = db;
				trace(">>"+*(String*)db2["0"]);
				trace(db2.toString());

				trace();
				programExit();
			}


			String className() { return "u::app"; }
	};
}

#define APPLICATION tDataBlock
#include <Application>
