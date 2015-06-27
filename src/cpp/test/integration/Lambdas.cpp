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
		call(fn() lambda)WithNoArgs() { lambda(); }																		\n\
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

PTT_TEST_CASE(
	AssignLambdaDeclarationToLambdaValid,
	"every MyClass is:																	\n\
		myMethod() {																	\n\
			var noargs fn() = { -> ; };													\n\
			var onearg fn(Num) = { Num -> Num; };										\n\
			var twoarg fn(Num, Text) = { Num, Text -> Num; };							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AssignLambdaDeclarationToLambdaInvalid,
	"every MyClass is:																	\n\
		myMethod() {																	\n\
			var noargs fn() = { Num -> ; };												\n\
			var noargs2 fn() = { Num, Text -> ; };										\n\
			var onearg fn(Num) = { -> ; };												\n\
			var onearg2 fn(Num) = { Num, Text -> Num; };								\n\
			var noargs3 fn() = { Num, Text -> Num; };									\n\
			var twoarg fn(Num, Text) = { -> ; };										\n\
			var twoarg2 fn(Num, Text) = { Num -> Num; };								\n\
			var twoarg3 fn(Num, Text) = { Text, Num -> Num; };							\n\
		}",
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
	LambdasReturnTypesValid,
	"every LowestClass is:												\n\
	every LeftClass (a LowestClass) is:									\n\
	every RightClass (a LowestClass) is:								\n\
	every MyClass is:													\n\
		myMethod(LeftClass, RightClass) {								\n\
			var LeftClass -- fn() left = { -> return LeftClass; };		\n\
			var RightClass -- fn() right = { -> return RightClass; };	\n\
			var fn() void = { -> return; };								\n\
			var LowestClass -- fn() lowest = { ->						\n\
				if true {												\n\
					return LeftClass;									\n\
				} else {												\n\
					return RightClass;									\n\
				}														\n\
			};															\n\
			lowest = { -> return LeftClass; };							\n\
			lowest = { -> return RightClass; };							\n\
			void = { -> return LeftClass; };							\n\
			void = { -> return RightClass; };							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdasReturnTypesInvalid,
	"every LowestClass is:												\n\
	every LeftClass (a LowestClass) is:									\n\
	every RightClass (a LowestClass) is:								\n\
	every MyClass is:													\n\
		myMethod(LeftClass, RightClass, LowestClass) {					\n\
			var LeftClass -- fn() left = { -> return LowestClass; };	\n\
			var RightClass -- fn() right = { -> return LowestClass; };	\n\
			var LowestClass -- fn() lowest = { ->						\n\
				if true {												\n\
					return;												\n\
				} else {												\n\
					return RightClass;									\n\
				}														\n\
			};															\n\
			var LeftClass -- fn() left2 = { -> return; };				\n\
			var RightClass -- fn() right2 = { -> return; };				\n\
			var LowestClass -- fn() lowest2 = { -> return; };			\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_INFERENCE_FAILURE)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	LambdasExhaustiveReturnsValid,
	"every MyClass is:				\n\
		myMethod() {				\n\
			{ -> ; };				\n\
			{ -> return; };			\n\
			{ -> return 4; };		\n\
			{ ->					\n\
				if true {			\n\
					return 4;		\n\
				} else {			\n\
					return 2;		\n\
				}					\n\
			};						\n\
			{ ->					\n\
				if true {			\n\
					return;			\n\
				}					\n\
			};						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdasExhaustiveReturnsInvalid,
	"every MyClass is:				\n\
		myMethod() {				\n\
			{ ->					\n\
				if true {			\n\
					return 4;		\n\
				}					\n\
			};						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	LambdasNoMethodFoundByCasingInvalid,
	"every MyClass is:					\n\
		inferringMethod() {				\n\
			hintingMethod({ one, two ->	\n\
				5 + one + two;			\n\
			});							\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_INFERENCE_FAILURE)
	PTT_EXPECT(TYPE_INFERENCE_FAILURE)
);

PTT_TEST_CASE(
	LambdasInferAliasedVars,
	"every MyClass is:						\n\
		hintingMethod(alias fn(Num, Num)) {	\n\
		}									\n\
		inferringMethod() {					\n\
			hintingMethod({ one, two ->		\n\
				5 + one + two;				\n\
			});								\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdaDeclarationNotLambdaArgumentFails,
	"every MyClass is:						\n\
		hintingMethod(Num) {				\n\
		}									\n\
		inferringMethod() {					\n\
			hintingMethod({ one, two ->		\n\
				5 + one + two;				\n\
			});								\n\
		}",
	PTT_EXPECT(TYPE_INFERENCE_FAILURE)
	PTT_EXPECT(TYPE_INFERENCE_FAILURE)
);

PTT_TEST_CASE(
	LambdaDeclarationArgumentHasNoArgumentsFails,
	"every MyClass is:						\n\
		hintingMethod(fn() myfun) {			\n\
		}									\n\
		inferringMethod() {					\n\
			hintingMethod({ two ->			\n\
			});								\n\
		}",
	PTT_EXPECT(TYPE_INFERENCE_FAILURE)
);

