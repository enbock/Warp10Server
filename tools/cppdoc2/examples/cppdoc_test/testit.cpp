#include "TestDocDerived.h"
#include "MultipleInheritance.h"


/**
 * Here's a global variable, for good measure.
 */
int gCount = 0;


/**
 * The main function.  Instantiates a {@link TestDoc}.
 */
void main()
{
	TestDoc x( 0 );
    x;

    MultipleInheritance y;
    y;
}
