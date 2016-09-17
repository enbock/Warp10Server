#pragma once

#include "TestDoc.h"

/**
 * Just testing the third level of hierarchy.
 */
class TestDocDerived : public TestDoc
{
    TestDocDerived()
        : TestDoc( 0 )
    {
    }
};