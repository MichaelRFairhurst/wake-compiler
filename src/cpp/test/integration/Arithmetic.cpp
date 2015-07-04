/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Arithmetic.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	MultiplyNumsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		multiplyNumByString() { 5 * 'test'; }									\n\
		multiplyStringByNum() { 'test' * 5; }									\n\
		multiplyNumByObject(MyClass) { 5 * MyClass; }							\n\
		multiplyObjectByNum(MyClass) { MyClass * 5; }							\n\
		multiplyLambdaByNum(MyClass --fn(MyClass) lambda) { lambda * 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	DivideNumsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		divideNumByString() { 5 / 'test'; }						\n\
		divideStringByNum() { 'test' / 5; }						\n\
		divideNumByObject(MyClass) { 5 / MyClass; }				\n\
		divideObjectByNum(MyClass) { MyClass / 5; }				\n\
		divideLambdaByNum(fn() lambda) { lambda / 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ModNumsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		modNumByString() { 5 \% 'test'; }						\n\
		modStringByNum() { 'test' % 5; }						\n\
		modNumByObject(MyClass) { 5 % MyClass; }				\n\
		modObjectByNum(MyClass) { MyClass % 5; }				\n\
		modLambdaByNum(fn() lambda) { lambda % 5; }				\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ModNativeNumsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		modNativeNumByString() { 5 %% 'test'; }						\n\
		modNativeStringByNum() { 'test' %% 5; }						\n\
		modNativeNumByObject(MyClass) { 5 %% MyClass; }				\n\
		modNativeObjectByNum(MyClass) { MyClass %% 5; }				\n\
		modNativeLambdaByNum(fn() lambda) { lambda %% 5; }				\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ModAltNumsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		modNativeNumByString() { 5 %%% 'test'; }						\n\
		modNativeStringByNum() { 'test' %%% 5; }						\n\
		modNativeNumByObject(MyClass) { 5 %%% MyClass; }				\n\
		modNativeObjectByNum(MyClass) { MyClass %%% 5; }				\n\
		modNativeLambdaByNum(fn() lambda) { lambda %%% 5; }				\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	AddNumsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		addNumByString() { 5 + 'test'; }						\n\
		addStringByNum() { 'test' + 5; }						\n\
		addNumByObject(MyClass) { 5 + MyClass; }				\n\
		addObjectByNum(MyClass) { MyClass + 5; }				\n\
		addLambdaByNum(fn() lambda) { lambda + 5; }			\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	SubtractNumsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		subtractNumByString() { 5 - 'test'; }					\n\
		subtractStringByNum() { 'test' - 5; }					\n\
		subtractNumByObject(MyClass) { 5 - MyClass; }			\n\
		subtractObjectByNum(MyClass) { MyClass - 5; }			\n\
		subtractLambdaByNum(fn() lambda) { lambda - 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ValidArithmeticAndComparisonsReturnProperTypes,
	"every MyClass is:																\n\
		validArithmetic() {															\n\
			5 / 5 / 5;					// done thrice to ensure int returned		\n\
			5 * 5 * 5;					// done thrice to ensure int returned		\n\
			5 + 5 + 5;					// done thrice to ensure int returned		\n\
			5 - 5 - 5;					// done thrice to ensure int returned		\n\
			'test' + 'test' + 'test';	// done thrice to ensure string returned	\n\
			'test' == 'test' == true;	// test returns truth						\n\
			'test' != 'test' == true;	// test returns truth						\n\
			5 == 5 == true;				// test returns truth						\n\
			5 != 5 == true;				// test returns truth						\n\
			5 < 5 == true;				// test returns truth						\n\
			5 > 5 == true;				// test returns truth						\n\
			5 <= 5 == true;				// test returns truth						\n\
			5 >= 5 == true;				// test returns truth						\n\
		}																			\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	ValidArithmeticGivenWhiteSpace,
	"every MyClass is:																\n\
		validArithmetic() {															\n\
			5/5/5;					// done thrice to ensure int returned		\n\
			5*5*5;					// done thrice to ensure int returned		\n\
			5+5+5;					// done thrice to ensure int returned		\n\
			5-5-5;					// done thrice to ensure int returned		\n\
			'test'+'test'+'test';	// done thrice to ensure string returned	\n\
			'test'=='test'==true;	// test returns truth						\n\
			'test'!='test'==true;	// test returns truth						\n\
			5==5 ==true;				// test returns truth						\n\
			5!=5==true;				// test returns truth						\n\
			5<5==true;				// test returns truth						\n\
			5>5==true;				// test returns truth						\n\
			5<=5==true;				// test returns truth						\n\
			5>=5==true;				// test returns truth						\n\
			5- 4<<3==8==true;		// test 5- 4 isn't (5) (-4)					\n\
		}																			\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	ArithmeticOperatorsNotValidOnStringPairs,
	"every MyClass is:									\n\
		subtractStrings() { 'test' - 'test'; }			\n\
		multiplyStrings() { 'test' * 'test'; }			\n\
		divideStrings() { 'test' / 'test'; }			\n\
		lessthanStrings() { 'test' < 'test'; }			\n\
		greaterthanStrings() { 'test' > 'test'; }		\n\
		lessthanequalStrings() { 'test' <= 'test'; }	\n\
		greaterthanequalStrings() { 'test' >= 'test'; }	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestValidCharArithmetic,
	"every MyClass is:								\n\
		addChars() { var Text = \\n + \\n; }		\n\
		addStringChar() { var Text = 'ab' + \\n; }	\n\
		addCharString() { var Text = \\n + 'ab'; }	\n\
	",
	PTT_VALID
);
