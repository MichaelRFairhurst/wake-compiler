/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * GenericMethods.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	TestGenericsInMethodAreUsableAsArguments,
	"every MyClass is:						\n\
		{T} ignore(T) {}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericsInMethodAreUsableAsReturn,
	"every MyClass is:						\n\
		provides Exception;					\n\
		{T} T -- ignore() {					\n\
			throw Exception from this;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericsInMethodAreEquivalent,
	"every MyClass is:						\n\
		provides Exception;					\n\
		{T} T -- identity(T) {				\n\
			return T;						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericsInMethodAreUsableWithinGenerics,
	"every MyClass is:						\n\
		provides Exception;					\n\
		{T} List{T} -- identity(List{T}) {	\n\
			return List;					\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericsInMethodAreUsableWithinFunctions,
	"every MyClass is:								\n\
		provides Exception;							\n\
		{T} T -- fn() -- identity(T -- fn() myfn) {	\n\
			return myfn;							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericsInMethodAreNotInterchangable,
	"every MyClass is:						\n\
		provides Exception;					\n\
		{T, R} R -- identity(T) {			\n\
			return T;						\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestDoubleGenericDeclarationIsError,
	"every MyClass is:				\n\
		{T, T} myMethod() {}",
	PTT_EXPECT(GENERIC_TYPE_COLLISION)
);

PTT_TEST_CASE(
	TestRedeclarationOfClassGenericDeclarationIsError,
	"every MyClass{T} is:			\n\
		{T} myMethod() {}",
	PTT_EXPECT(GENERIC_TYPE_COLLISION)
);

