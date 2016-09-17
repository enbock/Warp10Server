CppDoc
Version 2.4


Welcome to CppDoc.  The idea behind this application is simple: it generates
HTML documentation for your C++ classes, based on your source code and special
comments embedded within it.

CppDoc parses source-code comments that start with "/**".  So, it will
associate the comment "This is only a test." with the class Foo in the
following example:


	/**
	 * This is <I>only</I> a test.
	 */
	class Foo
	{
		...
	};

CppDoc recognizes and documents classes, member functions, member data, global
functions, typedefs, and enums.  The documentation comments are optional; if
you leave them out, CppDoc will still generate a nicely-formatted set of
documentation based on your class hierarchy and class members.

Two modules are included with CppDoc: the original one, which produces v1.0
output, and a new one which produces full Javadoc-like output and which
supports Javadoc tags, such as @see, @return, etc.

That's pretty much all there is to it.  See the Help section of the main dialog
for more details.


Free

CppDoc is free, and may not be sold.  It may be included with CD-ROM/DVD-ROM
distributions that carry nominal charges for the media.  It may be distributed
freely on any website or through any other distribution mechanism, as long as
no part of it is changed in any way, and as long as its ReadMe file remains
with it.

This program is offered "AS IS", with no warranties of any kind.

If you like CppDoc, or if you have a request for a bug fix or enhancement,
please submit it at http://www.cppdoc.com/bugs.  To sign up for the mailing
list, hit http://www.cppdoc.com/mailing/subscribe.do.  Finally, if you would
like to donate to CppDoc, visit http://www.cppdoc.com/donate.html. 
