/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Constructors.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	NotFoundTypeInCtorIsReported,
	"every MyClass is: needs NonExist;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MulipleCtorDefinition,
	"every MyClass is: needs Num; needs Num;",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
)

PTT_TEST_CASE(
	CtorAndPropertiesUsableInMethods,
	"every MyClass is:									\n\
	every OtherClass is:								\n\
		needs MyClass c;								\n\
		with MyClass b = c;								\n\
		with MyClass e = c;							\n\
		MyClass -- testScope(MyClass d) {				\n\
			return e; return b; return c; return d;	\n\
		}",
	PTT_VALID
)

PTT_TEST_CASE(
	CtorBodyIsTypeChecked,
	"every MyClass is:			\n\
		needs nothing then {	\n\
			5 + true;			\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	CtorBodyHasNoReturnTypeCheckWhenValid,
	"every MyClass is: needs nothing then { return; }",
	PTT_VALID
)

PTT_TEST_CASE(
	CtorBodyHasNoReturnTypeCheckWhenInValid,
	"every MyClass is: needs nothing then { return 5; }",
	PTT_EXPECT(TYPE_ERROR)
)

