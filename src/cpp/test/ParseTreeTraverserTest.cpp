#include <boost/test/unit_test.hpp>

#include "MockSemanticErrorPrinter.h"
#include "Parser.h"
#include "ParseTreeTraverser.h"
#include "LibraryLoader.h"

BOOST_AUTO_TEST_SUITE( ParseTreeTraverserTest )

#define PTT_PRINT_TREE 0

/**
 * These macros lead the charge in the test design
 */
#define PTT_TEST_CASE( NAME, CODE, EXPECTATIONS ) \
	BOOST_AUTO_TEST_CASE( NAME ) \
	{ \
		Parser p; \
		ObjectSymbolTable table; \
		LibraryLoader loader; \
		loader.loadStdLibToTable(&table); \
		ParseTreeTraverser t(&table); \
		MockSemanticErrorPrinter e; \
		p.parse( "every Int is: every Bool is: every Text is: " CODE ); \
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

/*
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
	MethodDefinedWithVariablesSameClassnameAlbeitArrayed,
	"every MyClass is: methodWithDualArgs( MyClass, MyClass[] ) {}",
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
		andInt() { 6 && 6; }						\n\
		orInt() { 6 || 6; }							\n\
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
		arrayAccessOnInt() { 9[1]; }							\n\
		arrayAccessOnString() { 'abcd'[1]; }					\n\
		arrayAccessOnClass(MyClass) { MyClass[1]; }				\n\
		arrayAccessWithClass(MyClass, Int[]) { Int[MyClass]; }	\n\
		arrayAccessWithString(Int[]) { Int['test']; }			\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ValidArrayIndexAccessAndReturningValidTypes,
	"every MyClass is:														\n\
		intArrayBecomesInt(Int[]) { Int[0] + 3; }							\n\
		stringArrayBecomesString(Text[]) { Text[1] + 'test'; }				\n\
		stringArrayArrayBecomesString(Text[][]) { Text[1][1] + 'test'; }	\n\
		intArrayArrayBecomesInt(Int[][]) { Int[1][1] + 5; }					\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	CannotIfCondOrInvertWithAnythingButBools,
	"every MyClass is:											\n\
		ifConditionWithInt() { if(5) 5; }						\n\
		ifConditionWith( MyClass ) { if(MyClass) 5; }			\n\
		ifConditionWithText() { if('test') 5; }					\n\
		whileConditionWithInt() { while(5) 5; }					\n\
		whileConditionWith( MyClass ) { while(MyClass) 5; }		\n\
		whileConditionWithText() { while('test') 5; }			\n\
		forConditionWithInt() { for(5; 5; 5) 5; }				\n\
		forConditionWith( MyClass ) { for(5; MyClass; 5) 5; }	\n\
		forConditionWithText() { for(5; 'test'; 5) 5; }			\n\
		invertInt() { !5; }										\n\
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
		truthVariable(Bool) { if(Bool) 5; }					\n\
		truthAlias(Bool b) { if(b) 5; }						\n\
		truthLiteralsWhile() { while(true) 5; while(false) 5; }	\n\
		truthVariableWhile(Bool) { while(Bool) 5; }			\n\
		truthAliasWhile(Bool b) { while(b) 5; }				\n\
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
		Int -- returnBoolLiteralAsInt() { return true; }									\n\
		Int -- returnTextLiteralAsInt() { return 'text'; }									\n\
		Int -- returnClassAsInt(MyClass) { return MyClass; }								\n\
		Int -- returnNothingAsInt() { return; }												\n\
		MyClass -- returnBoolLiteralAsClass() { return true; }								\n\
		MyClass -- returnTextLiteralAsClass() { return 'test'; }							\n\
		MyClass -- returnIntLiteralAsClass() { return 1; }									\n\
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
		Int -- returnInt() { return 5; }											\n\
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
		assignBoolTo(Int) { Int = true; }									\n\
		assignTextTo(Int) { Int = 'text'; }									\n\
		assignA(MyClass)To(Int) { Int = MyClass; }							\n\
		assignBoolTo(Text) { Text = true; }								\n\
		assignIntTo(Text) { Text = 5; }										\n\
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
		assignBoolTo(Bool) { Bool = true; }						\n\
		assignTextTo(Text) { Text = 'text'; }						\n\
		assignIntTo(Int) { Int = 5; }								\n\
		assign(MyClass c)To(MyClass b) { c = b; }				\n\
		assignA(MyClass)To(ParentClass) { ParentClass = MyClass; }	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InvalidLambdaInvocations,
	"every UnrelatedClass is: every ParentClass is:							\n\
	every MyClass (a ParentClass) is:										\n\
		call(fn(Text) lambda)WithInt() { lambda(4); }								\n\
		call(fn(Text) lambda)WithBool() { lambda(true); }						\n\
		call(fn(Text) lambda)With(MyClass) { lambda(MyClass); }					\n\
		call(fn(Bool) lambda)With(MyClass) { lambda(MyClass); }					\n\
		call(fn(Bool) lambda)WithInt() { lambda(4); }							\n\
		call(fn(Bool) lambda)WithText() { lambda('test'); }						\n\
		call(fn(Int) lambda)WithText() { lambda('test'); }						\n\
		call(fn(Int) lambda)WithBool() { lambda(true); }							\n\
		call(fn(Int) lambda)With(MyClass) { lambda(MyClass); }					\n\
		call(fn(Int, Text) lambda)WithTextInt() { lambda('abc', 4); }				\n\
		call(fn(MyClass) lambda)With(UnrelatedClass) { lambda(UnrelatedClass); }	\n\
		call(fn(MyClass) lambda)With(ParentClass) { lambda(ParentClass); }		\n\
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
		call(fn(Text) lambda)WithText() { lambda('test'); }																	\n\
																														\n\
		call(fn(Bool) lambda)WithBool() { lambda(true); }																	\n\
																														\n\
		call(fn(Int) lambda)WithInt() { lambda(4); }																			\n\
																														\n\
		call(fn(ParentClass) lambda)With(MyClass) { lambda(MyClass); }														\n\
																														\n\
		Text -- call(Text -- fn(Text, Int, Bool, ParentClass) lambda)WithProperArgsAnd(MyClass) {							\n\
			return lambda('text', 4, true, MyClass);																		\n\
		}																												\n\
																														\n\
		Int -- call(Int -- fn(Int, Bool, ParentClass, Text) lambda)WithProperArgsAnd(MyClass) {							\n\
			return lambda(4, true, MyClass, 'text');																		\n\
		}																												\n\
																														\n\
		Bool -- call(Bool -- fn(Bool, ParentClass, Text, Int) lambda)WitProperArgsAnd(MyClass) {						\n\
			return lambda(true, MyClass, 'text', 4);																		\n\
		}																												\n\
																														\n\
		MyClass -- call(MyClass -- fn(ParentClass, Bool, Text, Int) lambda)WithProperArgsAnd(MyClass) {					\n\
			return lambda(MyClass, true, 'text', 4);																		\n\
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
	"every MyClass is: with Int = 5; with $Int = Int; with $$Int = $Int;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignMultiplePropertiesLeftOrderIsntOK,
	"every MyClass is: with Int = $Int; with $Int = $$Int; with $$Int = 5;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignPropertyFromCtorIsOK,
	"every MyClass is: with Int too = Int; needs Int;",
	PTT_VALID
)

PTT_TEST_CASE(
	NotFoundTypeInCtorIsReported,
	"every MyClass is: needs NonExist;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MulipleCtorDefinition,
	"every MyClass is: needs Int; needs Int;",
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
		MyClass -- argIsInvalidType(MyClass c, Int) { return c.argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Int) { return c.argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Int) { return c.argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Int) { return c.argIsInvalidType(c); }					\n\
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
		MyClass -- argIsInvalidType(MyClass c, Int) { return argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Int) { return argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Int) { return argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Int) { return argIsInvalidType(c); }					\n\
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
		MyClass -- argIsInvalidType(MyClass c, Int) { return this.argIsInvalidType(c, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass c, Int) { return this.argIsInvalidExpression(c, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass c, Int) { return this.argIsInvalidType(c, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass c, Int) { return this.argIsInvalidType(c); }					\n\
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
)

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
	TestDeclareAnInt,
	"every MyClass is: Int -- myfn() { var Int = 5; return Int; }",
	PTT_VALID
)

PTT_TEST_CASE(
	TestDeclareAnIntWithBadType,
	"every MyClass is: myfn() { var Int = 'test'; }",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestDeclareAnIntWithUnknownType,
	"every MyClass is: myfn() { var LInt = 'test'; }",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	TestDeclareAnIntIsScoped,
	"every MyClass is: Int -- myfn() { if(true) { var Int = 5; } return Int; }",
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
	TestProvideIntOnNonIntIsBadNewsBears,
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
			Int <- 4,						\n\
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
		needs Int, ClassA;					\n\
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
	InjectCtorWithIntNotNeeded,
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
	InjectCtorWithTextAndIntOK,
	"every ClassA is:							\n\
	every ClassB is:							\n\
		needs Int, Text;						\n\
		provides ClassB <- ClassB(5, 'test');	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	DuplicateProvisionNoSpecialtyIsError,
	"every ClassA is:				\n\
		provides ClassA, ClassA;	\n\
	",
	PTT_EXPECT(MULTIPLE_PROVISION_DEFINITION)
)

PTT_TEST_CASE(
	DuplicateProvisionWithSpecialtyIsError,
	"every ClassA is:							\n\
		provides ClassA:Poop, ClassA:Poop;	\n\
	",
	PTT_EXPECT(MULTIPLE_PROVISION_DEFINITION)
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
	"every MyClass is: provides MyClass; MyClass -- method() { return MyClass <- this; }",
	PTT_VALID
)

PTT_TEST_CASE(
	RetrieveFromClassInvalid,
	"every MyClass is: MyClass -- method() { return MyClass <- this; }",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	BindToCtorNonexistProviding,
	"every MyClass is: needs Int, Text; provides Int <- 5, MyClass <- MyClass(Int, Text);",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	RetrieveFromClassCtordSpecialtiedValid,
	"every MyDB is:										\n\
														\n\
		needs Int:Port, Text:Username;				\n\
														\n\
		provides										\n\
			Int:Port <- 55,							\n\
			Text:Username <- 'Bilbo',					\n\
			MyDB <- MyDB(Int:Port, Text:Username);	\n\
														\n\
		MyDB -- cloneMe() {								\n\
			return MyDB <- this;						\n\
		}												\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectionsAndProvisionsRequireTransitiveProvidability,
	"every ClassA is:							\n\
		needs Int:Port;						\n\
	every ClassB is:							\n\
		needs ClassA, Text:Stuff;				\n\
	every ClassC is:							\n\
		needs ClassB, Int:ID;					\n\
	every Provider is:							\n\
		provides								\n\
			ClassB,								\n\
			ClassC,								\n\
			Int:ID,							\n\
			Text:Stuff;						\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	InjectionsAndProvisionsRequireTransitiveProvidabilityOK,
	"every ClassA is:					\n\
		needs Int:Port;				\n\
										\n\
	every ClassB is:					\n\
		needs ClassA, Text:Stuff;		\n\
										\n\
	every ClassC is:					\n\
		needs ClassB, Int:ID;			\n\
										\n\
	every Provider is:					\n\
		provides						\n\
			ClassC,						\n\
			Int:ID,					\n\
			Text:Stuff,				\n\
			Int:Port,					\n\
			ClassB,						\n\
			ClassA;						\n\
	",
	PTT_VALID
)

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

PTT_TEST_CASE(
	DeclareArraysInvalidValues,
	"every MyClass is:					\n\
		assignMeToArray() {				\n\
			var MyClass[] = this;			\n\
		}								\n\
		assignNumberToClassArray() {	\n\
			var MyClass[] = 4;				\n\
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
			var $MyClass[][] = [];						\n\
		}											\n\
		declareEmptyIntArrayAndArrayArray() {		\n\
			var Int[] = [];							\n\
			var $Int[][] = [];							\n\
		}											\n\
		declareEmptyStringArrayAndArrayArray() {	\n\
			var Text[] = [];							\n\
			var $Text[][] = [];						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AssignIndexesWrongTypes,
	"every MyClass is:										\n\
		with MyClass[] = [];								\n\
		with Int[] = [];									\n\
		with Text[] = [];									\n\
		with Bool[] = [];									\n\
		assignIntToMyClasses() { MyClass[1] = 5; }			\n\
		assignTextToMyClasses() { MyClass[1] = 'test'; }	\n\
		assignBoolToMyClasses() { MyClass[1] = true; }		\n\
		assignMyClassToInts() { Int[1] = this; }			\n\
		assignTextToInts() { Int[1] = 'test'; }				\n\
		assignBoolToInts() { Int[1] = true; }				\n\
		assignMyClassToTexts() { Text[1] = this; }			\n\
		assignIntToTexts() { Text[1] = 4; }					\n\
		assignBoolToTexts() { Text[1] = true; }			\n\
		assignMyClassToBools() { Bool[1] = this; }		\n\
		assignIntToBools() { Bool[1] = 4; }				\n\
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
		with Int[][] = [];									\n\
		with Text[][] = [];									\n\
		with Bool[][] = [];								\n\
		assignToMyClasses() { MyClass[][] = []; }			\n\
		assignToMyClassesIndex() { MyClass[1] = []; }		\n\
		assignToInts() { Int[][] = []; }					\n\
		assignToIntsIndex() { Int[1] = []; }				\n\
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
		with $Int = [];										\n\
		with $Text = [];									\n\
		with $MyClass = [];									\n\
		with $Bool = [];									\n\
		with Int[] = []; //valid							\n\
		with Text[] = []; //valid							\n\
		with Bool[] = []; //valid							\n\
		with MyClass[] = []; //valid						\n\
		assignToIntsIndex() { Int[1] = []; }				\n\
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
			(MyClass <- this) = 5;			\n\
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
	UnimplementedInterfaceMethodMakesClassAbstract,
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
	UnimplementedInterfaceMethodMakesClassAbstractIndirectProvision,
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
		needs $Int;				\n\
		Int -- myMethod(Int?) {	\n\
			if Int exists {		\n\
				return Int;		\n\
			}					\n\
			return $Int;		\n\
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
	OptionalIntsBoolsAndTextsCantBeUsed,
	"every MyClass is:											\n\
		addOptionalInts(Int?, $Int?) { Int + $Int; }			\n\
		subtractOptionalInts(Int?, $Int?) { Int - $Int; }		\n\
		divideOptionalInts(Int?, $Int?) { Int / $Int; }			\n\
		multiplyOptionalInts(Int?, $Int?) { Int * $Int; }		\n\
		concatOptionalTexts(Text?, $Text?) { Text * Text; }		\n\
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
			var Int? = 4;				\n\
			if Int exists {} else { \n\
				Int + 4;			\n\
			}						\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	ReturningWithinExistsIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Int?) {	\n\
			if Int exists {			\n\
				return true;		\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsElseIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Int?) {	\n\
			if Int exists {			\n\
			} else {				\n\
				return true;		\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsWithAnElseIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Int?) {	\n\
			if Int exists {			\n\
				return true;		\n\
			} else {				\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsAndElseIsExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Int?) {	\n\
			if Int exists {			\n\
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
		Int --  myMethod() {			\n\
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
		with Int = 0;												\n\
		with Bool = false;											\n\
		with MyClass? = nothing;									\n\
		inAddition() { (Int = 3) + 2; }								\n\
		inSubtraction() { (Int = 3) - 2; }							\n\
		inDivision() { (Int = 3) / 2; }								\n\
		inMultiplication() { (Int = 3) * 2; }						\n\
		inAssignment() { Int = (Int = 3); }							\n\
		Int -- inReturn() { return Int = 3; }						\n\
		inMethodSubject() { (MyClass = this).inMethodSubject(); }	\n\
		inEquivalence() { (Int = 5) == 5; }							\n\
		inLT() { (Int = 5) <= 5; }									\n\
		inGT() { (Int = 5) >= 5; }									\n\
		inIfStmt() { if(Bool = true) {} }							\n\
		inWhileStmt() { while(Bool = true) {} }						\n\
		inMethodArg($Int) { inMethodArg(Int = 5); }					\n\
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
		with Int = 0;															\n\
		with Bool = false;														\n\
		inAddition() { (Int := 3) + 2; }										\n\
		inSubtraction() { (Int := 3) - 2; }										\n\
		inDivision() { (Int := 3) / 2; }										\n\
		inMultiplication() { (Int := 3) * 2; }									\n\
		inAssignment() { Int = (Int := 3); }									\n\
		Int -- inReturn() { return Int := 3; }									\n\
		inMethodSubject(MyClass) { (MyClass := this).inMethodSubject(this); }	\n\
		inMethodArg($Int) { inMethodArg(Int := 5); }							\n\
		inEquivalence() { (Int := 5) == 5; }									\n\
		inLT() { (Int := 5) <= 5; }												\n\
		inGT() { (Int := 5) >= 5; }												\n\
		inIfStmt() { if(Bool := true) {} }										\n\
		inWhileStmt() { while(Bool := true) {} }								\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestUsePublicPropertiesOnOtherObjectAndThis,
	"every MyClass is:						\n\
		with public Int = 3;				\n\
		with public Int aliased = 3;		\n\
		myMethod(MyClass) {					\n\
			MyClass.Int + MyClass.aliased;	\n\
			MyClass.Int = 3;				\n\
			MyClass.aliased = 3;			\n\
			this.Int + this.aliased;		\n\
			this.Int = 3;					\n\
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
		with public Int = 3;					\n\
		with public Int aliased = 3;			\n\
		myMethod(MyClass) {						\n\
			MyClass.Int == this;				\n\
			MyClass.Int.nonExistMethod();		\n\
			MyClass.aliased == this;			\n\
			MyClass.aliased.nonExistMethod();	\n\
			MyClass.Int = this;					\n\
			MyClass.Int = true;					\n\
			MyClass.aliased = this;				\n\
			MyClass.aliased = true;				\n\
			this.Int == this;					\n\
			this.Int.nonExistMethod();			\n\
			this.aliased == this;				\n\
			this.aliased.nonExistMethod();		\n\
			this.Int = this;					\n\
			this.Int = true;					\n\
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
*/

PTT_TEST_CASE(
	TestParameterizedClassReturningType,
	"every MyClass{T} is:	\n\
		T -- myMethod(T) {	\n\
			return T;		\n\
	}",
	PTT_VALID
);

BOOST_AUTO_TEST_SUITE_END()
