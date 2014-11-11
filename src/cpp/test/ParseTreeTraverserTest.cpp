/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ParseTreeTraverserTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <boost/test/unit_test.hpp>

#include "MockSemanticErrorPrinter.h"
#include "Parser.h"
#include "ParseTreeTraverser.h"
#include "LibraryLoader.h"
#include "ErrorTracker.h"

BOOST_AUTO_TEST_SUITE( ParseTreeTraverserTest )

#define PTT_PRINT_TREE 0

/**
 * These macros lead the charge in the test design
 */
#define PTT_TEST_CASE( NAME, CODE, EXPECTATIONS ) \
	BOOST_AUTO_TEST_CASE( NAME ) \
	{ \
		Parser p; \
		ErrorTracker errors; \
		ClassSpaceSymbolTable table; \
		LibraryLoader loader; \
		loader.loadStdLibToTable(&table); \
		ParseTreeTraverser t(&table, errors); \
		MockSemanticErrorPrinter e; \
		p.parse( CODE  "\nevery Num is: every Bool is: every Text is: every List{T} is: Num -- getSize() { return 0; } every Exception is: Text[] -- getStackTrace() { var Text[] = []; return Text[]; }"); \
		if(PTT_PRINT_TREE) p.print(); \
		{ EXPECTATIONS } \
		t.traverse(p.getParseTree()); \
		t.printErrors(e); \
		BOOST_REQUIRE(e.passed()); \
	}

#define PTT_VALID ;

#define PTT_EXPECT( ERROR ) e.expect(ERROR);

/**
 * Now use them
 */
PTT_TEST_CASE(
	MultipleClassDefinition,
	"every MyClass is: every MyClass is:",
	PTT_EXPECT(MULTIPLE_CLASS_DEFINITION)
)

PTT_TEST_CASE(
	CircularInheritance,
	"every MyClassA (a MyClassB) is: every MyClassB (a MyClassA) is:",
	PTT_EXPECT(CIRCULAR_INHERITANCE)
)

PTT_TEST_CASE(
	TransitiveCircularInheritance,
	"every MyClassA (a MyClassB) is: every MyClassB (a MyClassC) is: every MyClassC (a MyClassA) is:",
	PTT_EXPECT(CIRCULAR_INHERITANCE)
)

PTT_TEST_CASE(
	ChildClassNotExists,
	"every MyClassA (a MyClassB) is:",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ExtendChildClassTwice,
	"every Parent is: every Child (a Parent, capable Parent) is:",
	PTT_EXPECT(MULTIPLE_INHERITANCE)
)

PTT_TEST_CASE(
	ExtendTwoClasses,
	"every ClassA is: every ClassB is: every ClassC (a ClassA, a ClassB) is:",
	PTT_EXPECT(MORE_THAN_ONE_SUBCLASS)
)

PTT_TEST_CASE(
	ProvideClassNotExists,
	"every ClassA is: provides NonExistClass;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ProvideBindToClassNotExists,
	"every ClassA is: provides ClassA <- NonExistClass;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ProvideBindToClassConstructorNotExists,
	"every ClassA is: provides ClassA <- NonExistClass(ClassA);",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ProvideBindToClassConstructorArgumentNotExists,
	"every ClassB is: needs ClassA; every ClassA is: provides ClassB <- ClassB(NonExistClass);",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ProvideBindToClassConstructorArgumentTwoNotExists,
	"every ClassB is: needs ClassA, ClassA also; every ClassA is: provides ClassA, ClassB <- ClassB(ClassA, NonExistClass);",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodDeclaredTwiceSimple,
	"every ClassA is: getName() {} getName() {}",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
)

PTT_TEST_CASE(
	MethodDeclaredTwiceComplex,
	"every ClassA is:										\n\
		getName(											\n\
			ClassA,											\n\
			ClassA b,										\n\
			ClassA -- fn(									\n\
				ClassA,										\n\
				ClassA -- fn()								\n\
			) alias1										\n\
		)FromEmptyFunc(										\n\
			ClassA -- fn() alias2							\n\
		) {}												\n\
															\n\
		getName(											\n\
			ClassA,											\n\
			ClassA c,										\n\
			ClassA -- fn(									\n\
				ClassA,										\n\
				ClassA -- fn()								\n\
			) alias3										\n\
		)FromEmptyFunc(										\n\
			ClassA -- fn() alias4							\n\
		) {}												\n\
		",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
)

PTT_TEST_CASE(
	MethodWithNonExistReturn,
	"every MyClass is: NonExist -- badMethod() {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithNonExistArgument,
	"every MyClass is: MyClass -- badMethod(NonExist) {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaReturnWithInvalidReturn,
	"every MyClass is: NonExist -- fn() -- badMethod() {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaReturnWithInvalidArgument,
	"every MyClass is: MyClass -- fn(NonExist) -- badMethod() {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaArgumentWithInvalidReturn,
	"every MyClass is: badMethod( NonExist -- fn() aliased ) {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaArgumentWithInvalidArgument,
	"every MyClass is: badMethod( MyClass -- fn( NonExist ) aliased ) {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
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
	MethodDefinedWithVariablesSameClassname,
	"every MyClass is: methodWithDualArgs( MyClass, MyClass ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameClassnameOneArrayedOK,
	"every MyClass is: methodWithDualArgs( MyClass, MyClass[] ) {}",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameClassnameDifferentArrayed,
	"every MyClass is: methodWithDualArgs( MyClass[], MyClass[][] ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameAlias,
	"every MyClass is: methodWithDualArgs( MyClass b, MyClass b ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameShadow,
	"every MyClass is: methodWithDualArgs( $MyClass, $MyClass ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesNonexistAlias,
	"every MyClass is: methodWithEmptyScope() { b; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesNonInstanceBoundClass,
	"every MyClass is: methodWithEmptyScope() { MyClass; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesNonInstanceBoundShadow,
	"every MyClass is: methodWithEmptyScope(MyClass) { $MyClass; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesClassnameForAliasBoundClass,
	"every MyClass is: methodWithEmptyScope(MyClass b) { MyClass; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

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
	ArrayIndexAccessTypeErrors,
	"every MyClass is:											\n\
		arrayAccessOnNum() { 9[1]; }							\n\
		arrayAccessOnString() { 'abcd'[1]; }					\n\
		arrayAccessOnBool() { true[1]; }						\n\
		arrayAccessOnNum(Num alias) { alias[1]; }				\n\
		arrayAccessOnString(Text alias) { alias[1]; }			\n\
		arrayAccessOnBool(Bool alias) { alias[1]; }				\n\
		arrayAccessOnClass(MyClass my) { my[1]; }				\n\
		arrayAccessWithClass(MyClass, Num[]) { Num[MyClass]; }	\n\
		arrayAccessWithString(Num[]) { Num['test']; }			\n\
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
)

PTT_TEST_CASE(
	ArrayIndexSymbolNotFoundErrors,
	"every MyClass is:									\n\
		arrayAccessOnClass(Text) { Text[1]; }			\n\
		arrayAccessOnClass(Num) { Num[1]; }				\n\
		arrayAccessOnClass(Bool) { Bool[1]; }			\n\
		arrayAccessOnClass(MyClass) { MyClass[1]; }		\n\
	",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	ValidArrayIndexAccessAndReturningValidTypes,
	"every MyClass is:														\n\
		intArrayBecomesNum(Num[]) { Num[0] + 3; }							\n\
		stringArrayBecomesString(Text[]) { Text[1] + 'test'; }				\n\
		stringArrayArrayBecomesString(Text[][]) { Text[1][1] + 'test'; }	\n\
		intArrayArrayBecomesNum(Num[][]) { Num[1][1] + 5; }					\n\
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

PTT_TEST_CASE(
	CatchesTypeErrorsWithinIfElseAndInvertWorks,
	"every MyClass is:													\n\
		failInIf() { if(true) 5 + 'illegal'; }							\n\
		failInElse() { if(true) 5; else 5 + 'illegal'; }				\n\
		failInIfInverted() { if(!true) 5 + 'illegal'; }					\n\
		failInElseInverted() { if(!true) 5; else 5 + 'illegal'; }		\n\
		failInWhile() { while(!true) 5 + 'illegal'; }					\n\
		failInWhileInverted() { while(!true) 5 + 'illegal'; }			\n\
		failInFor() { for(5; !true; 5) 5 + 'illegal'; }					\n\
		failInForInverted() { for(5; !true; 5) 5 + 'illegal'; }			\n\
		failInForInit() { for(5 + 'illegal'; !true; 5) 5; }				\n\
		failInForInitInverted() { for( 5 + 'illegal'; !true; 5) 5; }	\n\
		failInForIncr() { for(5; !true; 5 + 'illegal') 5; }				\n\
		failInForIncrInverted() { for(5; !true; 5 + 'illegal') 5; }		\n\
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
	ValidIfConditions,
	"every MyClass is:											\n\
		truthLiterals() { if(true) 5; if(false) 5; }			\n\
		truthNumLiteral() { if(1) 5; }			\n\
		truthVariable(Bool) { if(Bool) 5; }					\n\
		numVariable(Num) { if(Num) 5; }					\n\
		truthAlias(Bool b) { if(b) 5; }						\n\
		numAlias(Num n) { if(n) 5; }						\n\
		truthLiteralsWhile() { while(true) 5; while(false) 5; }	\n\
		truthVariableWhile(Bool) { while(Bool) 5; }			\n\
		truthAliasWhile(Bool b) { while(b) 5; }				\n\
		numLiteralWhile() { while(5) 5; }	\n\
		numVariableWhile(Num) { while(Num) 5; }			\n\
		numAliasWhile(Num n) { while(n) 5; }				\n\
	",
	PTT_VALID
)

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

PTT_TEST_CASE(
	InvalidAssignments,
	"every UnrelatedClass is:												\n\
	every ParentClass is:													\n\
	every MyClass (a ParentClass) is:										\n\
		assignBoolTo(Num) { Num = true; }									\n\
		assignTextTo(Num) { Num = 'text'; }									\n\
		assignA(MyClass)To(Num) { Num = MyClass; }							\n\
		assignBoolTo(Text) { Text = true; }								\n\
		assignNumTo(Text) { Text = 5; }										\n\
		assignA(MyClass)To(Text) { Text = MyClass; }						\n\
		assignAn(UnrelatedClass)To(MyClass) { MyClass = UnrelatedClass; }	\n\
		assignA(ParentClass)To(MyClass) { MyClass = ParentClass; }			\n\
	",
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
	ValidAssignments,
	"every ParentClass is:											\n\
	every MyClass (a ParentClass) is:								\n\
		assignBoolTo(Bool) { Bool = true; }							\n\
		assignTextTo(Text) { Text = 'text'; }						\n\
		assignNumTo(Num) { Num = 5; }								\n\
		assign(MyClass c)To(MyClass b) { c = b; }					\n\
		assignA(MyClass)To(ParentClass) { ParentClass = MyClass; }	\n\
	",
	PTT_VALID
)

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
	InvalidPropertyClassIsNotFoundError,
	"every MyClass is: with NonExist = 4;\n",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	assignNonExistClassToPropertyIsNotFoundError,
	"every MyClass is: with MyClass = NonExist;\n",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignNonSubTypeToPropertyIsTypeError,
	"every Unrelated is: every MyClass is: with MyClass = Unrelated;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignMultiplePropertiesRightOrderIsntOK,
	"every MyClass is: with Num = 5; with $Num = Num; with $$Num = $Num;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignMultiplePropertiesLeftOrderIsntOK,
	"every MyClass is: with Num = $Num; with $Num = $$Num; with $$Num = 5;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignPropertyFromCtorIsOK,
	"every MyClass is: with Num too = Num; needs Num;",
	PTT_VALID
)

