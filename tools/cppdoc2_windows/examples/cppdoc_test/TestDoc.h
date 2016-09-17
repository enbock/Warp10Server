#pragma once

#pragma warning ( disable : 4290 )

#include "TestDocException.h"
#include "TestDocBase.h"
#include "String.h"



/**
 * Class for documentation testing.  Some inline stuff is to follow.<br>
 * The document root path is: {@docroot}<br>
 * Here are the source files: {@files}<br>
 * Here is source file 1: {@file1}<br>
 * Here is source file 2: {@file2}
 * 
 * @author  rico suave
 * @version  2.0
 * @see "The help file"
 * @see #getStr
 * @see Code.Foo
 * @see <a href="http://www.cppdoc.com/cppdoc">the CppDoc website</a>
 * @since version 1.0
 */
class TestDoc : public TestDocBase
{
public:
    /**
     * A constructor.
     */
    TestDoc( long x )
    {
    }


    virtual ~TestDoc();


    /**
     * A test function to show off the <code>@param</code> tag.
     * Note that the <code>throw</code> specification in the function declaration is not
     * required for the <code>@throw</code> tag.
     *
     * @throw TestDocException when anything bad happens
     * @param x the first parameter... which isn't used
     * @param y another unused parameter
     * @return the String "hi"
     */
    String getStr( int x, float y ) throw( TestDocException )
    {
        if ( true )
            throw TestDocException();
        return "hi";
    }


    /**
     * Here's a link: {@link TestDocBase#protectedBaseMethod(int) (Click Me!)}, and
     * here's another link: {@link #getStr}, and
     * here's an unresolvable link: {@link NonexistantClass#foo foo}.
     * @see #getStr
     * @see #someDeprecatedMethod
     */
#ifdef PREPROCESSOR_TEST
    void bar( String thisIsFromPreprocessorTest )
#else
    void bar()
#endif
    {
    }


    /**
	 * This is some documentation attached to a
     * deprecated method.
     * @deprecated This method doesn't do anything.  Use {@link #bar the bar() function}
     *             instead.
     */
    void someDeprecatedMethod()
    {
    }


protected:
    /**
     * This is a plain ol' doc comment on a protected method.
     */
    int plainOlFunc()
    {
    }


	/**
	 * A static method.
	 */
    static float someStaticFunc( int* arg )
    {
        return 0;
    }


private:
    /**
     * This private method will only get documented if you enable documentation
     * of private members.
     */
    void privateFunc()
    {
    }
};
