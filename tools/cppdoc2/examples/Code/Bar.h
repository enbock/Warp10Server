#pragma once

#include "Foo.h"

#define UGLY int weirdFunc( bool arg )
#define UGLIER(returnType_)   \
    {                         \
        return returnType_(); \
    }
    // (isn't that horrible?)


class SomeExternalClass;


/**
 * Another class, to demonstrate some more stuff.
 */
class Bar
{
public:
	
	/**
	 * You can document enums and other such beasts.
	 */
	enum Mood
	{
		Up = 10,
		Down,
		Sideways
	};


	/**
	 * CppDoc won't get confused by all but the most gut-wrenching declarations.
	 */
	typedef SomeExternalClass*& ( *RandomFuncPtr )( class AnotherClass* = 0 );

	
	/**
	 * CppDoc will cross-reference whatever it can.  Note that
	 * <i>RandomFuncPtr</i> is a link.
	 */
	void doSomethingStrange( RandomFuncPtr ptr, int ) const
	{
	}


	/**
	 * #defines are OK.  CppDoc will handle preprocessor macros
	 * as well as #ifdef, #ifndef, etc.
	 */
	UGLY
	UGLIER(int)


protected:
	/**
	 * A protected member that returns an object.
	 */
	Foo getFoo() const
	{
	}
};