PTT_TEST_CASE(
	NotFoundTypeInCtorIsReported,
	"every MyClass is: needs NonExist;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MulipleCtorDefinition,
	"every MyClass is: needs Num; needs Num;",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
)

PTT_TEST_CASE(
	CtorAndPropertiesUsableInMethods,
	"every MyClass is:									\n\
	every OtherClass is:								\n\
		needs MyClass c;								\n\
		with MyClass b = c;								\n\
		with MyClass e = c;							\n\
		MyClass -- testScope(MyClass d) {				\n\
			return e; return b; return c; return d;	\n\
		}",
	PTT_VALID
)

PTT_TEST_CASE(
	CtorBodyIsTypeChecked,
	"every MyClass is:			\n\
		needs nothing then {	\n\
			5 + true;			\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	CtorBodyHasNoReturnTypeCheckWhenValid,
	"every MyClass is: needs nothing then { return; }",
	PTT_VALID
)

PTT_TEST_CASE(
	CtorBodyHasNoReturnTypeCheckWhenInValid,
	"every MyClass is: needs nothing then { return 5; }",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	MethodInvocationOfClassNotThisComplex,
	"every MyClass is: MyClass -- method(MyClass c)Here(MyClass b)Yeah() { return c.method(c)Here(b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassNotThisSimple,
	"every MyClass is: MyClass -- method(MyClass c) { return c.method(c); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassNotThisArgumentsAreTypeChecked,
	"every MyClass is:																								\n\
		MyClass -- argIsInvalidType(MyClass c, Num) { return c.argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Num) { return c.argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Num) { return c.argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Num) { return c.argIsInvalidType(c); }					\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisComplex,
	"every MyClass is: MyClass -- method(MyClass c)Here(MyClass b)Yeah() { return method(c)Here(b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisSimple,
	"every MyClass is: MyClass -- method(MyClass c) { return method(c); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisArgumentsAreTypeChecked,
	"every MyClass is:																							\n\
		MyClass -- argIsInvalidType(MyClass c, Num) { return argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Num) { return argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Num) { return argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Num) { return argIsInvalidType(c); }					\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitComplex,
	"every MyClass is: MyClass -- method(MyClass c)Here(MyClass b)Yeah() { return this.method(c)Here(b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitSimple,
	"every MyClass is: MyClass -- method(MyClass c) { return this.method(c); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitArgumentsAreTypeChecked,
	"every MyClass is:																								\n\
		MyClass -- argIsInvalidType(MyClass c, Num) { return this.argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Num) { return this.argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Num) { return this.argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Num) { return this.argIsInvalidType(c); }					\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	CaseNotWithinSwitchIsError,
	"every MyClass is: afn() { case 5: }",
	PTT_EXPECT(ILLEGAL_CASE)
)

PTT_TEST_CASE(
	DefaultCaseNotWithinSwitchIsError,
	"every MyClass is: afn() { default: }",
	PTT_EXPECT(ILLEGAL_CASE)
)

/* SWITCH CASE LOGIC IS PENDING
PTT_TEST_CASE(
	CaseWithinSwitchIsOk,
	"every MyClass is: afn() { switch(5) { case 1: case 2: default: } }",
	PTT_VALID
)

PTT_TEST_CASE(
	CaseWithinSwitchAfterClosingPriorSwitchIsOk,
	"every MyClass is: afn() { switch(5) { case 1: switch (8) { case 6: } case 2: default: } }",
	PTT_VALID
)

PTT_TEST_CASE(
	BreakWithinSwitchAfterClosingPriorSwitchIsOk,
	"every MyClass is: afn() { switch(5) { case 1: break; switch (8) { case 6: break; } case 2: break; default: } }",
	PTT_VALID
)*/

PTT_TEST_CASE(
	BreakNotWithinSwitchForOrWhileIsError,
	"every MyClass is: afn() { break; }",
	PTT_EXPECT(ILLEGAL_BREAK)
)

PTT_TEST_CASE(
	ContinueNotWithinForOrWhileIsError,
	"every MyClass is: afn() { continue; }",
	PTT_EXPECT(ILLEGAL_CONTINUE)
)

PTT_TEST_CASE(
	BreakAndContinueWithinForOrWhileIsOK,
	"every MyClass is: afn() { while(true) { while(true) { break; continue; } for(5; true; 5) { break; continue; }; break; continue; } }",
	PTT_VALID
)

PTT_TEST_CASE(
	InexhaustiveReturnFromIfsWithoutElses,
	"every MyClass is: Bool -- afn() { if(true) return true; if(true) return true; if(true) return true; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromIfsWithoutReturns,
	"every MyClass is: Bool -- afn() { if(true) 5; else return true; if(true) 5; else return true; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromElsesWithoutReturns,
	"every MyClass is: Bool -- afn() { if(true) return true; else 5; if(true) return true; else 5; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromReturningWithinLoopsOnly,
	"every MyClass is: Bool -- afn() { while(true) return true; for(1; true; 1) return true; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromNoReturn,
	"every MyClass is: Bool -- afn() { 5 + 5; 6 + 6; 8 + 9 + 7; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnComplicated,
	"every MyClass is:				\n\
		Bool -- afn() {			\n\
			if(true) {				\n\
				if(true) {			\n\
					5 + 5;			\n\
					6 + 6;			\n\
					return true;	\n\
				}					\n\
			} else {				\n\
				5 + 6;				\n\
				7 == 10;			\n\
				while(true) {		\n\
					return true;	\n\
				}					\n\
			}						\n\
			6 + 7;					\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	ExhaustiveReturnComplicated,
	"every MyClass is:				\n\
		Bool -- afn() {			\n\
			if(true) {				\n\
				if(true) {			\n\
					5 + 5;			\n\
					6 + 6;			\n\
					return true;	\n\
				} else {			\n\
					7 / 16;			\n\
				}					\n\
				return true;		\n\
			} else {				\n\
				5 + 6;				\n\
				7 == 10;			\n\
				return true;		\n\
			}						\n\
			6 + 7;					\n\
	}",
	PTT_VALID
)

PTT_TEST_CASE(
	InexhaustiveReturnsOkWithVoidReturnType,
	"every MyClass is: afn() { } ",
	PTT_VALID
)

PTT_TEST_CASE(
	TestDeclareAnNum,
	"every MyClass is: Num -- myfn() { var Num = 5; return Num; }",
	PTT_VALID
)

PTT_TEST_CASE(
	TestDeclareAnNumWithBadType,
	"every MyClass is: myfn() { var Num = 'test'; }",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestDeclareAnNumWithUnknownType,
	"every MyClass is: myfn() { var LNum = 'test'; }",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	TestDeclareAnNumIsScoped,
	"every MyClass is: Num -- myfn() { if(true) { var Num = 5; } return Num; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	TestCastDownToInvalidClass,
	"every MyClass is: method() { (cast NonExist) this; }",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	TestCastDownToNonSubType,
	"every MyClass is: method() { (cast MyClass) 5; }",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestCastDownValid,
	"every ParentClass is:					\n\
	every MyClass (a ParentClass) is:		\n\
		method(ParentClass) {				\n\
			method((cast ParentClass) this);		\n\
		}",
	PTT_VALID
)

