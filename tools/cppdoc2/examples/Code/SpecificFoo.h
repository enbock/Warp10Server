#pragma once

#include "Foo.h"


/**
 * A subclass of Foo.
 */
class SpecificFoo : public Foo
{
public:
	
	/**
	 * This is an override of <i>funk</i> in class <i>Foo</i>.
	 */
	void funk( int )
	{
	}
};
