/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Booleans.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	AndOperatorAndOrOperatorExpectBooleans,
	"every MyClass is:								\n\
		andString() { 'test' && 'test'; }			\n\
		orString() { 'test' || 'test'; }			\n\
		andNum() { 6 && 6; }						\n\
		orNum() { 6 || 6; }							\n\
		andClass(MyClass) { MyClass && MyClass; }	\n\
		orClass(MyClass) { MyClass || MyClass; }	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	AndOperatorAndOrOperatorReturnBools,
	"every MyClass is:									\n\
		andBools() { true && true && true; }			\n\
		orBools() { true || true || true; }			\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	CannotIfCondOrInvertWithAnythingButBools,
	"every MyClass is:											\n\
		ifConditionWith( MyClass ) { if(MyClass) 5; }			\n\
		ifConditionWithText() { if('test') 5; }					\n\
		whileConditionWith( MyClass ) { while(MyClass) 5; }		\n\
		whileConditionWithText() { while('test') 5; }			\n\
		forConditionWithNum() { for(5; 5; 5) 5; }				\n\
		forConditionWith( MyClass ) { for(5; MyClass; 5) 5; }	\n\
		forConditionWithText() { for(5; 'test'; 5) 5; }			\n\
		invertNum() { !5; }										\n\
		invertText() { !'test'; }								\n\
		invert( MyClass ) { !MyClass; }							\n\
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
)