PTT_TEST_CASE(
	TestProvideNumOnNonNumIsBadNewsBears,
	"every MyClass is:			\n\
		provides MyClass <- 4;	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestProvideTextOnNonTextIsBadNewsBears,
	"every MyClass is:				\n\
		provides MyClass <- 'text';	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestProvideNotSubclassIsBadNewsBears,
	"every ParentClass is:					\n\
	every MyClass (a ParentClass) is:		\n\
		provides MyClass <- ParentClass;	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ValidProvisions,
	"every ParentClass is:					\n\
	every MyClass (a ParentClass) is:		\n\
		provides							\n\
			Num <- 4,						\n\
			Text <- 'Test',					\n\
			ParentClass <- MyClass,			\n\
			MyClass <- MyClass;				\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectCtorInvalidSubtype,
	"every ClassA is:						\n\
	every ClassB is:						\n\
		needs ClassA;						\n\
		provides ClassB <- ClassB(ClassB);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorIsTooFewDependencies,
	"every ClassA is:						\n\
	every ClassB is:						\n\
		needs Num, ClassA;					\n\
		provides ClassB <- ClassB(5);		\n\
	",
	PTT_EXPECT(MISMATCHED_INJECTION)
)

PTT_TEST_CASE(
	InjectCtorIsTooManyDependencies,
	"every ClassA is:								\n\
	every ClassB is:								\n\
		needs ClassA;								\n\
		provides ClassB <- ClassB(ClassB, ClassB);	\n\
	",
	PTT_EXPECT(MISMATCHED_INJECTION)
)

PTT_TEST_CASE(
	InjectCtorValid,
	"every ClassA is:								\n\
	every ClassB is:								\n\
		needs ClassA;								\n\
		provides ClassA, ClassB <- ClassB(ClassA);	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectCtorSpecializationWarning,
	"every ClassA is:								\n\
	every ClassB is:								\n\
		needs ClassA:Specified;					\n\
		provides ClassA, ClassB <- ClassB(ClassA);	\n\
	",
	PTT_EXPECT(WARNING)
)

PTT_TEST_CASE(
	InjectCtorSpecializationMatchedNotWarning,
	"every ClassA is:											\n\
	every ClassB is:											\n\
		needs ClassA:Specified;								\n\
		provides												\n\
			ClassA:Specified,									\n\
			ClassB <- ClassB(ClassA:Specified);				\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectCtorWithNumNotNeeded,
	"every ClassA is:					\n\
	every ClassB is:					\n\
		needs ClassA;					\n\
		provides ClassB <- ClassB(6);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorWithTextNotNeeded,
	"every ClassA is:						\n\
	every ClassB is:						\n\
		needs ClassA;						\n\
		provides ClassB <- ClassB('test');	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorWithTextAndNumOK,
	"every ClassA is:							\n\
	every ClassB is:							\n\
		needs Num, Text;						\n\
		provides ClassB <- ClassB(5, 'test');	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	DuplicateProvisionNoSpecialtyIsError,
	"every ClassA is:				\n\
		provides ClassA, ClassA;	\n\
	",
	PTT_EXPECT(DUPLICATE_PROPERTY_DEFINITION)
)

PTT_TEST_CASE(
	DuplicateProvisionWithSpecialtyIsError,
	"every ClassA is:							\n\
		provides ClassA:Poop, ClassA:Poop;	\n\
	",
	PTT_EXPECT(DUPLICATE_PROPERTY_DEFINITION)
)

PTT_TEST_CASE(
	DuplicateProvisionWithDifferentSpecialtyIsOk,
	"every ClassA is:							\n\
		provides ClassA:Poop, ClassA:NotPoop;	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	RetrieveFromClassValid,
	"every MyClass is: provides MyClass; MyClass -- method() { return MyClass from this; }",
	PTT_VALID
)

PTT_TEST_CASE(
	RetrieveFromClassInvalid,
	"every MyClass is: MyClass -- method() { return MyClass from this; }",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	BindToCtorNonexistProviding,
	"every MyClass is: needs Num, Text; provides Num <- 5, MyClass <- MyClass(Num, Text);",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	RetrieveFromClassCtordSpecialtiedValid,
	"every MyDB is:										\n\
														\n\
		needs Num:Port, Text:Username;				\n\
														\n\
		provides										\n\
			Num:Port <- 55,							\n\
			Text:Username <- 'Bilbo',					\n\
			MyDB <- MyDB(Num:Port, Text:Username);	\n\
														\n\
		MyDB -- cloneMe() {								\n\
			return MyDB from this;						\n\
		}												\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectionsAndProvisionsRequireTransitiveProvidability,
	"every ClassA is:							\n\
		needs Num:Port;						\n\
	every ClassB is:							\n\
		needs ClassA, Text:Stuff;				\n\
	every ClassC is:							\n\
		needs ClassB, Num:ID;					\n\
	every Provider is:							\n\
		provides								\n\
			ClassB,								\n\
			ClassC,								\n\
			Num:ID,							\n\
			Text:Stuff;						\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	InjectionsAndProvisionsRequireTransitiveProvidabilityOK,
	"every ClassA is:					\n\
		needs Num:Port;				\n\
										\n\
	every ClassB is:					\n\
		needs ClassA, Text:Stuff;		\n\
										\n\
	every ClassC is:					\n\
		needs ClassB, Num:ID;			\n\
										\n\
	every Provider is:					\n\
		provides						\n\
			ClassC,						\n\
			Num:ID,					\n\
			Text:Stuff,				\n\
			Num:Port,					\n\
			ClassB,						\n\
			ClassA;						\n\
	",
	PTT_VALID
)

/*
PTT_TEST_CASE(
	CircularDependenciesAreError,
	"every CircularA is: needs CircularB;	\n\
	every CircularB is: needs CircularC;	\n\
	every CircularC is: needs CircularA;",
	PTT_EXPECT(CIRCULAR_DEPENDENCIES)
);

PTT_TEST_CASE(
	SelfDependencyIsError,
	"every Circular is: needs Circular;",
	PTT_EXPECT(CIRCULAR_DEPENDENCIES)
);
*/

PTT_TEST_CASE(
	DeclareArraysInvalidValues,
	"every MyClass is:					\n\
		assignMeToArray() {				\n\
			var MyClass[] = this;		\n\
		}								\n\
		assignNumberToClassArray() {	\n\
			var MyClass[] = 4;			\n\
		}								\n\
		assignTextToClassArray() {		\n\
			var MyClass[] = 'test';		\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	DeclareEmptyArraysValid,
	"every MyClass is:								\n\
		declareEmptyClassArrayAndArrayArray() {		\n\
			var MyClass[] = [];						\n\
			var $MyClass[][] = [];					\n\
		}											\n\
		declareEmptyNumArrayAndArrayArray() {		\n\
			var Num[] = [];							\n\
			var $Num[][] = [];						\n\
		}											\n\
		declareEmptyStringArrayAndArrayArray() {	\n\
			var Text[] = [];						\n\
			var $Text[][] = [];						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AssignIndexesWrongTypes,
	"every MyClass is:										\n\
		with MyClass[] = [];								\n\
		with Num[] = [];									\n\
		with Text[] = [];									\n\
		with Bool[] = [];									\n\
		assignNumToMyClasses() { MyClass[1] = 5; }			\n\
		assignTextToMyClasses() { MyClass[1] = 'test'; }	\n\
		assignBoolToMyClasses() { MyClass[1] = true; }		\n\
		assignMyClassToNums() { Num[1] = this; }			\n\
		assignTextToNums() { Num[1] = 'test'; }				\n\
		assignBoolToNums() { Num[1] = true; }				\n\
		assignMyClassToTexts() { Text[1] = this; }			\n\
		assignNumToTexts() { Text[1] = 4; }					\n\
		assignBoolToTexts() { Text[1] = true; }				\n\
		assignMyClassToBools() { Bool[1] = this; }			\n\
		assignNumToBools() { Bool[1] = 4; }					\n\
		assignTextToBools() { Bool[1] = 'test'; }			\n\
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
);

PTT_TEST_CASE(
	ValidEmptyArrayAssignments,
	"every MyClass is:										\n\
		with MyClass[][] = [];								\n\
		with Num[][] = [];									\n\
		with Text[][] = [];									\n\
		with Bool[][] = [];								\n\
		assignToMyClasses() { MyClass[][] = []; }			\n\
		assignToMyClassesIndex() { MyClass[1] = []; }		\n\
		assignToNums() { Num[][] = []; }					\n\
		assignToNumsIndex() { Num[1] = []; }				\n\
		assignToTexts() { Text[][] = []; }					\n\
		assignToTextsIndex() { Text[1] = []; }				\n\
		assignToBools() { Bool[][] = []; }				\n\
		assignToBoolsIndex() { Bool[1] = []; }			\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	InvalidEmptyArrayAssignments,
	"every MyClass is:										\n\
		with $Num = [];										\n\
		with $Text = [];									\n\
		with $MyClass = [];									\n\
		with $Bool = [];									\n\
		with Num[] = []; //valid							\n\
		with Text[] = []; //valid							\n\
		with Bool[] = []; //valid							\n\
		with MyClass[] = []; //valid						\n\
		assignToNumsIndex() { Num[1] = []; }				\n\
		assignToTextsIndex() { Text[1] = []; }				\n\
		assignToBoolsIndex() { Bool[1] = []; }			\n\
		assignToMyClassesIndex() { MyClass[1] = []; }		\n\
	",
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
	InvalidLeftHandAssignments,
	"every MyClass is:						\n\
		provides MyClass;					\n\
		invalidAssignments() {				\n\
			this = 5;						\n\
			invalidAssignments() = 5;		\n\
			this.invalidAssignments() = 5;	\n\
			(MyClass from this) = 5;			\n\
		}",
	PTT_EXPECT(INVALID_ASSIGNMENT)
	PTT_EXPECT(INVALID_ASSIGNMENT)
	PTT_EXPECT(INVALID_ASSIGNMENT)
	PTT_EXPECT(INVALID_ASSIGNMENT)
);

