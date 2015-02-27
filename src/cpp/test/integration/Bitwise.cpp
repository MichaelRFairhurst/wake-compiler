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
		multiplyNumByString() { 5 << 'test'; }									\n\
		multiplyStringByNum() { 'test' << 5; }									\n\
		multiplyNumByObject(MyClass) { 5 << MyClass; }							\n\
		multiplyObjectByNum(MyClass) { MyClass << 5; }							\n\
		multiplyLambdaByNum(MyClass --fn(MyClass) lambda) { lambda << 5; }		\n\
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
		multiplyNumByString() { 5 >> 'test'; }									\n\
		multiplyStringByNum() { 'test' >> 5; }									\n\
		multiplyNumByObject(MyClass) { 5 >> MyClass; }							\n\
		multiplyObjectByNum(MyClass) { MyClass >> 5; }							\n\
		multiplyLambdaByNum(MyClass --fn(MyClass) lambda) { lambda >> 5; }		\n\
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
		multiplyNumByString() { 5 & 'test'; }									\n\
		multiplyStringByNum() { 'test' & 5; }									\n\
		multiplyNumByObject(MyClass) { 5 & MyClass; }							\n\
		multiplyObjectByNum(MyClass) { MyClass & 5; }							\n\
		multiplyLambdaByNum(MyClass --fn(MyClass) lambda) { lambda & 5; }		\n\
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
		multiplyNumByString() { 5 ^ 'test'; }									\n\
		multiplyStringByNum() { 'test' ^ 5; }									\n\
		multiplyNumByObject(MyClass) { 5 ^ MyClass; }							\n\
		multiplyObjectByNum(MyClass) { MyClass ^ 5; }							\n\
		multiplyLambdaByNum(MyClass --fn(MyClass) lambda) { lambda ^ 5; }		\n\
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
		multiplyNumByString() { 5 | 'test'; }									\n\
		multiplyStringByNum() { 'test' | 5; }									\n\
		multiplyNumByObject(MyClass) { 5 | MyClass; }							\n\
		multiplyObjectByNum(MyClass) { MyClass | 5; }							\n\
		multiplyLambdaByNum(MyClass --fn(MyClass) lambda) { lambda | 5; }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)