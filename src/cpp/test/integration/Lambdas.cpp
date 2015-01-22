/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Lambdas.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	InvalidLambdaInvocations,
	"every UnrelatedClass is: every ParentClass is:									\n\
	every MyClass (a ParentClass) is:												\n\
		call(fn(Text) lambda)WithNum() { lambda(4); }								\n\
		call(fn(Text) lambda)WithBool() { lambda(true); }							\n\
		call(fn(Text) lambda)With(MyClass) { lambda(MyClass); }						\n\
		call(fn(Bool) lambda)With(MyClass) { lambda(MyClass); }						\n\
		call(fn(Bool) lambda)WithNum() { lambda(4); }								\n\
		call(fn(Bool) lambda)WithText() { lambda('test'); }							\n\
		call(fn(Num) lambda)WithText() { lambda('test'); }							\n\
		call(fn(Num) lambda)WithBool() { lambda(true); }							\n\
		call(fn(Num) lambda)With(MyClass) { lambda(MyClass); }						\n\
		call(fn(Num, Text) lambda)WithTextNum() { lambda('abc', 4); }				\n\
		call(fn(MyClass) lambda)With(UnrelatedClass) { lambda(UnrelatedClass); }	\n\
		call(fn(MyClass) lambda)With(ParentClass) { lambda(ParentClass); }			\n\
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
)

PTT_TEST_CASE(
	ValidLambdaInvocations,
	"every ParentClass is:																								\n\
	every MyClass (a ParentClass) is:																					\n\
																														\n\
		call(fn(Text) lambda)WithText() { lambda('test'); }																\n\
																														\n\
		call(fn(Bool) lambda)WithBool() { lambda(true); }																\n\
																														\n\
		call(fn(Num) lambda)WithNum() { lambda(4); }																	\n\
																														\n\
		call(fn(ParentClass) lambda)With(MyClass) { lambda(MyClass); }													\n\
																														\n\
		Text -- call(Text -- fn(Text, Num, Bool, ParentClass) lambda)WithProperArgsAnd(MyClass) {						\n\
			return lambda('text', 4, true, MyClass);																	\n\
		}																												\n\
																														\n\
		Num -- call(Num -- fn(Num, Bool, ParentClass, Text) lambda)WithProperArgsAnd(MyClass) {							\n\
			return lambda(4, true, MyClass, 'text');																	\n\
		}																												\n\
																														\n\
		Bool -- call(Bool -- fn(Bool, ParentClass, Text, Num) lambda)WitProperArgsAnd(MyClass) {						\n\
			return lambda(true, MyClass, 'text', 4);																	\n\
		}																												\n\
																														\n\
		MyClass -- call(MyClass -- fn(ParentClass, Bool, Text, Num) lambda)WithProperArgsAnd(MyClass) {					\n\
			return lambda(MyClass, true, 'text', 4);																	\n\
		}																												\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	ValidFile,
	"every Parent is:																	\n\
																						\n\
	every Child (a Parent) is:															\n\
		provides																		\n\
			Child,																		\n\
			Parent <- Child;															\n\
																						\n\
		Parent -- my(Parent)Function(Child, Parent b, Parent -- fn(Child) aliased) {	\n\
			Child; b; return aliased(Child);											\n\
		}																				\n\
																						\n\
		Parent -- fn(Child) -- myFunction(Child, Parent, Parent -- fn(Child) aliased) {	\n\
			return aliased;																\n\
		}																				\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	LambdaDeclarationBodyIsTypechecked,
	"every MyClass is:																	\n\
		myMethod() {																	\n\
			{ -> 5 + 'error'; };														\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	LambdaDeclarationArgsAreAvailable,
	"every MyClass is:																	\n\
		myMethod() {																	\n\
			{ Num -> 5 + Num; };														\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdaDeclarationEnclosedVariables,
	"every MyClass is:																	\n\
		myMethod() {																	\n\
			var Num = 5;																\n\
			{ $Num -> $Num + Num; };													\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdaDeclarationPropertiesAreAvailable,
	"every MyClass is:																	\n\
		with Num = 5;																	\n\
		myMethod() {																	\n\
			{ $Num -> $Num + Num; };													\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdaVarsDontLivePastScope,
	"every MyClass is:																	\n\
		myMethod() {																	\n\
			{ Num -> Num; };															\n\
			Num;																		\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);
