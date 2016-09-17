#pragma once

#include <string>

/**
 * Just a test of multiple inheritance.  It's derived from {@link TestDoc},
 * {@link TestDocException}, and (strangely) std::string.
 */
class MultipleInheritance : public TestDoc, public TestDocException, private std::string
{
public:
    MultipleInheritance()
        : TestDoc( 0 )
    {
    }


    /**
     * Does nothing.
     */
    void doNothing()
    {
    }
};