PTT_TEST_CASE(
	AbstractMethodMakesClassAbstract,
	"every MyClass is:		\n\
		provides MyClass;	\n\
		myMethod();",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	AbstractParentMethodMakesClassAbstract,
	"every MyClass (a ParentClass) is:	\n\
		provides MyClass;				\n\
	every ParentClass is:				\n\
		myMethod();",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	UnimplementedNumerfaceMethodMakesClassAbstract,
	"every MyClass (capable ParentClass) is:	\n\
		provides MyClass;						\n\
	every ParentClass is:						\n\
		myMethod() { ; }",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	AbstractMethodMakesClassAbstractIndirectProvision,
	"every MyClass is:					\n\
		provides MyClass <- MyClass;	\n\
		myMethod();",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	AbstractParentMethodMakesClassAbstractIndirectProvision,
	"every MyClass (a ParentClass) is:	\n\
		provides MyClass <- MyClass;	\n\
	every ParentClass is:				\n\
		myMethod();",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	UnimplementedNumerfaceMethodMakesClassAbstractIndirectProvision,
	"every MyClass (capable ParentClass) is:	\n\
		provides MyClass <- MyClass;			\n\
	every ParentClass is:						\n\
		myMethod() { ; }",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	UsingAOptionalTypeIsError,
	"every MyClass is:					\n\
		myMethod(MyClass?) {			\n\
			MyClass.myMethod(MyClass);	\n\
		}",
	PTT_EXPECT(DIRECT_USE_OF_OPTIONAL_TYPE)
);

PTT_TEST_CASE(
	AssignmentOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:				\n\
		assignment(MyClass?) {		\n\
			var $MyClass = MyClass;	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	AssignmentOfOptionalTypeToOptionalTypeIsValid,
	"every MyClass is:				\n\
		assignment(MyClass?) {		\n\
			var $MyClass? = MyClass;	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ReturnOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:						\n\
		MyClass -- returnment(MyClass?) {	\n\
			return MyClass;					\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	ReturnOfOptionalTypeAsOptionalTypeIsValid,
	"every MyClass is:						\n\
		MyClass? -- returnment(MyClass?) {	\n\
			return MyClass;					\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ArgumentOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:			\n\
		optional(MyClass?) {	\n\
			real(MyClass);		\n\
		}						\n\
		real(MyClass) {			\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	ArgumentOfOptionalTypeAsOptionalTypeIsValid,
	"every MyClass is:			\n\
		optional(MyClass?) {	\n\
			real(MyClass);		\n\
		}						\n\
		real(MyClass?) {		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullAssignmentToRealTypeIsError,
	"every MyClass is:			\n\
		myMethod(MyClass) {		\n\
			MyClass = nothing;	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	NullAssignmentToOptionalTypeIsValid,
	"every MyClass is:			\n\
		myMethod(MyClass?) {	\n\
			MyClass = nothing;	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullReturnOfRealTypeIsError,
	"every MyClass is:			\n\
		MyClass -- myMethod() { \n\
			return nothing;		\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	NullReturnOfOptionalTypeIsValid,
	"every MyClass is:				\n\
		MyClass? -- myMethod() {	\n\
			return nothing;			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullArgumentOfRealArgumentIsError,
	"every MyClass is:		\n\
		real(MyClass) {		\n\
			real(nothing);	\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	ExistsClauseOnRealTypeNotAllowed,
	"every MyClass is:				\n\
		myMethod(MyClass) {			\n\
			if MyClass exists {}	\n\
		}",
	PTT_EXPECT(EXISTS_ON_NONOPTIONAL_TYPE)
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeUsable,
	"every MyClass is:				\n\
		method() {}					\n\
		myMethod(MyClass?) {		\n\
			if MyClass exists {		\n\
				MyClass.method();	\n\
			}						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeAssignable,
	"every MyClass is:					\n\
		myMethod(MyClass?) {			\n\
			if MyClass exists {			\n\
				var $MyClass = MyClass;	\n\
			}							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeReturnable,
	"every MyClass is:			\n\
		needs $Num;				\n\
		Num -- myMethod(Num?) {	\n\
			if Num exists {		\n\
				return Num;		\n\
			}					\n\
			return $Num;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeValidAsArgument,
	"every MyClass is:			\n\
		optional(MyClass?) {	\n\
			if MyClass exists {	\n\
				real(MyClass);	\n\
			}					\n\
		}						\n\
		real(MyClass) {			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AfterExistClauseOptionalsRemainOptional,
	"every MyClass is:					\n\
		print(Text) {}					\n\
		usage(MyClass?) {				\n\
			if MyClass exists {}		\n\
			MyClass.print(\"a\");		\n\
		}								\n\
		assignment(MyClass?) {			\n\
			if MyClass exists {}		\n\
			var $MyClass = MyClass;		\n\
		}								\n\
		MyClass -- returnin(MyClass?) {	\n\
			if MyClass exists {}		\n\
			return MyClass;				\n\
		}								\n\
		optional(MyClass?) {			\n\
			if MyClass exists {}		\n\
			real(MyClass);				\n\
		}								\n\
		real(MyClass) {					\n\
		}",
	PTT_EXPECT(DIRECT_USE_OF_OPTIONAL_TYPE)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	OptionalNumsBoolsAndTextsCantBeUsed,
	"every MyClass is:										\n\
		addOptionalNums(Num?, $Num?) { Num + $Num; }		\n\
		subtractOptionalNums(Num?, $Num?) { Num - $Num; }	\n\
		divideOptionalNums(Num?, $Num?) { Num / $Num; }		\n\
		multiplyOptionalNums(Num?, $Num?) { Num * $Num; }	\n\
		concatOptionalTexts(Text?, $Text?) { Text * Text; }	\n\
		notOptionalBool(Bool?) { !Bool; }					\n\
		andOptionalBools(Bool?, $Bool?) { Bool && $Bool; }	\n\
		orOptionalBools(Bool?, $Bool?) { Bool || $Bool; }	\n\
	",
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
	ExistsElseStatementIsTypeChecked,
	"every MyClass is:				\n\
		myMethod() {				\n\
			var Num? = 4;			\n\
			if Num exists {} else { \n\
				Num + 4;			\n\
			}						\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	ReturningWithinExistsIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Num?) {	\n\
			if Num exists {			\n\
				return true;		\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsElseIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Num?) {	\n\
			if Num exists {			\n\
			} else {				\n\
				return true;		\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsWithAnElseIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Num?) {	\n\
			if Num exists {			\n\
				return true;		\n\
			} else {				\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsAndElseIsExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Num?) {	\n\
			if Num exists {			\n\
				return true;		\n\
			} else {				\n\
				return false;		\n\
			}						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	MethodCallOnAnUnknownClassIsUnknownVariableError,
	"every MyClass is:				\n\
		myMethod() {				\n\
			Whatever.whatever();	\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	ReturnMethodCallOnAnUnknownClassIsUnknownVariableError,
	"every MyClass is:					\n\
		Num --  myMethod() {			\n\
			return Whatever.whatever();	\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	ReturnVoidMethodCallOnAnUnknownClassIsUnknownVariableError,
	"every MyClass is:					\n\
		myMethod() {					\n\
			return Whatever.whatever();	\n\
		}",
	PTT_EXPECT(TYPE_ERROR) // invalid return
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	UsingNormalAssignmentAsValueIsNotAllowed,
	"every MyClass is:												\n\
		with Num = 0;												\n\
		with Bool = false;											\n\
		with MyClass? = nothing;									\n\
		inAddition() { (Num = 3) + 2; }								\n\
		inSubtraction() { (Num = 3) - 2; }							\n\
		inDivision() { (Num = 3) / 2; }								\n\
		inMultiplication() { (Num = 3) * 2; }						\n\
		inAssignment() { Num = (Num = 3); }							\n\
		Num -- inReturn() { return Num = 3; }						\n\
		inMethodSubject() { (MyClass = this).inMethodSubject(); }	\n\
		inEquivalence() { (Num = 5) == 5; }							\n\
		inLT() { (Num = 5) <= 5; }									\n\
		inGT() { (Num = 5) >= 5; }									\n\
		inIfStmt() { if(Bool = true) {} }							\n\
		inWhileStmt() { while(Bool = true) {} }						\n\
		inMethodArg($Num) { inMethodArg(Num = 5); }					\n\
	",
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND) // the  uninferencable type
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
);

