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
		multiplyNumByString() { 5.0 * 'test'; }									\n\
		multiplyStringByNum() { 'test' * 5.0; }									\n\
		multiplyNumByObject(MyClass) { 5.0 * MyClass; }							\n\
		multiplyObjectByNum(MyClass) { MyClass * 5.0; }							\n\
		multiplyLambdaByNum(MyClass --fn(MyClass) lambda) { lambda * 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	MultiplyIntsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		multiplyIntByString() { 5 * 'test'; }									\n\
		multiplyStringByInt() { 'test' * 5; }									\n\
		multiplyIntByObject(MyClass) { 5 * MyClass; }							\n\
		multiplyObjectByInt(MyClass) { MyClass * 5; }							\n\
		multiplyLambdaByInt(MyClass --fn(MyClass) lambda) { lambda * 5; }		\n\
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
		divideNumByString() { 5.0 / 'test'; }						\n\
		divideStringByNum() { 'test' / 5.0; }						\n\
		divideNumByObject(MyClass) { 5.0 / MyClass; }				\n\
		divideObjectByNum(MyClass) { MyClass / 5.0; }				\n\
		divideLambdaByNum(fn() lambda) { lambda / 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	DivideIntsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		divideIntByString() { 5 / 'test'; }						\n\
		divideStringByInt() { 'test' / 5; }						\n\
		divideIntByObject(MyClass) { 5 / MyClass; }				\n\
		divideObjectByInt(MyClass) { MyClass / 5; }				\n\
		divideLambdaByInt(fn() lambda) { lambda / 5; }		\n\
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
		modNumByString() { 5.0 \% 'test'; }						\n\
		modStringByNum() { 'test' % 5.0; }						\n\
		modNumByObject(MyClass) { 5.0 % MyClass; }				\n\
		modObjectByNum(MyClass) { MyClass % 5.0; }				\n\
		modLambdaByNum(fn() lambda) { lambda % 5.0; }				\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ModIntsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		modIntByString() { 5 \% 'test'; }						\n\
		modStringByInt() { 'test' % 5; }						\n\
		modIntByObject(MyClass) { 5 % MyClass; }				\n\
		modObjectByInt(MyClass) { MyClass % 5; }				\n\
		modLambdaByInt(fn() lambda) { lambda % 5; }				\n\
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
		modNativeNumByString() { 5.0 %% 'test'; }						\n\
		modNativeStringByNum() { 'test' %% 5.0; }						\n\
		modNativeNumByObject(MyClass) { 5.0 %% MyClass; }				\n\
		modNativeObjectByNum(MyClass) { MyClass %% 5.0; }				\n\
		modNativeLambdaByNum(fn() lambda) { lambda %% 5.0; }				\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ModNativeIntsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		modNativeIntsyString() { 5 %% 'test'; }						\n\
		modNativeStringByInt() { 'test' %% 5; }						\n\
		modNativeIntByObject(MyClass) { 5 %% MyClass; }				\n\
		modNativeObjectByInt(MyClass) { MyClass %% 5; }				\n\
		modNativeLambdaByInt(fn() lambda) { lambda %% 5; }				\n\
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
		modNativeNumByString() { 5.0 %%% 'test'; }						\n\
		modNativeStringByNum() { 'test' %%% 5.0; }						\n\
		modNativeNumByObject(MyClass) { 5.0 %%% MyClass; }				\n\
		modNativeObjectByNum(MyClass) { MyClass %%% 5.0; }				\n\
		modNativeLambdaByNum(fn() lambda) { lambda %%% 5.0; }				\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ModAltIntsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		modNativeIntByString() { 5 %%% 'test'; }						\n\
		modNativeStringByInt() { 'test' %%% 5; }						\n\
		modNativeIntByObject(MyClass) { 5 %%% MyClass; }				\n\
		modNativeObjectByInt(MyClass) { MyClass %%% 5; }				\n\
		modNativeLambdaByInt(fn() lambda) { lambda %%% 5; }				\n\
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
		addNumByString() { 5.0 + 'test'; }						\n\
		addStringByNum() { 'test' + 5.0; }						\n\
		addNumByObject(MyClass) { 5.0 + MyClass; }				\n\
		addObjectByNum(MyClass) { MyClass + 5.0; }				\n\
		addLambdaByNum(fn() lambda) { lambda + 5.0; }			\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	AddIntsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		addIntByString() { 5 + 'test'; }						\n\
		addStringByInt() { 'test' + 5; }						\n\
		addIntByObject(MyClass) { 5 + MyClass; }				\n\
		addObjectByInt(MyClass) { MyClass + 5; }				\n\
		addLambdaByInt(fn() lambda) { lambda + 5; }			\n\
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
		subtractNumByString() { 5.0 - 'test'; }					\n\
		subtractStringByNum() { 'test' - 5.0; }					\n\
		subtractNumByObject(MyClass) { 5.0 - MyClass; }			\n\
		subtractObjectByNum(MyClass) { MyClass - 5.0; }			\n\
		subtractLambdaByNum(fn() lambda) { lambda - 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	SubtractIntsAndAnythingElseIsTypeError,
	"every MyClass is:											\n\
		subtractIntByString() { 5 - 'test'; }					\n\
		subtractStringByInt() { 'test' - 5; }					\n\
		subtractIntByObject(MyClass) { 5 - MyClass; }			\n\
		subtractObjectByInt(MyClass) { MyClass - 5; }			\n\
		subtractLambdaByInt(fn() lambda) { lambda - 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	LessThanIntsAndAnythingElseIsTypeError,
	"every MyClass is:									\n\
		ltIntByString() { 5 < 'test'; }					\n\
		ltStringByInt() { 'test' < 5; }					\n\
		ltIntByObject(MyClass) { 5 < MyClass; }			\n\
		ltObjectByInt(MyClass) { MyClass < 5; }			\n\
		ltLambdaByInt(fn() lambda) { 5 < lambda; }		\n\
		ltIntByLambda(fn() lambda) { lambda < 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	LessThanEqIntsAndAnythingElseIsTypeError,
	"every MyClass is:									\n\
		lteIntByString() { 5 <= 'test'; }					\n\
		lteStringByInt() { 'test' <= 5; }					\n\
		lteIntByObject(MyClass) { 5 <= MyClass; }			\n\
		lteObjectByInt(MyClass) { MyClass <= 5; }			\n\
		lteLambdaByInt(fn() lambda) { 5 <= lambda; }		\n\
		lteIntByLambda(fn() lambda) { lambda <= 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	GreaterThanIntsAndAnythingElseIsTypeError,
	"every MyClass is:									\n\
		gtIntByString() { 5 > 'test'; }					\n\
		gtStringByInt() { 'test' > 5; }					\n\
		gtIntByObject(MyClass) { 5 > MyClass; }			\n\
		gtObjectByInt(MyClass) { MyClass > 5; }			\n\
		gtLambdaByInt(fn() lambda) { 5 > lambda; }		\n\
		gtIntByLambda(fn() lambda) { lambda > 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	GreaterThanEqIntsAndAnythingElseIsTypeError,
	"every MyClass is:									\n\
		gteIntByString() { 5 >= 'test'; }					\n\
		gteStringByInt() { 'test' >= 5; }					\n\
		gteIntByObject(MyClass) { 5 >= MyClass; }			\n\
		gteObjectByInt(MyClass) { MyClass >= 5; }			\n\
		gteLambdaByInt(fn() lambda) { 5 >= lambda; }		\n\
		gteIntByLambda(fn() lambda) { lambda >= 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)


PTT_TEST_CASE(
	LessThanNumsAndAnythingElseIsTypeError,
	"every MyClass is:									\n\
		ltNumByString() { 5.0 < 'test'; }					\n\
		ltStringByNum() { 'test' < 5.0; }					\n\
		ltNumByObject(MyClass) { 5.0 < MyClass; }			\n\
		ltObjectByNum(MyClass) { MyClass < 5.0; }			\n\
		ltLambdaByNum(fn() lambda) { 5.0 < lambda; }		\n\
		ltNumByLambda(fn() lambda) { lambda < 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	LessThanEqNumsAndAnythingElseIsTypeError,
	"every MyClass is:									\n\
		lteNumByString() { 5.0 <= 'test'; }					\n\
		lteStringByNum() { 'test' <= 5.0; }					\n\
		lteNumByObject(MyClass) { 5.0 <= MyClass; }			\n\
		lteObjectByNum(MyClass) { MyClass <= 5.0; }			\n\
		lteLambdaByNum(fn() lambda) { 5.0 <= lambda; }		\n\
		lteNumByLambda(fn() lambda) { lambda <= 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	GreaterThanNumsAndAnythingElseIsTypeError,
	"every MyClass is:									\n\
		gtNumByString() { 5.0 > 'test'; }					\n\
		gtStringByNum() { 'test' > 5.0; }					\n\
		gtNumByObject(MyClass) { 5.0 > MyClass; }			\n\
		gtObjectByNum(MyClass) { MyClass > 5.0; }			\n\
		gtLambdaByNum(fn() lambda) { 5.0 > lambda; }		\n\
		gtNumByLambda(fn() lambda) { lambda > 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	GreaterThanEqNumsAndAnythingElseIsTypeError,
	"every MyClass is:									\n\
		gteNumByString() { 5.0 >= 'test'; }					\n\
		gteStringByNum() { 'test' >= 5.0; }					\n\
		gteNumByObject(MyClass) { 5.0 >= MyClass; }			\n\
		gteObjectByNum(MyClass) { MyClass >= 5.0; }			\n\
		gteLambdaByNum(fn() lambda) { 5.0 >= lambda; }		\n\
		gteNumByLambda(fn() lambda) { lambda >= 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
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
			5.0 == 5.0 == true;			// test returns truth						\n\
			5.0 != 5.0 == true;			// test returns truth						\n\
			5.0 < 5.0 == true;			// test returns truth						\n\
			5.0 > 5.0 == true;			// test returns truth						\n\
			5.0 <= 5.0 == true;			// test returns truth						\n\
			5.0 >= 5.0 == true;			// test returns truth						\n\
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

PTT_TEST_CASE(
	TestNumArithmeticResultsInNumsValid,
	"every MyClass is:								\n\
		myMethod() {								\n\
			var Num = 5.0 + 5;						\n\
			Num = 5 + 5.0;							\n\
			Num = 5.0 + 5.0;						\n\
			Num = 5.0 - 5;							\n\
			Num = 5 - 5.0;							\n\
			Num = 5.0 - 5.0;						\n\
			Num = 5.0 * 5;							\n\
			Num = 5 * 5.0;							\n\
			Num = 5.0 * 5.0;						\n\
			Num = 5.0 / 5;							\n\
			Num = 5 / 5.0;							\n\
			Num = 5.0 / 5.0;						\n\
			Num = 5.0 % 5;							\n\
			Num = 5 % 5.0;							\n\
			Num = 5.0 % 5.0;						\n\
			Num = 5.0 %% 5;							\n\
			Num = 5 %% 5.0;							\n\
			Num = 5.0 %% 5.0;						\n\
			Num = 5.0 %%% 5;						\n\
			Num = 5 %%% 5.0;						\n\
			Num = 5.0 %%% 5.0;						\n\
		}											\n\
		",
	PTT_VALID
);

PTT_TEST_CASE(
	TestNumIntComparisonsResultsInBoolValid,
	"every MyClass is:							\n\
		myMethod() {							\n\
			var Bool = 5.5 < 5;					\n\
			Bool = 5 < 5.5; 					\n\
			Bool = 5.5 > 5; 					\n\
			Bool = 5 > 5.5; 					\n\
			Bool = 5.5 >= 5; 					\n\
			Bool = 5 >= 5.5; 					\n\
			Bool = 5.5 <= 5; 					\n\
			Bool = 5 <= 5.5; 					\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestNumArithmeticResultsInNumsInvalid,
	"every MyClass is:							\n\
		addL() { var Int = 5.0 + 5; }			\n\
		addR() { var Int = 5 + 5.0; }			\n\
		addB() { var Int = 5.0 + 5.0; }			\n\
		subL() { var Int = 5.0 - 5; }			\n\
		subR() { var Int = 5 - 5.0; }			\n\
		subB() { var Int = 5.0 - 5.0; }			\n\
		mulL() { var Int = 5.0 * 5; }			\n\
		mulR() { var Int = 5 * 5.0; }			\n\
		mulB() { var Int = 5.0 * 5.0; }			\n\
		divL() { var Int = 5.0 / 5; }			\n\
		divR() { var Int = 5 / 5.0; }			\n\
		divB() { var Int = 5.0 / 5.0; }			\n\
		modL() { var Int = 5.0 % 5; }			\n\
		modR() { var Int = 5 % 5.0; }			\n\
		modB() { var Int = 5.0 % 5.0; }			\n\
		modnL() { var Int = 5.0 %% 5; }			\n\
		modnR() { var Int = 5 %% 5.0; }			\n\
		modnB() { var Int = 5.0 %% 5.0; }		\n\
		modaltL() { var Int = 5.0 %%% 5; }		\n\
		modaltR() { var Int = 5 %%% 5.0; }		\n\
		modaltB() { var Int = 5.0 %%% 5.0; }	\n\
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
);

PTT_TEST_CASE(
	TestIntArithmeticResultsInIntsValid,
	"every MyClass is:								\n\
		myMethod() {								\n\
			var Int = 5 + 5 + 5;					\n\
			Int = 5 - 5 - 5;						\n\
			Int = 5 * 5 * 5;						\n\
			Int = 5 % 5 % 5;						\n\
			Int = 5 %% 5 %% 5;						\n\
			Int = 5 %%% 5 %%% 5;					\n\
		}											\n\
		",
	PTT_VALID
);

PTT_TEST_CASE(
	TestIntegerDivisionIsntDefault,
	"every MyClass is:								\n\
		myMethod() {								\n\
			var Int = 5 / 5;						\n\
		}										\n\
		",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestIntsAreUsableAsNums,
	"every MyClass is:							\n\
		myMethod() {							\n\
			var Num = 5;						\n\
		}										\n\
		",
	PTT_VALID
);

PTT_TEST_CASE(
	TestNumsAreNotUsableAsInts,
	"every MyClass is:							\n\
		myMethod() {							\n\
			var Int = 5.0;						\n\
		}										\n\
		",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestIntCantArithmeticAssignDouble,
	"every MyClass is:							\n\
		addNotOk(Int) { Int += 5.0; }			\n\
		subNotOk(Int) { Int -= 5.0; }			\n\
		divNotOk(Int) { Int /= 5.0; }			\n\
		mulNotOk(Int) { Int *= 5.0; }			\n\
		//modNotOk(Int) { Int %= 5.0; }			\n\
		",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	//PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestIntCantDivEqual,
	"every MyClass is:						\n\
		divNotOk(Int) { Int /= 5; }			\n\
		",
	PTT_EXPECT(TYPE_ERROR)
);
