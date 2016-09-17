#pragma once


/**
 * This is the <b>Foo</b> class.
 */
class Foo
{
public:
	
	/**
	 * The default constructor for Foo.
	 */
	Foo()
	{
	}


	/**
	 * Another constructor for Foo.  Incidentally, you can put
	 * <pre>
	 *     HTML formatting
	 * </pre>
	 * in documentation comments.
	 */
	Foo( const char* );


	/**
	 * Foo destructor.
	 */
	~Foo()
	{
	}
	
	
	/**
	 * This is the function <i>funk</i> within
	 * the Foo class.
	 */
	virtual void funk( int n )
	{
	}


	bool anotherFunc();  // this is documented in Foo.cpp



private:
	/**
	 * Here is a private member variable (<i>m_var</i>).
	 */
	int m_var;
};
