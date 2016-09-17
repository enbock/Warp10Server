#pragma once


class TestDocBase
{
public:
    int baseField;

    TestDocBase()
    {
    }

    virtual ~TestDocBase()
    {
    }

    void publicBaseMethod()
    {
    }

protected:
    void protectedBaseMethod( int i )
    {
    }


	/**
	 * A protected constructor.
	 */
	TestDocBase( int i )
	{
	}
};