PTT_TEST_CASE(
	UsingValuedAssignmentAsValueIsOK,
	"every MyClass is:															\n\
		with Num = 0;															\n\
		with Bool = false;														\n\
		inAddition() { (Num := 3) + 2; }										\n\
		inSubtraction() { (Num := 3) - 2; }										\n\
		inDivision() { (Num := 3) / 2; }										\n\
		inMultiplication() { (Num := 3) * 2; }									\n\
		inAssignment() { Num = (Num := 3); }									\n\
		Num -- inReturn() { return Num := 3; }									\n\
		inMethodSubject(MyClass) { (MyClass := this).inMethodSubject(this); }	\n\
		inMethodArg($Num) { inMethodArg(Num := 5); }							\n\
		inEquivalence() { (Num := 5) == 5; }									\n\
		inLT() { (Num := 5) <= 5; }												\n\
		inGT() { (Num := 5) >= 5; }												\n\
		inIfStmt() { if(Bool := true) {} }										\n\
		inWhileStmt() { while(Bool := true) {} }								\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestUsePublicPropertiesOnOtherObjectAndThis,
	"every MyClass is:						\n\
		with public Num = 3;				\n\
		with public Num aliased = 3;		\n\
		myMethod(MyClass) {					\n\
			MyClass.Num + MyClass.aliased;	\n\
			MyClass.Num = 3;				\n\
			MyClass.aliased = 3;			\n\
			this.Num + this.aliased;		\n\
			this.Num = 3;					\n\
			this.aliased = 3;				\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AccessPropertyOnTypeErrorIsntSegfault,
	"every MyClass is: myMethod() { Stou.lalala; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	TestUsePublicPropertiesOnOtherObjectAndThisInvalid,
	"every MyClass is:							\n\
		with public Num = 3;					\n\
		with public Num aliased = 3;			\n\
		myMethod(MyClass) {						\n\
			MyClass.Num == this;				\n\
			MyClass.Num.nonExistMethod();		\n\
			MyClass.aliased == this;			\n\
			MyClass.aliased.nonExistMethod();	\n\
			MyClass.Num = this;					\n\
			MyClass.Num = true;					\n\
			MyClass.aliased = this;				\n\
			MyClass.aliased = true;				\n\
			this.Num == this;					\n\
			this.Num.nonExistMethod();			\n\
			this.aliased == this;				\n\
			this.aliased.nonExistMethod();		\n\
			this.Num = this;					\n\
			this.Num = true;					\n\
			this.aliased = this;				\n\
			this.aliased = true;				\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestParameterizedClassReturningType,
	"every MyClass{T, E} is:	\n\
		T -- myMethod(T) {		\n\
			return T;			\n\
		}						\n\
								\n\
		E -- myMethod(E) {		\n\
			return E;			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestParameterizedClassReturningWrongType,
	"every MyClass{T, E} is:	\n\
		E -- returnClass() {	\n\
			return this;		\n\
		}						\n\
								\n\
		E -- returnBool() {		\n\
			return true;		\n\
		}						\n\
								\n\
		E -- returnText() {		\n\
			return 'Test';		\n\
		}						\n\
								\n\
		E -- returnNum() {		\n\
			return 5;			\n\
		}						\n\
								\n\
		E -- myMethod(T) {		\n\
			return T;			\n\
		}						\n\
								\n\
		T -- myMethod(E) {		\n\
			return E;			\n\
		}",

	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestUseConcreteParameterizedClasses,
	"every MyClass{T} is:								\n\
		MyClass{Num} -- takeWithNum(MyClass{Num}) {		\n\
			return MyClass;								\n\
		}												\n\
		MyClass{Text} -- takeWithText(MyClass{Text}) {	\n\
			return MyClass;								\n\
		}												\n\
		MyClass{T} -- takeWithT(MyClass{T}) {			\n\
			return MyClass;								\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestUseConcreteParameterizedClassesIncorrectly,
	"every MyClass{T} is:									\n\
		MyClass{Text} -- returnNumAsText(MyClass{Num}) {	\n\
			return MyClass;									\n\
		}													\n\
		MyClass{Text} -- returnTAsText(MyClass{T}) {		\n\
			return MyClass;									\n\
		}													\n\
		MyClass{T} -- returnNumAsT(MyClass{Num}) {			\n\
			return MyClass;									\n\
		}													\n\
		MyClass{T} -- returnTextAsT(MyClass{Text}) {		\n\
			return MyClass;									\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestGenericMethodReturnTypesHaveChanged,
	"every MyClass{T} is:													\n\
		T -- returnATypeT() {												\n\
			return returnATypeT();											\n\
		}																	\n\
		Num -- returnATypeNum(MyClass{Num}) {								\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Text -- returnAText(MyClass{Text}) {								\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Bool -- returnABool(MyClass{Bool}) {								\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		MyClass{T} -- returnAMyClassT(MyClass{MyClass{T}}) {				\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Num? -- returnOptionalNumArray(MyClass{Num?}) {						\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Num[] -- returnNumArray(MyClass{Num[]}) {							\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		T[] -- returnTArray(MyClass{T[]}) {									\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		T? -- returnOptionalT(MyClass{T?}) {								\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Num? -- returnOptionalNum(MyClass{Num?}) {							\n\
			return MyClass.returnATypeT();									\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericNeedsAndProperties,
	"every Generic{T} is:														\n\
	every MyClass{T} is:														\n\
		needs T plainneed,														\n\
			T? optionalneed,													\n\
			T[] arrayedneed,													\n\
			Generic{T} containedneed,											\n\
			T -- fn() returnedneed,												\n\
			T? -- fn() returnedoptionalneed,									\n\
			T[] -- fn() returnedarrayedneed,									\n\
			Generic{T} -- fn() returnedcontainedneed,							\n\
			fn(T) argumentneed,													\n\
			fn(T?) argumentoptionalneed,										\n\
			fn(T[]) argumentarrayedneed,										\n\
			fn(Generic{T}) argumentcontainedneed;								\n\
		with T plainprop = plainneed;											\n\
		with T? optionalprop = optionalneed;									\n\
		with T[] arrayedprop = arrayedneed;										\n\
		with Generic{T} containedprop = containedneed;							\n\
		with T -- fn() returnedprop = returnedneed;								\n\
		with T? -- fn() returnedoptionalprop = returnedoptionalneed;			\n\
		with T[] -- fn() returnedarrayedprop = returnedarrayedneed;				\n\
		with Generic{T} -- fn() returnedcontainedprop = returnedcontainedneed;	\n\
		with fn(T) argumentprop = argumentneed;									\n\
		with fn(T?) argumentoptionalprop = argumentoptionalneed;				\n\
		with fn(T[]) argumentarrayedprop = argumentarrayedneed;					\n\
		with fn(Generic{T}) argumentcontainedprop = argumentcontainedneed;		\n\
		",
		PTT_VALID
);

PTT_TEST_CASE(
	TestBadParameterizationsInPropertiesAndNeeds,
	"every Generic{T} is:										\n\
	every MyClass is:											\n\
		needs Generic,											\n\
			Generic{MyClass, MyClass, MyClass},					\n\
			Generic{Generic{MyClass, MyClass}},					\n\
			MyClass{Generic{MyClass}},							\n\
			MyClass{Generic{MyClass}, Generic{MyClass}};		\n\
		with Generic = 1;										\n\
		with Generic{MyClass, MyClass, MyClass} = 1;			\n\
		with Generic{Generic{MyClass, MyClass}} = 1;			\n\
		with MyClass{Generic{MyClass}} = 1;						\n\
		with MyClass{Generic{MyClass}, Generic{MyClass}} = 1;	\n\
	",
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
);

