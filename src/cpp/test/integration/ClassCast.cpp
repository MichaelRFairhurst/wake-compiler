/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ClassCast.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	TestCastDownToInvalidClass,
	"every MyClass is: method() { (cast NonExist) this; }",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	TestCastDownToNonSubType,
	"every MyClass is: method() { (cast MyClass) 5; }",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestCastDownValid,
	"every ParentClass is:						\n\
	every MyClass (a ParentClass) is:			\n\
		method(ParentClass) {					\n\
			method((cast ParentClass) this);	\n\
		}",
	PTT_VALID
)

PTT_TEST_CASE(
	TestUnsafeCastInvalidAccepted,
	"every UnrelatedClass is:							\n\
	every MyClass is:									\n\
		method(UnrelatedClass) {						\n\
			method((unsafe cast UnrelatedClass) this);	\n\
		}",
	PTT_VALID
)
