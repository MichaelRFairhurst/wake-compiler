/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MethodReturns.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	InvalidReturnValues,
	"every UnrelatedClass is:																\n\
	every ParentClass is:																	\n\
	every MyClass (a ParentClass) is:														\n\
		Bool -- returnTextLiteralAsBool() { return 'text'; }								\n\
		Bool -- returnNumberLiteralAsBool() { return 1; }									\n\
		Bool -- returnClassAsBool(MyClass) { return MyClass; }							\n\
		Bool -- returnNothingAsBool() { return; }											\n\
		Text -- returnBoolLiteralAsText() { return true; }									\n\
		Text -- returnNumberLiteralAsText() { return 1; }									\n\
		Text -- returnClassAsText(MyClass) { return MyClass; }								\n\
		Text -- returnNothingAsText() { return; }											\n\
		Num -- returnBoolLiteralAsNum() { return true; }									\n\
		Num -- returnTextLiteralAsNum() { return 'text'; }									\n\
		Num -- returnClassAsNum(MyClass) { return MyClass; }								\n\
		Num -- returnNothingAsNum() { return; }												\n\
		MyClass -- returnBoolLiteralAsClass() { return true; }								\n\
		MyClass -- returnTextLiteralAsClass() { return 'test'; }							\n\
		MyClass -- returnNumLiteralAsClass() { return 1; }									\n\
		MyClass -- returnUnrelatedClassAsClass(UnrelatedClass) { return UnrelatedClass; }	\n\
		MyClass -- returnParentClassAsClass(ParentClass) { return ParentClass; }			\n\
		MyClass -- returnNothingAsMyClass() { return; }										\n\
		returnTextLiteralInVoidMethod() { return 'text'; }									\n\
		returnNumberLiteralInVoidMethod() { return 1; }										\n\
		returnClassInVoidMethod(MyClass) { return MyClass; }								\n\
		returnBoolLiteralInVoidMethod() { return true; }									\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ValidReturnValues,
	"every RelatedClass (a MyClass) is:												\n\
	every MyClass is:																\n\
		Bool -- returnBool() { return true; }										\n\
		Text -- returnText() { return 'text'; }										\n\
		Num -- returnNum() { return 5; }											\n\
		MyClass -- returnAMyClass(MyClass) { return MyClass; }						\n\
		MyClass -- returnARelatedClass(RelatedClass) { return RelatedClass; }		\n\
		MyClass -- returnRelatedClassAsClass(RelatedClass) { return RelatedClass; }	\n\
		returnNothingInVoidMethod() { return; }										\n\
	",
	PTT_VALID
)