PTT_TEST_CASE(
	TestUseGenericMethodsCorrectly,
	"every Generic{T1, T2} is:					\n\
		type1(T1) {}							\n\
		optional1(T1?) {}						\n\
		arrayed1(T1[]) {}						\n\
		type2(T2) {}							\n\
		optional2(T2?) {}						\n\
		arrayed2(T2[]) {}						\n\
		contained(Generic{T1, T2}) {}			\n\
	every MyClass is:							\n\
		needs Num,								\n\
			Num? optionalNum,					\n\
			Num[] intArray,						\n\
			Text,								\n\
			Text? optionalText,					\n\
			Text[] textArray;					\n\
		testNumText(Generic{Num,Text}) {		\n\
			Generic.type1(Num);					\n\
			Generic.optional1(optionalNum);		\n\
			Generic.arrayed1(intArray);			\n\
			Generic.type2(Text);				\n\
			Generic.optional2(optionalText);	\n\
			Generic.arrayed2(textArray);		\n\
			Generic.contained(Generic);			\n\
		}										\n\
		testTextNum(Generic{Text,Num}) {		\n\
			Generic.type1(Text);				\n\
			Generic.optional1(optionalText);	\n\
			Generic.arrayed1(textArray);		\n\
			Generic.type2(Num);					\n\
			Generic.optional2(optionalNum);		\n\
			Generic.arrayed2(intArray);			\n\
			Generic.contained(Generic);			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCanProvideAndRetrieveValidGenericImplementations,
	"every MyClass{T} is:						\n\
		provides								\n\
			MyClass{Num},						\n\
			MyClass{Text},						\n\
			MyClass{Bool},						\n\
			MyClass{Num?},						\n\
			MyClass{Text?},						\n\
			MyClass{Bool?},						\n\
			MyClass{Num[]},						\n\
			MyClass{Text[]},					\n\
			MyClass{Bool[]},					\n\
			MyClass{MyClass{Num}},				\n\
			MyClass{MyClass{Text}},				\n\
			MyClass{MyClass{Bool}},				\n\
			MyClass{MyClass{Num?}},				\n\
			MyClass{MyClass{Text?}},			\n\
			MyClass{MyClass{Bool?}},			\n\
			MyClass{MyClass{Num[]}},			\n\
			MyClass{MyClass{Text[]}},			\n\
			MyClass{MyClass{Bool[]}};			\n\
		useProvisions() {						\n\
			MyClass{Num} from this;				\n\
			MyClass{Text} from this;				\n\
			MyClass{Bool} from this;				\n\
			MyClass{Num?} from this;				\n\
			MyClass{Text?} from this;				\n\
			MyClass{Bool?} from this;				\n\
			MyClass{Num[]} from this;				\n\
			MyClass{Text[]} from this;			\n\
			MyClass{Bool[]} from this;			\n\
			MyClass{MyClass{Num}} from this;		\n\
			MyClass{MyClass{Text}} from this;		\n\
			MyClass{MyClass{Bool}} from this;		\n\
			MyClass{MyClass{Num?}} from this;		\n\
			MyClass{MyClass{Text?}} from this;	\n\
			MyClass{MyClass{Bool?}} from this;	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestUseGenericThingsInMethods,
	"every MyClass{A, B, C, D} is:										\n\
		needs A, B, C, D,												\n\
			A? optA, B? optB, C? optC, D? optD,							\n\
			A[] arrA, B[] arrB, C[] arrC, D[] arrD;						\n\
		with A propA = A;												\n\
		with B propB = B;												\n\
		with C propC = C;												\n\
		with D propD = D;												\n\
		with A? propoptA = optA;										\n\
		with B? propoptB = optB;										\n\
		with C? propoptC = optC;										\n\
		with D? propoptD = optD;										\n\
		with A[] proparrA = arrA;										\n\
		with B[] proparrB = arrB;										\n\
		with C[] proparrC = arrC;										\n\
		with D[] proparrD = arrD;										\n\
		useThem() {														\n\
			var A varA = A; varA = propA;								\n\
			var B varB = B; varB = propB;								\n\
			var C varC = C; varC = propC;								\n\
			var D varD = D; varD = propD;								\n\
			var A? varoptA = optA; varoptA = propoptA;					\n\
			var B? varoptB = optB; varoptB = propoptB;					\n\
			var C? varoptC = optC; varoptC = propoptC;					\n\
			var D? varoptD = optD; varoptD = propoptD;					\n\
			var A[] vararrA = arrA; vararrA = proparrA;					\n\
			var B[] vararrB = arrB; vararrB = proparrB;					\n\
			var C[] vararrC = arrC; vararrC = proparrC;					\n\
			var D[] vararrD = arrD; vararrD = proparrD;					\n\
			// test the types were right								\n\
			optA = nothing; optB = nothing;								\n\
			optC = nothing; optD = nothing;								\n\
			propoptA = nothing; propoptB = nothing;						\n\
			propoptC = nothing; propoptD = nothing;						\n\
			varoptA = nothing; varoptB = nothing;						\n\
			varoptC = nothing; varoptD = nothing;						\n\
			arrA[0]; arrB[0];											\n\
			arrC[0]; arrD[0];											\n\
			proparrA[0]; proparrB[0];									\n\
			proparrC[0]; proparrD[0];									\n\
			vararrA[0]; vararrB[0];										\n\
			vararrC[0]; vararrD[0];										\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCircularNeedThroughInheritanceIsInvalid,
	"every SubClass (a MyClass) is: every MyClass is: needs SubClass;",
	PTT_EXPECT(CIRCULAR_DEPENDENCIES)
);

PTT_TEST_CASE(
	TestParentsNeedsAreVisibleToChild,
	"every MyClass is: needs Text:hey; every SubClass (a MyClass) is: Text -- getHey() { return Text; }",
	PTT_VALID
);

PTT_TEST_CASE(
	TestParentNeedsAreNecessaryToProvide,
	"every MyClass is: needs Text:hey;	\n\
	every SubClass (a MyClass) is:		\n\
		provides SubClass;",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestGetWithSpecialtyInMethod,
	"every MyClass is:							\n\
		provides MyClass:Specialized;			\n\
		method() {								\n\
			var MyClass:Specialized from this;	\n\
			MyClass:Specialized from this;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGetWithoutSpecialtyInMethod,
	"every MyClass is:						\n\
		provides MyClass:Specialized;		\n\
		method() {							\n\
			var MyClass from this;			\n\
		}									\n\
		methodTwo() {						\n\
			MyClass from this;				\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestBehavioralProvisions,
	"every MyClass is:					\n\
		provides Num:Test <- { return 5; };",
	PTT_VALID
);

PTT_TEST_CASE(
	TestInvalidBehavioralProvision,
	"every MyClass is:							\n\
		provides Num:Test <- { return 'not int'; };",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestUseThisInBehavioralProvisions,
	"every MyClass is:							\n\
		provides MyClass <- { return this; };",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvidesBlockDoesntNeedTransitiveDeps,
	"every MyClass is:							\n\
		needs Text:SomeNeed;					\n\
		provides MyClass <- { return this; };",
	PTT_VALID
);

PTT_TEST_CASE(
	TestVarDeclarationInForLoopIsTemporary,
	"every MyClass is: method() {		\n\
		for(var Num = 0; true; 0) { } 	\n\
		Num += 1;						\n\
	}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	TestExistsClauseOnAlias,
	"every MyClass is: method() {		\n\
		var Num? aliased = nothing;		\n\
		if aliased exists {				\n\
			aliased + 2;				\n\
		}								\n\
	}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestExistsInvalidClauseOnAlias,
	"every MyClass is: method() {		\n\
		var Num aliased = 4;			\n\
		if aliased exists { }			\n\
	}",
	PTT_EXPECT(EXISTS_ON_NONOPTIONAL_TYPE)
);

PTT_TEST_CASE(
	TestEndOfExistsScopeDoesntRemoveVariableFromTable,
	"every MyClass is: method() {			\n\
		var Num? = nothing;					\n\
		if(true) {							\n\
			if Num exists { }				\n\
		}									\n\
		Num = nothing;						\n\
	}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestForeachAddsLowerVariables,
	"every MyClass is:						\n\
		method() {							\n\
			var Num[] = [];					\n\
			foreach(Num[]) {				\n\
				Num += Num[0];				\n\
			}								\n\
			var Text[] = [];				\n\
			foreach(Text[]) {				\n\
				Text += Text[0];			\n\
			}								\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestForeachWontOverwriteLowerVariables,
	"every MyClass is:						\n\
		lowerArrayArray() {					\n\
			var Num[][] = [];				\n\
			foreach(Num[][]) { }			\n\
		}									\n\
		overwriteVar() {					\n\
			var Num[] = [];					\n\
			var Num = 3;					\n\
			foreach(Num[]) { }				\n\
		}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestForeachOnNonArray,
	"every MyClass is:						\n\
		onThis() { foreach(this) { } }		\n\
		on(Num) { foreach(Num) { } }		\n\
		on(Text) { foreach(Text) { } }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	MemberAccessOnOptionalType,
	"every MyClass is:						\n\
		with public Text hello = '';		\n\
		method() {							\n\
		var MyClass? = nothing;				\n\
		MyClass.hello;						\n\
	}",
	PTT_EXPECT(DIRECT_USE_OF_OPTIONAL_TYPE)
);

PTT_TEST_CASE(
	TestDuplicatePropertiesThrowsAlreadyDefined,
	"every MyClass is: needs Num one, Num one;",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestExistsOnAPropertyIsDisallowed,
	"every MyClass is:						\n\
		with Num? = nothing;				\n\
		callExistsOnProperty() {			\n\
			if Num exists {}				\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestThrowNonExceptionIsTypeError,
	"every MyClass is:						\n\
		throwThis() {						\n\
			throw this;						\n\
		}									\n\
		throwNum() {						\n\
			throw 5;						\n\
		}									\n\
		throwText() {						\n\
			throw 'test';					\n\
		}									\n\
		throwBool() {						\n\
			throw true;						\n\
		}									\n\
		throwList() {						\n\
			var Exception[] = [];			\n\
			throw Exception[];				\n\
		}									\n\
		throwOptional() {					\n\
			var Exception? = nothing;		\n\
			throw Exception;				\n\
		}									\n\
		throwFunction(function fn()) {		\n\
			throw function;					\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestCatchExceptionIsNotTypeError,
	"every MyException (an Exception) is:	\n\
	every MyClass is:						\n\
		provides Exception, MyException;	\n\
		catchException() {					\n\
			try {} catch(Exception) {}		\n\
		}									\n\
		catchMyException() {				\n\
			try {} catch(MyException) {}	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCatchNonExceptionIsTypeError,
	"every MyClass is:							\n\
		catchThis() {							\n\
			try {} catch(MyClass) {}			\n\
		}										\n\
		catchNum() {							\n\
			try {} catch(Num) {}				\n\
		}										\n\
		catchText() {							\n\
			try {} catch(Text) {}				\n\
		}										\n\
		catchBool() {							\n\
			try {} catch(Bool) {}				\n\
		}										\n\
		catchList() {							\n\
			try {} catch(Exception[]) {}		\n\
		}										\n\
		catchFunction() {						\n\
			try {} catch(fn(Num) aliases) {}	\n\
		}										\n\
		catchOptional() {						\n\
			try {} catch(Exception?) {}			\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestForeachOnAnAliasNoNamespaceClash,
	"every MyClass is:						\n\
		myMethod() {						\n\
			var aliased Num[] = [];			\n\
			foreach(aliased) { Num + 5; }	\n\
		}",
);

PTT_TEST_CASE(
	TestThrowExceptionIsNotTypeError,
	"every MyException (an Exception) is:	\n\
	every MyClass is:						\n\
		provides Exception, MyException;	\n\
		throwException() {					\n\
			throw Exception from this;		\n\
		}									\n\
		throwMyException() {				\n\
			throw MyException from this;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMethodWithReturnIsExhaustiveReturns,
	"every MyClass is:						\n\
		provides Exception;					\n\
		Num -- throwInsteadOfReturn() {		\n\
			throw Exception from this;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCatchUnknownClassIsError,
	"every MyClass is:						\n\
		catchBlah() {						\n\
			try { } catch(Blah) { }			\n\
		}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
);

