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
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
 	BitshiftleftNumsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		bitshiftLeftNumByString() { 5 << 'test'; }									\n\
		bitshiftLeftStringByNum() { 'test' << 5; }									\n\
		bitshiftLeftNumByObject(MyClass) { 5 << MyClass; }							\n\
		bitshiftLeftObjectByNum(MyClass) { MyClass << 5; }							\n\
		bitshiftLeftLambdaByNum(MyClass --fn(MyClass) lambda) { lambda << 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitshiftrightNumsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		bitshiftRightNumByString() { 5 >> 'test'; }									\n\
		bitshiftRightStringByNum() { 'test' >> 5; }									\n\
		bitshiftRightNumByObject(MyClass) { 5 >> MyClass; }							\n\
		bitshiftRightObjectByNum(MyClass) { MyClass >> 5; }							\n\
		bitshiftRightLambdaByNum(MyClass --fn(MyClass) lambda) { lambda >> 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitandNumsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		bitwiseAndNumByString() { 5 & 'test'; }									\n\
		bitwiseAndStringByNum() { 'test' & 5; }									\n\
		bitwiseAndNumByObject(MyClass) { 5 & MyClass; }							\n\
		bitwiseAndObjectByNum(MyClass) { MyClass & 5; }							\n\
		bitwiseAndLambdaByNum(MyClass --fn(MyClass) lambda) { lambda & 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitxorNumsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		bitwiseXorNumByString() { 5 ^ 'test'; }									\n\
		bitwiseXorStringByNum() { 'test' ^ 5; }									\n\
		bitwiseXorNumByObject(MyClass) { 5 ^ MyClass; }							\n\
		bitwiseXorObjectByNum(MyClass) { MyClass ^ 5; }							\n\
		bitwiseXorLambdaByNum(MyClass --fn(MyClass) lambda) { lambda ^ 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
 	BitorNumsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		bitwiseOrNumByString() { 5 | 'test'; }									\n\
		bitwiseOrStringByNum() { 'test' | 5; }									\n\
		bitwiseOrNumByObject(MyClass) { 5 | MyClass; }							\n\
		bitwiseOrObjectByNum(MyClass) { MyClass | 5; }							\n\
		bitwiseOrLambdaByNum(MyClass --fn(MyClass) lambda) { lambda | 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)