PTT_TEST_CASE(
	LambdaDeclarationArgumentHasTooFewArgumentsFails,
	"every MyClass is:						\n\
		hintingMethod(fn(Num) myfun) {		\n\
		}									\n\
		inferringMethod() {					\n\
			hintingMethod({ one, two ->		\n\
			});								\n\
		}",
	PTT_EXPECT(TYPE_INFERENCE_FAILURE)
);

PTT_TEST_CASE(
	EqualLambdaTypesHaveCommonSubtypeOfThemselves,
	"every MyClass is:														\n\
		myMethod(MyClass -- fn(MyClass) one, MyClass -- fn(MyClass) two) {	\n\
			var MyClass -- fn(MyClass) common = [one, two][0];				\n\
		}																	\n\
		myMethod(MyClass -- fn() one, MyClass -- fn() two) {				\n\
			var MyClass -- fn() common = [one, two][0];						\n\
		}																	\n\
		myMethod(fn(MyClass) one, fn(MyClass) two) {						\n\
			var fn(MyClass) common = [one, two][0];							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdaVoidNonVoidTypesHaveCommonSubtypeOfVoid,
	"every MyClass is:											\n\
		myMethod(MyClass -- fn(MyClass) one, fn(MyClass) two) {	\n\
			var MyClass -- fn(MyClass) common = [one, two][0];	\n\
			var MyClass -- fn(MyClass) again = [two, one][0];	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	LambdaNonVoidTypesHaveCommonSubtypeOfReturn,
	"every ParentClass is:												\n\
	every ChildClass1 (a ParentClass) is:								\n\
	every ChildClass2 (a ParentClass) is:								\n\
		myMethod(ChildClass1 -- fn() one, ChildClass2 -- fn() two) {	\n\
			var ParentClass -- fn() common = [one, two][0];				\n\
			common = [two, one][0];										\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdaSameNumberOfArgumentTypesNoCommonSubtype,
	"every ParentClass is:										\n\
	every ChildClass1 (a ParentClass) is:						\n\
	every ChildClass2 (a ParentClass) is:						\n\
		myMethod(fn(ChildClass1) one, fn(ChildClass2) two) {	\n\
			[one, two];											\n\
			[two, one];											\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	LambdaDifferentNumberOfArgumentTypesNoCommonSubtype,
	"every MyClass is:													\n\
		myMethod(fn() one, fn(MyClass) two, fn(MyClass, Num) three) {	\n\
			[one, two];													\n\
			[one, three];												\n\
			[two, one];													\n\
			[two, three];												\n\
			[three, one];												\n\
			[three, two];												\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	LambdasNoCommonReturnTypeIsVoidReturn,
	"every MyClass is:															\n\
		myMethod(Text -- fn() one, Num -- fn() two, MyClass -- fn() three) {	\n\
			var Num -- fn() blah = [one, two][0];								\n\
			var Num -- fn() blahh = [two, one][0];								\n\
			var Text -- fn() blahhh = [one, two][0];							\n\
			var Text -- fn() blahhhh = [two, one][0];							\n\
			var Num -- fn() blahhhhh = [one, three][0];							\n\
			var Num -- fn() blahhhhhh = [three, one][0];						\n\
			var MyClass -- fn() blahhhhhhh = [one, three][0];					\n\
			var MyClass -- fn() blahhhhhhhh = [three, one][0];					\n\
			var Text -- fn() blahhhhhhhh = [two, three][0];						\n\
			var Text -- fn() blahhhhhhhhh = [three, two][0];					\n\
			var MyClass -- fn() blahhhhhhhhhh = [two, three][0];				\n\
			var MyClass -- fn() blahhhhhhhhhhh = [three, two][0];				\n\
			var fn() blahhhhhhhhhhhh = [one, two][0];							\n\
			var fn() blahhhhhhhhhhhhh = [two, one][0];							\n\
			var fn() blahhhhhhhhhhhhhh = [one, three][0];						\n\
			var fn() blahhhhhhhhhhhhhhh = [three, one][0];						\n\
			var fn() blahhhhhhhhhhhhhhhh = [two, three][0];						\n\
			var fn() blahhhhhhhhhhhhhhhhh = [three, two][0];					\n\
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
	LambdasPassedIntoNonLambdaArgsDoesntTryToInfer,
	"every MyClass is:				\n\
		myMethod(Num) { }			\n\
		otherMethod() {				\n\
			myMethod({ t -> ;});	\n\
		}",
	PTT_EXPECT(TYPE_INFERENCE_FAILURE)
);

PTT_TEST_CASE(
	LambdaArgsAreHandledLastForBindingMethodGennerics,
	"every MyClass is:																			\n\
		{R, A, B, C, D} R -- callfour(R -- fn(A, B, C, D) callme, A, B, C, D) {					\n\
			return callme(A, B, C, D);															\n\
		}																						\n\
		MyClass -- useIt() {																	\n\
			return callfour({a, b, c, d -> a + b; c + d; return this; }, 1, 2, 'hey', 'man');	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	LambdasWithEqualsRocketReturnsWithOrWithoutSemicolon,
	"every MyClass is:										\n\
		method() {											\n\
			var Bool -- fn() myfn1 = { => true };			\n\
			var Bool -- fn() myfn2 = { => true; };			\n\
			var Bool -- fn(Bool) myfn3 = { Bool => true };	\n\
			var Bool -- fn(Bool) myfn4 = { Bool => true; };	\n\
		}",
	PTT_VALID
);
