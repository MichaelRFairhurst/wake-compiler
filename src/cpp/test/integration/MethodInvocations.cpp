/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MethodInvocations.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	MethodInvocationOfClassNotThisComplex,
	"every MyClass is: MyClass -- method(MyClass c)Here(MyClass b)Yeah() { return c.method(c)Here(b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassNotThisSimple,
	"every MyClass is: MyClass -- method(MyClass c) { return c.method(c); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassNotThisArgumentsAreTypeChecked,
	"every MyClass is:																								\n\
		MyClass -- argIsInvalidType(MyClass c, Num) { return c.argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Num) { return c.argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Num) { return c.argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Num) { return c.argIsInvalidType(c); }					\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisComplex,
	"every MyClass is: MyClass -- method(MyClass c)Here(MyClass b)Yeah() { return method(c)Here(b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisSimple,
	"every MyClass is: MyClass -- method(MyClass c) { return method(c); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisArgumentsAreTypeChecked,
	"every MyClass is:																							\n\
		MyClass -- argIsInvalidType(MyClass c, Num) { return argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Num) { return argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Num) { return argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Num) { return argIsInvalidType(c); }					\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitComplex,
	"every MyClass is: MyClass -- method(MyClass c)Here(MyClass b)Yeah() { return this.method(c)Here(b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitSimple,
	"every MyClass is: MyClass -- method(MyClass c) { return this.method(c); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitArgumentsAreTypeChecked,
	"every MyClass is:																								\n\
		MyClass -- argIsInvalidType(MyClass c, Num) { return this.argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Num) { return this.argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Num) { return this.argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Num) { return this.argIsInvalidType(c); }					\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodCallOnAnUnknownClassIsUnknownVariableError,
	"every MyClass is:				\n\
		myMethod() {				\n\
			Whatever.whatever();	\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	ReturnMethodCallOnAnUnknownClassIsUnknownVariableError,
	"every MyClass is:					\n\
		Num --  myMethod() {			\n\
			return Whatever.whatever();	\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	ReturnVoidMethodCallOnAnUnknownClassIsUnknownVariableError,
	"every MyClass is:					\n\
		myMethod() {					\n\
			return Whatever.whatever();	\n\
		}",
	PTT_EXPECT(TYPE_ERROR) // invalid return
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);