PTT_TEST_CASE(
	TestCallGenericMethod,
	"every MyClass is:				\n\
		{T} myMethod(T) {			\n\
		}							\n\
		callingMethod(MyClass) {	\n\
			MyClass.myMethod(5);	\n\
			this.myMethod(5);		\n\
			myMethod(5);			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCallGenericMethodTwoArgsValid,
	"every MyClass is:				\n\
		{T} myMethod(T, $T) {		\n\
		}							\n\
		callingMethod(MyClass) {	\n\
			MyClass.myMethod(5, 5);	\n\
			this.myMethod(5, 5);	\n\
			myMethod(5, 5);			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCallGenericMethodTwoArgsInvalid,
	"every MyClass is:						\n\
		{T} myMethod(T, $T) {				\n\
		}									\n\
		callingMethod(MyClass) {			\n\
			MyClass.myMethod(5, 'test');	\n\
			this.myMethod(5, 'test');		\n\
			myMethod(5, 'test');			\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestCallGenericMethodDoubleParameterizationValid,
	"every MyClass{A, B} is:							\n\
		{T} myMethod(MyClass{T, T}) {					\n\
		}												\n\
		{T} myMethod2(MyClass{T, T}, $MyClass{T, T}) {	\n\
		}												\n\
		callingMethod(MyClass{Num, Num}) {				\n\
			MyClass.myMethod(MyClass);					\n\
			this.myMethod(MyClass);						\n\
			myMethod(MyClass);							\n\
			MyClass.myMethod2(MyClass, MyClass); 		\n\
			this.myMethod2(MyClass, MyClass); 			\n\
			myMethod2(MyClass, MyClass); 				\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCallGenericMethodDoubleParameterizationInvalid,
	"every MyClass{A, B} is:				\n\
		{T} myMethod(MyClass{T, T}) {		\n\
		}									\n\
		callingMethod(MyClass{Num, Text}) {	\n\
			MyClass.myMethod(MyClass); 		\n\
			this.myMethod(MyClass); 		\n\
			myMethod(MyClass); 				\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestCallGenericMethodListsOptionalsLambdasValid,
	"every MyClass is:										\n\
		{T} list(T[], $T[]) {								\n\
		}													\n\
		{T} optional(T?, $T?) {								\n\
		}													\n\
		{T} lambda(T--fn(T,T) one, T--fn(T,T) two) {		\n\
		}													\n\
		callingList(MyClass) {								\n\
			MyClass.list([1], [2]);							\n\
			this.list([1], [2]);							\n\
			list([1], [2]);									\n\
		}													\n\
		callingOptional(MyClass, Num?) {					\n\
			MyClass.optional(Num, Num);						\n\
			this.optional(Num, Num);						\n\
			optional(Num, Num);								\n\
		}													\n\
		callingLambda(MyClass, Num--fn(Num,Num) lambdafn) {	\n\
			MyClass.lambda(lambdafn, lambdafn);				\n\
			this.lambda(lambdafn, lambdafn);				\n\
			lambda(lambdafn, lambdafn);						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCallGenericMethodListsOptionalsLambdasInvalid,
	"every MyClass is:																					\n\
		{T} list(T[], $T[]) {																			\n\
		}																								\n\
		{T} optional(T?, $T?) {																			\n\
		}																								\n\
		{T} lambda(T--fn(T,T) one, T--fn(T,T) two) {													\n\
		}																								\n\
		callingList(MyClass) {																			\n\
			MyClass.list([1], ['hey']);																	\n\
			this.list([1], ['hey']);																	\n\
			list([1], ['hey']);																			\n\
		}																								\n\
		callingOptional(MyClass, Num?, Text?) {															\n\
			MyClass.optional(Num, Text);																\n\
			this.optional(Num, Text);																	\n\
			optional(Num, Text);																		\n\
		}																								\n\
		callingLambda(MyClass, Num--fn(Num,Num) one, Num--fn(Text,Num) two, Text--fn(Num,Num) three) {	\n\
			MyClass.lambda(one, two);																	\n\
			this.lambda(one, two);																		\n\
			lambda(one, two);																			\n\
			MyClass.lambda(one, three);																	\n\
			this.lambda(one, three);																	\n\
			lambda(one, three);																			\n\
		}",
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
	TestCallGenericIncompatibleTypes,
	"every MyClass{A} is:																				\n\
		{T} list(T[]) {																					\n\
		}																								\n\
		{T} optional(T?) {																				\n\
		}																								\n\
		{T} lambda(T--fn(T,T) one) {																	\n\
		}																								\n\
		{T} param(MyClass{T}) {																			\n\
		}																								\n\
		callingList(MyClass{A}, Num?, fn() myfn) {														\n\
			MyClass.list(MyClass); 																		\n\
			MyClass.list(Num); 																			\n\
			MyClass.list(myfn); 																		\n\
		}																								\n\
		callingOptional(MyClass{A}, Num[], fn() myfn) {													\n\
			MyClass.optional(MyClass);																	\n\
			this.optional(Num[]);																		\n\
			optional(myfn);																				\n\
		}																								\n\
		callingLambda(MyClass{A}, Num[], Num?) {														\n\
			MyClass.lambda(MyClass);																	\n\
			this.lambda(Num[]);																			\n\
			lambda(Num);																				\n\
		}																								\n\
		callingParam(MyClass{A}, Num, Num[], $Num?, fn() myfn) {										\n\
			MyClass.param(Num);																			\n\
			MyClass.param(myfn);																		\n\
			this.param(Num[]);																			\n\
			param($Num);																				\n\
		}",
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
	TestMatchallTypesAreAccepted,
	"every MyClass is:				\n\
		{T} list(T[], T);			\n\
		{T} optional(T?, $T);		\n\
		callIt(MyClass) {			\n\
			list([], 5);			\n\
			optional(nothing, 5);	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMatchallTypesDontBindMatchall,
	"every MyClass is:						\n\
		{T} list(T[], T, $T);				\n\
		{T} optional(T?, $T, $$T);			\n\
		callIt(MyClass) {					\n\
			list([], 5, 'hey');				\n\
			optional(nothing, 5, 'hey');	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);
