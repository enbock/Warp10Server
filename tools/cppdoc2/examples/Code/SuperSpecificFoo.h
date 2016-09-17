#pragma once

#include "SpecificFoo.h"
#include <string>


/**
 * A subclass of SpecificFoo.<br>
 * Here's some <b>formatting</b> just in case you're not thrilled yet.
 */
class SuperSpecificFoo : public SpecificFoo
{
public:
	
	/**
	 * This function doesn't do anything either.  What's the point of all
	 * this inheritance?
	 */
	std::string yetAnotherFunc() throw( Bar )
	{
		return "hello there";
	}
};