PTT_TEST_CASE(
	TestThrowErroneousExpression,
	"every MyClass is:						\n\
		throwNonexistMethodCall() {			\n\
			throw iDontExist();				\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	// and nothing else
);

PTT_TEST_CASE(
	TestTryBlockAndCatchBlockErrorsAreDiscovered,
	"every MyClass is:						\n\
		errorsInTry() {						\n\
			try {							\n\
				5 + true;					\n\
			}								\n\
		}									\n\
		errorsInCatch() {					\n\
			try {} catch(Exception) {		\n\
				5 + true;					\n\
			}								\n\
		}",
		PTT_EXPECT(TYPE_ERROR)
		PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestCaughtExceptionVariablesAreUsable,
	"every MyException (an Exception) is:		\n\
		needs public Exception;					\n\
	every MyClass is:							\n\
		useException() {						\n\
			try { } catch(Exception) {			\n\
				Exception.getStackTrace();		\n\
			}									\n\
		}										\n\
		useMyException() {						\n\
			try { } catch(MyException) {		\n\
				MyException.Exception;			\n\
			}									\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	testReturnWithinATryOrACatchIsNotExhaustive,
	"every MyClass is:						\n\
		Num -- returnInTry() {				\n\
			try {							\n\
				return 5;					\n\
			} catch(Exception) { }			\n\
		}									\n\
		Num -- returnInCatch() {			\n\
			try { } catch(Exception) {		\n\
				return 5;					\n\
			}								\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	testReturnWithinATryAndItsCatchIsExhaustive,
	"every MyClass is:						\n\
		Num -- returnInTry() {				\n\
			try {							\n\
				return 5;					\n\
			} catch(Exception) {			\n\
				return 5;					\n\
			}								\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	testVarsWithinBlockAndWithinCatchAreIsolateScopes,
	"every MyClass is:							\n\
		declareVarsInTryUnusableOutside() {		\n\
			try {								\n\
				var Num = 5;					\n\
			} catch(Exception) {				\n\
				Num += 1;						\n\
			}									\n\
			Num += 5;							\n\
		}										\n\
		declareVarsInCatchUnusableOutside() {	\n\
			try {								\n\
			} catch(Exception) {				\n\
				var Num = 5;					\n\
			}									\n\
			Num += 5;							\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	TestForeachOnAnAliasNamespaceClash,
	"every MyClass is:						\n\
		myMethod() {						\n\
			var aliased Num[] = [];			\n\
			var Num = 5;					\n\
			foreach(aliased) { }			\n\
		}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestAccessArrayedClassMember,
	"every MyClass is:							\n\
		with public Text[] = [];				\n\
		myMethod(MyClass) {						\n\
			MyClass.Text[2] + 'mustbetext';		\n\
			var Text[] = MyClass.Text[];		\n\
			this.Text[2] + 'mustbetext';		\n\
			Text[] = this.Text[];				\n\
			(MyClass).Text[2] + 'mustbetext';	\n\
			Text[] = (MyClass).Text[]; 			\n\
			(this).Text[2] + 'mustbetext';		\n\
			Text[] = (this).Text[]; 			\n\
			(MyClass.Text[2]) + 'mustbetext';	\n\
			Text[] = (MyClass.Text[]); 			\n\
			(this.Text[2]) + 'mustbetext';		\n\
			Text[] = (this.Text[]); 			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestAccessArrayedMemberAsNonArrayedIsException,
	"every MyClass is:						\n\
		with public Text[] = [];			\n\
		myMethod(MyClass) {					\n\
			MyClass.Text.getSize(); 		\n\
			this.Text.getSize(); 			\n\
			MyClass.Text + 'doesntexist';	\n\
			this.Text + 'doesntexist';		\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestAccessNonArrayedMemberAsArrayedIsException,
	"every MyClass is:							\n\
		with public Text = '';					\n\
		myMethod(MyClass) {						\n\
			MyClass.Text[].getSize();			\n\
			this.Text[].getSize();				\n\
			MyClass.Text[2] + 'doesntexist';	\n\
			this.Text[2] + 'doesntexist';		\n\
			(MyClass).Text[].getSize(); 		\n\
			(this).Text[].getSize(); 			\n\
			(MyClass).Text[2] + 'doesntexist';	\n\
			(this).Text[2] + 'doesntexist';		\n\
			(MyClass.Text[]).getSize(); 		\n\
			(this.Text[]).getSize(); 			\n\
			(MyClass.Text[2]) + 'doesntexist';	\n\
			(this.Text[2]) + 'doesntexist';		\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestMemberAccessNotMatchingArrayCountIsException,
	"every MyClass is:						\n\
		with public Text[][] = [];			\n\
		myMethod(MyClass) {					\n\
			MyClass.Text[]; 				\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	TestMemberAccessChainPositiveTestCase,
	"every MyClass is:															\n\
		with public MyClass[] = [];												\n\
		MyClass[] -- getMyClassList(Num) {										\n\
			return MyClass[];													\n\
		}																		\n\
		myMethod() {															\n\
			var Num = 0;														\n\
			MyClass[2].MyClass[2].getMyClassList(Num)[2].MyClass[0].MyClass[0]; \n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMemberAccessMultipleArrayedLevelPositive,
	"every MyClass is:						\n\
		with public Text[][] = [];			\n\
		myMethod(MyClass) {					\n\
			var $Text[][] = Text[][];		\n\
			var $$Text[] = Text[0];			\n\
			$Text[][] = MyClass.Text[][];	\n\
			$$Text[] = MyClass.Text[0];		\n\
			$Text[][] = this.Text[][];		\n\
			$$Text[] = this.Text[0];		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMemberAccessMultipleArrayedLevelNegative,
	"every MyClass is:						\n\
		with public Text[][] = [];			\n\
		myMethod(MyClass) {					\n\
			var $Text[][] = Text[];			\n\
			var $$Text[] = Text;			\n\
			$Text[][] = MyClass.Text[];		\n\
			$$Text[] = MyClass.Text;		\n\
			$Text[][] = this.Text[];		\n\
			$$Text[] = this.Text;			\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	TestDeclareMultipleArrayedMembersNamespaceConflict,
	"every MyClass is:					\n\
		with public Text[] = [];		\n\
		with public Text[][] = [];",
	PTT_EXPECT(DUPLICATE_PROPERTY_DEFINITION)
);

PTT_TEST_CASE(
	TestAbstractMethodsDontComplainAboutExhaustiveReturns,
	"every MyClass is:		\n\
		Text -- method();",
	PTT_VALID
);

PTT_TEST_CASE(
	TestForeachInValid,
	"every MyClass is:					\n\
		myMethod(Text[]) {				\n\
			foreach(text in Text[]) {	\n\
				text + 'another text';	\n\
			}							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestTypeErrorsWithinForeachAreCaught,
	"every MyClass is:					\n\
		myMethod(Text[]) {				\n\
			foreach(text in Text[]) {	\n\
				5 + 'another text';		\n\
			}							\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestNonSubtypeinForeachIsTypeError,
	"every MyClass is:					\n\
		myMethod(Text[]) {				\n\
			foreach(Num in Text[]) {	\n\
			}							\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestRedefinitionOfAliasInForeachInIsCaught,
	"every MyClass is:					\n\
		myMethod(Text[], Text text) {	\n\
			foreach(text in Text[]) {	\n\
			}							\n\
		}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestRedefinitionOfTypeInForeachInIsCaught,
	"every MyClass is:					\n\
		myMethod(Text[], Text) {		\n\
			foreach(Text in Text[]) {	\n\
			}							\n\
		}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestMethodCannotBeExtendedAsVoid,
	"every ParentClass is:					\n\
		Text -- method() { return ""; }		\n\
	every ChildClass (a ParentClass) is:	\n\
		method() {}							\n\
	",
	PTT_EXPECT(TYPE_ERROR) // it complains about the return type once like this
	PTT_EXPECT(INVALID_CHILD_RETURN_TYPE)
);

PTT_TEST_CASE(
	TestMethodCannotBeExtendedOrthogonally,
	"every ParentClass is:					\n\
		Text -- method() { return ''; }		\n\
	every ChildClass (a ParentClass) is:	\n\
		Num -- method() { return 2; }		\n\
	",
	PTT_EXPECT(INVALID_CHILD_RETURN_TYPE)
);

PTT_TEST_CASE(
	TestVoidMethodCanBeExtendedToReturn,
	"every ParentClass is:					\n\
		method() { }						\n\
	every ChildClass (a ParentClass) is:	\n\
		Num -- method() { return 2; }		\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMethodCannotBeExtendedToReturnCovariantType,
	"every ParentClass is:												\n\
		ChildClass -- method(ChildClass, ParentClass) {					\n\
			return ChildClass;											\n\
		}																\n\
	every ChildClass (a ParentClass) is:								\n\
		ParentClass -- method(ChildClass, ParentClass) {				\n\
			return ParentClass;											\n\
		}																\n\
	",
	PTT_EXPECT(INVALID_CHILD_RETURN_TYPE)
);

