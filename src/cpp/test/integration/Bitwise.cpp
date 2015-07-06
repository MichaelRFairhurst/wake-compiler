/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Arithmetic.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Nathan Fairhurst
 * Revised By: Michael Fairhurst
 *
 **************************************************/

PTT_TEST_CASE(
 	BitnotNumsAndAnythingElseIsTypeError,
	"every MyClass is:														\n\
		bitnotNumByString() { ~'test'; }									\n\
		bitnotNumByObject(MyClass) { ~MyClass; }							\n\
		bitnotLambdaByNum(MyClass --fn(MyClass) lambda) { ~lambda; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitshiftleftIntsByNonIntIsTypeError,
	"every MyClass is:																\n\
		bitshiftLeftIntByString() { 5 << 'test'; }									\n\
		bitshiftLeftIntByNum() { 5 << 5.5; }										\n\
		bitshiftLeftStringByInt() { 'test' << 5; }									\n\
		bitshiftLeftIntByObject(MyClass) { 5 << MyClass; }							\n\
		bitshiftLeftObjectByInt(MyClass) { MyClass << 5; }							\n\
		bitshiftLeftLambdaByInt(MyClass --fn(MyClass) lambda) { lambda << 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitshiftleftNumsAndNonIntIsTypeError,
	"every MyClass is:																\n\
		bitshiftLeftNumByString() { 5.0 << 'test'; }								\n\
		bitshiftLeftNumByNum() { 5.0 << 5.5; }										\n\
		bitshiftLeftNumByObject(MyClass) { 5.0 << MyClass; }						\n\
		bitshiftLeftLambdaByNum(MyClass --fn(MyClass) lambda) { 5.0 << lambda; }	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitshiftrightIntsByNonIntIsTypeError,
	"every MyClass is:																\n\
		bitshiftRightIntByString() { 5 >> 'test'; }									\n\
		bitshiftRightIntByNum() { 5 >> 5.5; }										\n\
		bitshiftRightStringByInt() { 'test' >> 5; }									\n\
		bitshiftRightIntByObject(MyClass) { 5 >> MyClass; }							\n\
		bitshiftRightObjectByInt(MyClass) { MyClass >> 5; }							\n\
		bitshiftRightLambdaByInt(MyClass --fn(MyClass) lambda) { lambda >> 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitshiftrightNumsByNonIntIsTypeError,
	"every MyClass is:																	\n\
		bitshiftRightNumByString() { 5.0 >> 'test'; }									\n\
		bitshiftRightNumByNum() { 5.0 >> 5.5; }											\n\
		bitshiftRightNumByObject(MyClass) { 5.0 >> MyClass; }							\n\
		bitshiftRightLambdaByNum(MyClass --fn(MyClass) lambda) { lambda >> 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitandIntsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		bitwiseAndIntByString() { 5 & 'test'; }									\n\
		bitwiseAndStringByInt() { 'test' & 5; }									\n\
		bitwiseAndIntByObject(MyClass) { 5 & MyClass; }							\n\
		bitwiseAndObjectByInt(MyClass) { MyClass & 5; }							\n\
		bitwiseAndLambdaByInt(MyClass --fn(MyClass) lambda) { lambda & 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitandNumsAndAnythingElseIsTypeError,
	"every MyClass is:																\n\
		bitwiseAndNumByString() { 5.0 & 'test'; }									\n\
		bitwiseAndStringByNum() { 'test' & 5.0; }									\n\
		bitwiseAndNumByObject(MyClass) { 5.0 & MyClass; }							\n\
		bitwiseAndObjectByNum(MyClass) { MyClass & 5.0; }							\n\
		bitwiseAndLambdaByNum(MyClass --fn(MyClass) lambda) { lambda & 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitxorIntsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		bitwiseXorIntByString() { 5 ^ 'test'; }									\n\
		bitwiseXorStringByInt() { 'test' ^ 5; }									\n\
		bitwiseXorIntByObject(MyClass) { 5 ^ MyClass; }							\n\
		bitwiseXorObjectByInt(MyClass) { MyClass ^ 5; }							\n\
		bitwiseXorLambdaByInt(MyClass --fn(MyClass) lambda) { lambda ^ 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitxorNumsAndAnythingElseIsTypeError,
	"every MyClass is:																\n\
		bitwiseXorNumByString() { 5.0 ^ 'test'; }									\n\
		bitwiseXorStringByNum() { 'test' ^ 5.0; }									\n\
		bitwiseXorNumByObject(MyClass) { 5.0 ^ MyClass; }							\n\
		bitwiseXorObjectByNum(MyClass) { MyClass ^ 5.0; }							\n\
		bitwiseXorLambdaByNum(MyClass --fn(MyClass) lambda) { lambda ^ 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitorIntsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		bitwiseOrIntByString() { 5 | 'test'; }									\n\
		bitwiseOrStringByInt() { 'test' | 5; }									\n\
		bitwiseOrIntByObject(MyClass) { 5 | MyClass; }							\n\
		bitwiseOrObjectByInt(MyClass) { MyClass | 5; }							\n\
		bitwiseOrLambdaByInt(MyClass --fn(MyClass) lambda) { lambda | 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitorNumsAndAnythingElseIsTypeError,
	"every MyClass is:																\n\
		bitwiseOrNumByString() { 5.0 | 'test'; }									\n\
		bitwiseOrStringByNum() { 'test' | 5.0; }									\n\
		bitwiseOrNumByObject(MyClass) { 5.0 | MyClass; }							\n\
		bitwiseOrObjectByNum(MyClass) { MyClass | 5.0; }							\n\
		bitwiseOrLambdaByNum(MyClass --fn(MyClass) lambda) { lambda | 5.0; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	CantBitcompareNumWithInt,
	"every MyClass is:														\n\
		bitwiseOrNumByInt() { 5.0 | 5; }									\n\
		bitwiseOrIntByNum() { 5 | 5.0; }									\n\
		bitwiseXorNumByInt() { 5.0 ^ 5; }									\n\
		bitwiseXorIntByNum() { 5 ^ 5.0; }									\n\
		bitwiseAndNumByInt() { 5.0 & 5; }									\n\
		bitwiseAndIntByNum() { 5 & 5.0; }									\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

/*
PTT_TEST_CASE(
	TestIntCantBitwiseAssignDouble,
	"every MyClass is:								\n\
		bitOrNotOk(Int) { Int |= 5.0; }				\n\
		bitAndNotOk(Int) { Int &= 5.0; }			\n\
		bitXorNotOk(Int) { Int ^= 5.0; }			\n\
		bitRightshiftNotOk(Int) { Int <<= 5.0; }	\n\
		bitLeftshiftNotOk(Int) { Int >>= 5.0; }		\n\
		",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);
*/

PTT_TEST_CASE(
	TestBitwiseOperationReturnTypesMatchInputsValid,
	"every MyClass is:								\n\
		myMethod() {								\n\
			var Num = 0; var Int = 0;				\n\
			Num = ~5.5; 							\n\
			Num = -5.5; 							\n\
			Num = 5.5 | 5.0;						\n\
			Num = 5.5 & 5.0;						\n\
			Num = 5.5 ^ 5.0;						\n\
			Num = 5.5 << 5;							\n\
			Num = 5.5 >> 5;							\n\
			Int = ~5;								\n\
			Int = -5;								\n\
			Int = 5 | 5;							\n\
			Int = 5 & 5;							\n\
			Int = 5 ^ 5;							\n\
			Int = 5 << 5;							\n\
			Int = 5 >> 5;							\n\
		}											\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestBitwiseOperationReturnTypesMatchInputsInvalid,
	"every MyClass is:								\n\
		bitwiseNotNum() { var Int = ~5.5; }			\n\
		unaryMinusNum() { var Int = -5.5; }			\n\
		bitwiseOrNums() { var Int = 5.5 | 5.0; }	\n\
		bitwesAndNums() { var Int = 5.5 & 5.0; }	\n\
		bitwiseXorNums() { var Int = 5.5 ^ 5.0; }	\n\
		bitshiftLeftNum() { var Int = 5.5 << 5; }	\n\
		bitshiftRightNum() { var Int = 5.5 >> 5; }	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);