PTT_TEST_CASE(
	TestMethodCanBeExtendedToReturnContravariantType,
	"every ParentClass is:												\n\
		ParentClass -- method(ChildClass, ParentClass) {				\n\
			return ParentClass;											\n\
		}																\n\
	every ChildClass (a ParentClass) is:								\n\
		ChildClass -- method(ChildClass, ParentClass) {					\n\
			return ChildClass;											\n\
		}																\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestAccessAliasedPropertyArrayUsesAliasNoBrackets,
	"every MyClass is:					\n\
		with public Text[] arr = [];	\n\
		myMethod() {					\n\
			var Text[] = this.arr;		\n\
			var Text = this.arr[0];		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvisionArgumentNotSubtypeOfNeed,
	"every MyClass is:						\n\
		needs Num;							\n\
		provides MyClass <- MyClass(?Text);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestProvisionArgumentSubtypeOfNeed,
	"every MyClass is:						\n\
		needs Num;							\n\
		provides MyClass <- MyClass(?Num);	\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvisionContravariantArgumentIsInvalid,
	"every ClassA is:							\n\
	every ClassB (a ClassA) is:					\n\
	every MyClass is:							\n\
		needs ClassB;							\n\
		provides MyClass <- MyClass(?ClassA);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestProvisionCovariantArgumentIsValid,
	"every ClassA is:							\n\
	every ClassB (a ClassA) is:					\n\
	every MyClass is:							\n\
		needs ClassA;							\n\
		provides MyClass <- MyClass(?ClassB);	\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvisionContravariantInjectionIsInvalid,
	"every ClassA is:							\n\
	every ClassB (a ClassA) is:					\n\
	every MyClass is:							\n\
		needs ClassB;							\n\
		provides MyClass <- MyClass(ClassA),	\n\
			ClassA;								\n\
	",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestProvisionCovariantInjectionIsValid,
	"every ClassA is:							\n\
	every ClassB (a ClassA) is:					\n\
	every MyClass is:							\n\
		needs ClassA;							\n\
		provides MyClass <- MyClass(ClassB),	\n\
			ClassB;								\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvisionArgumentNonexistClass,
	"every MyClass is:							\n\
		needs Num;								\n\
		provides MyClass <- MyClass(?Nurmm);	\n\
	",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
);

PTT_TEST_CASE(
	TestCallProvisionWithNoArgsIsSymbolNotFound,
	"every MyClass is:							\n\
		needs Text;								\n\
		provides MyClass <- MyClass(?Text);		\n\
		myMethod() {							\n\
			var MyClass from this;				\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestCallProvisionWithTooManyArgsIsSymbolNotFound,
	"every MyClass is:								\n\
		needs Text;									\n\
		provides MyClass <- MyClass(?Text);			\n\
		myMethod() {								\n\
			var MyClass('Text', 'Text') from this;	\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestCallProvisionWithWrongArgsIsSymbolNotFound,
	"every MyClass is:								\n\
		needs Text;									\n\
		provides MyClass <- MyClass(?Text);			\n\
		myMethod() {								\n\
			var MyClass(5) from this;				\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestCallProvisionWithCorrectArgsIsCorrect,
	"every MyClass is:								\n\
		needs Text;									\n\
		provides MyClass <- MyClass(?Text);			\n\
		myMethod() {								\n\
			var MyClass('Hey') from this;			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCallOverloadedProvisionIsCorrect,
	"every MyClass is:								\n\
		needs Text;									\n\
		provides MyClass <- MyClass(?Text);			\n\
		provides MyClass <- MyClass('hey');			\n\
		myMethod() {								\n\
			var MyClass('Hey') from this;			\n\
			var $MyClass from this;					\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestSubtypeProvisionDontNeedToProvideNeeds,
	"every ClassA is:											\n\
		needs ClassB;											\n\
	every ClassB is:											\n\
	every ParentClassA (capable ClassA) is:						\n\
	every Provider is:											\n\
		provides ClassA <- ParentClassA;						\n\
		provides ParentClassA;									\n\
		",
	PTT_VALID
);

PTT_TEST_CASE(
	TestInjectionProvisionDontNeedToProvideNeeds,
	"every ClassA is:											\n\
		needs ClassB;											\n\
	every ClassB is:											\n\
	every Provider is:											\n\
		provides ClassA <- ClassA(ClassB:test);					\n\
		provides ClassB:test;									\n\
		",
	PTT_VALID
);

PTT_TEST_CASE(
	TestBehavioralProvisionUsingArguments,
	"every ClassA is:											\n\
	every ClassB is:											\n\
	every MyClass is:											\n\
		needs ClassA, ClassB;									\n\
		provides MyClass <- (ClassA, ClassB) {					\n\
			return this;										\n\
		};														\n\
	",
);

PTT_TEST_CASE(
	TestBehavioralProvisionTypeChecksArguments,
	"every MyClass is:											\n\
		provides MyClass <- (Num, Text, Bool) {					\n\
			Num + 'hey';										\n\
			Text + 5;											\n\
			Bool + 'hey';										\n\
			return this;										\n\
		};														\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestBehavioralProvisionTypeChecksArgumentsPositive,
	"every ClassA is:											\n\
	every ClassB is:											\n\
	every MyClass is:											\n\
		needs ClassA, ClassB;									\n\
		provides MyClass <- (Num, Text, Bool) {					\n\
			Num + 5;											\n\
			Text + 'hey';										\n\
			Bool = false;										\n\
			return this;										\n\
		};														\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestUseBehavioralProvision,
	"every ClassA is:											\n\
	every ClassB is:											\n\
	every MyClass is:											\n\
		needs ClassA, ClassB;									\n\
		provides MyClass <- (Num, Text, Bool) {					\n\
			return MyClass(Num, Text, Bool) from this;			\n\
		};														\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestValidArrayLiterals,
	"every MyClass is:											\n\
		method() {												\n\
			var Text[] = ['hey'];								\n\
			var $Text[] = ['hey', 'man'];						\n\
			var $$Text[] = [];									\n\
			var Num[] = [];										\n\
			var $$Num[] = [1];									\n\
			var $$$Num[] = [1, 2];								\n\
			var $$$$Num[][] = [[1]];							\n\
			var $$$Text[][] = [['hey']];						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestInvalidArrayLiterals,
	"every MyClass is:											\n\
		method() {												\n\
			var Num[] = ['hey'];								\n\
			var Text[] = ['hey', 3];							\n\
			var $Text[] = [true];								\n\
			var $Num[] = [nothing];								\n\
			var $$Num[] = [this];								\n\
			var $$$Num[] = [1, [2]];							\n\
			var $$$$Num[][] = [['hey']];						\n\
			var $$Text[][] = [[['hey']]];						\n\
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
	TestValidTernaries,
	"every MyClass is:											\n\
		method() {												\n\
			var Bool = true if true else true;					\n\
			var Num = 1 if 2 else 3;							\n\
			var Text = 'hey' if Bool else 'hello';				\n\
			var MyClass? = nothing if 1 else this;				\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestInvalidTernaries,
	"every MyClass is:											\n\
		method() {												\n\
			var Num = 1 if 'hey' else 3;						\n\
			var $Num = 'hey' if true else 'hello';				\n\
			var $$Num = 'hey' if true else 1;					\n\
			var MyClass = this if this else true;				\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestStrangeValidUsagesOfEmptyArray,
	"every MyClass is:											\n\
		with Num[][][] = [];									\n\
		with Num len = [].getSize();							\n\
		MyClass[] -- myMethod() {								\n\
			var Text[][] = [[], [], []];						\n\
			var Text = [[], [], []][0][0];						\n\
			var $Text = [][0][0][0]; // technically valid!		\n\
			return [];											\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestStrangeInvalidUsagesOfEmptyArray,
	"every MyClass is:											\n\
		with Num[][][] = [[[[]]]];								\n\
		with Num len = [];										\n\
		MyClass -- myMethod() {									\n\
			var Text[][] = [[3], [], []];						\n\
			return [];											\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestNestedArrayAndOptionals,
	"every MyClass is:											\n\
		needs Num??;											\n\
		with Num?[] = [nothing];								\n\
		with $Num?[] = [];										\n\
		with $$Num[]? = [];										\n\
		with $$$Num[]? = nothing;								\n\
		with $$$$Num?[][][]?[]?[] = [nothing, [nothing, [[[nothing]]]]];",
	PTT_VALID
);

PTT_TEST_CASE(
	TestSetOptionalOptionalToValues,
	"every MyClass is:											\n\
		myMethod() {											\n\
			var Num? = nothing;									\n\
			Num = [1, 1][0];									\n\
			Num = [1, nothing][0];								\n\
			Num = [nothing][0];									\n\
			var $Num?? = Num;									\n\
			$Num = nothing;										\n\
			$Num = 1;											\n\
			$Num = [1, 1][0];									\n\
			$Num = [1, nothing][0];								\n\
			$Num = [1, Num][0];									\n\
			$Num = [nothing, Num][0];							\n\
			$Num = [nothing][0];								\n\
			$Num = [Num][0];									\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericListCanBeReferencedWithoutGenericParams,
	"every MyClass is:											\n\
		with $List{Text}[] = [];								\n\
		myMethod() {											\n\
			var List{Text}[] = [];								\n\
			List[] = [];										\n\
			$List[] = [];										\n\
			this.$List[] = [];									\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestAnnotatedMethodsNeedsClassesParseAndTypeCheck,
	"@Annotated													\n\
	@AnnotatedVal(true)											\n\
	@AnnotatedParams('test', 123)								\n\
	every MyClass is:											\n\
		needs													\n\
			@Annotated											\n\
			@AnnotatedVal(true)									\n\
			@AnnotatedParams('test', 123)						\n\
			Num;												\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123)							\n\
		myTypecheckedMethod() {									\n\
			1 + 'test';											\n\
		}														\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123)							\n\
		myMethod() {											\n\
		}														\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123)							\n\
		myMethod() {											\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
);


BOOST_AUTO_TEST_SUITE_END()
