#include <boost/test/unit_test.hpp>

#include "MockSemanticErrorPrinter.h"
#include "Parser.h"
#include "ParseTreeTraverser.h"

BOOST_AUTO_TEST_SUITE( ObjectSymbolTableTest )

/**
 * These macros lead the charge in the test design
 */
#define PTT_TEST_CASE( NAME, CODE, EXPECTATIONS ) \
	BOOST_AUTO_TEST_CASE( NAME ) \
	{ \
		Parser p; \
		ParseTreeTraverser t; \
		MockSemanticErrorPrinter e; \
		p.parse( CODE ); \
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
	"every ClassA is: provides ClassA <- ClassA(NonExistClass);",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ProvideBindToClassConstructorArgumentTwoNotExists,
	"every ClassA is: provides ClassA <- ClassA(ClassA, NonExistClass);",
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
			ClassA @b,										\n\
			ClassA -- fn(									\n\
				ClassA,										\n\
				ClassA -- fn()								\n\
			) @alias1										\n\
		)FromEmptyFunc(										\n\
			ClassA -- fn() @alias2							\n\
		) {}												\n\
															\n\
		getName(											\n\
			ClassA,											\n\
			ClassA @c,										\n\
			ClassA -- fn(									\n\
				ClassA,										\n\
				ClassA -- fn()								\n\
			) @alias3										\n\
		)FromEmptyFunc(										\n\
			ClassA -- fn() @alias4							\n\
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
	"every MyClass is: badMethod( NonExist -- fn() @aliased ) {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaArgumentWithInvalidArgument,
	"every MyClass is: badMethod( MyClass -- fn( NonExist ) @aliased ) {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ValidFile,
	"every Parent is:																				\n\
																									\n\
	every Child (a Parent) is:																		\n\
		provides																					\n\
			Parent,																					\n\
			Child <- Parent;																		\n\
																									\n\
		Parent -- My(Parent)Function(Child, Parent @b, Parent -- fn(Child) @aliased) {				\n\
			Child; @b; return @aliased(Child);														\n\
		}																							\n\
																									\n\
		Child -- fn(Parent, Child) -- MyFunction(Child, Parent, Parent -- fn(Child) @aliased) {		\n\
																									\n\
		}																							\n\
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
	"every MyClass is: methodWithDualArgs( MyClass @a, MyClass @a ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameShadow,
	"every MyClass is: methodWithDualArgs( $MyClass, $MyClass ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesNonexistAlias,
	"every MyClass is: methodWithEmptyScope() { @a; }",
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
	"every MyClass is: methodWithEmptyScope(MyClass @a) { MyClass; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	MultiplyIntsAndAnythingElseIsTypeError,
	"every MyClass is:															\n\
		multiplyIntByString() { 5 * 'test'; }									\n\
		multiplyStringByInt() { 'test' * 5; }									\n\
		multiplyIntByObject(MyClass) { 5 * MyClass; }							\n\
		multiplyObjectByInt(MyClass) { MyClass * 5; }							\n\
		multiplyLambdaByInt(MyClass --fn(MyClass) @lambda) { @lambda * 5; }		\n\
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
		divideLambdaByInt(fn() @lambda) { @lambda / 5; }		\n\
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
		addLambdaByInt(fn() @lambda) { @lambda + 5; }			\n\
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
		subtractLambdaByInt(fn() @lambda) { @lambda - 5; }		\n\
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
			'test' == 'test' == True;	// test returns truth						\n\
			'test' != 'test' == True;	// test returns truth						\n\
			5 == 5 == True;				// test returns truth						\n\
			5 != 5 == True;				// test returns truth						\n\
			5 < 5 == True;				// test returns truth						\n\
			5 > 5 == True;				// test returns truth						\n\
			5 <= 5 == True;				// test returns truth						\n\
			5 >= 5 == True;				// test returns truth						\n\
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
	AndOperatorAndOrOperatorReturnTruths,
	"every MyClass is:									\n\
		andTruths() { True && True && True; }			\n\
		orTruths() { True || True || True; }			\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	ArrayIndexAccessTypeErrors,
	"every Int is: every MyClass is:							\n\
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
	"every Int is: every Text is: every MyClass is:							\n\
		intArrayBecomesInt(Int[]) { Int[0] + 3; }							\n\
		stringArrayBecomesString(Text[]) { Text[1] + 'test'; }				\n\
		stringArrayArrayBecomesString(Text[][]) { Text[1][1] + 'test'; }	\n\
		intArrayArrayBecomesInt(Int[][]) { Int[1][1] + 5; }					\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	CannotIfCondOrInvertWithAnythingButTruths,
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
		failInIf() { if(True) 5 + 'illegal'; }							\n\
		failInElse() { if(True) 5; else 5 + 'illegal'; }				\n\
		failInIfInverted() { if(!True) 5 + 'illegal'; }					\n\
		failInElseInverted() { if(!True) 5; else 5 + 'illegal'; }		\n\
		failInWhile() { while(!True) 5 + 'illegal'; }					\n\
		failInWhileInverted() { while(!True) 5 + 'illegal'; }			\n\
		failInFor() { for(5; !True; 5) 5 + 'illegal'; }					\n\
		failInForInverted() { for(5; !True; 5) 5 + 'illegal'; }			\n\
		failInForInit() { for(5 + 'illegal'; !True; 5) 5; }				\n\
		failInForInitInverted() { for( 5 + 'illegal'; !True; 5) 5; }	\n\
		failInForIncr() { for(5; !True; 5 + 'illegal') 5; }				\n\
		failInForIncrInverted() { for(5; !True; 5 + 'illegal') 5; }		\n\
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
	"every Truth is: every MyClass is:							\n\
		truthLiterals() { if(True) 5; if(False) 5; }			\n\
		truthVariable(Truth) { if(Truth) 5; }					\n\
		truthAlias(Truth @a) { if(@a) 5; }						\n\
		truthLiteralsWhile() { while(True) 5; while(False) 5; }	\n\
		truthVariableWhile(Truth) { while(Truth) 5; }			\n\
		truthAliasWhile(Truth @a) { while(@a) 5; }				\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InvalidReturnValues,
	"every Truth is: every Int is: every Text is:												\n\
		every UnrelatedClass is:																\n\
		every ParentClass is:																	\n\
		every MyClass (a ParentClass) is:														\n\
			Truth -- returnTextLiteralAsTruth() { return 'text'; }								\n\
			Truth -- returnNumberLiteralAsTruth() { return 1; }									\n\
			Truth -- returnClassAsTruth(MyClass) { return MyClass; }							\n\
			Truth -- returnNothingAsTruth() { return; }											\n\
			Text -- returnTruthLiteralAsText() { return True; }									\n\
			Text -- returnNumberLiteralAsText() { return 1; }									\n\
			Text -- returnClassAsText(MyClass) { return MyClass; }								\n\
			Text -- returnNothingAsText() { return; }											\n\
			Int -- returnTruthLiteralAsInt() { return True; }									\n\
			Int -- returnTextLiteralAsInt() { return 'text'; }									\n\
			Int -- returnClassAsInt(MyClass) { return MyClass; }								\n\
			Int -- returnNothingAsInt() { return; }												\n\
			MyClass -- returnTruthLiteralAsClass() { return True; }								\n\
			MyClass -- returnTextLiteralAsClass() { return 'test'; }							\n\
			MyClass -- returnIntLiteralAsClass() { return 1; }									\n\
			MyClass -- returnUnrelatedClassAsClass(UnrelatedClass) { return UnrelatedClass; }	\n\
			MyClass -- returnParentClassAsClass(ParentClass) { return ParentClass; }			\n\
			MyClass -- returnNothingAsMyClass() { return; }										\n\
			returnTextLiteralInVoidMethod() { return 'text'; }									\n\
			returnNumberLiteralInVoidMethod() { return 1; }										\n\
			returnClassInVoidMethod(MyClass) { return MyClass; }								\n\
			returnTruthLiteralInVoidMethod() { return True; }									\n\
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
	"every Truth is: every Int is: every Text is:										\n\
		every RelatedClass (a MyClass) is:												\n\
		every MyClass is:																\n\
			Truth -- returnTruth() { return True; }										\n\
			Text -- returnText() { return 'text'; }										\n\
			Int -- returnInt() { return 5; }											\n\
			MyClass -- returnAMyClass(MyClass) { return MyClass; }						\n\
			MyClass -- returnARelatedClass(RelatedClass) { return RelatedClass; }		\n\
			MyClass -- returnRelatedClassAsClass(RelatedClass) { return RelatedClass; }	\n\
			returnNothingInVoidMethod() { return; }	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InvalidAssignments,
	"every Truth is: every Int is: every Text is:								\n\
		every UnrelatedClass is:												\n\
		every ParentClass is:													\n\
		every MyClass (a ParentClass) is:										\n\
			assignTruthTo(Int) { Int = True; }									\n\
			assignTextTo(Int) { Int = 'text'; }									\n\
			assignA(MyClass)To(Int) { Int = MyClass; }							\n\
			assignTruthTo(Text) { Text = True; }								\n\
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
	"every Truth is: every Int is: every Text is:						\n\
		every ParentClass is:											\n\
		every MyClass (a ParentClass) is:								\n\
			assignTruthTo(Truth) { Truth = True; }						\n\
			assignTextTo(Text) { Text = 'text'; }						\n\
			assignIntTo(Int) { Int = 5; }								\n\
			assign(MyClass @a)To(MyClass @b) { @a = @b; }				\n\
			assignA(MyClass)To(ParentClass) { ParentClass = MyClass; }	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InvalidLambdaInvocations,
	"every Truth is: every Int is: every Text is:								\n\
		every UnrelatedClass is: every ParentClass is:							\n\
		every MyClass (a ParentClass) is:										\n\
			call(fn(Text) @fn)WithInt() { @fn(4); }								\n\
			call(fn(Text) @fn)WithTruth() { @fn(True); }						\n\
			call(fn(Text) @fn)With(MyClass) { @fn(MyClass); }					\n\
			call(fn(Truth) @fn)With(MyClass) { @fn(MyClass); }					\n\
			call(fn(Truth) @fn)WithInt() { @fn(4); }							\n\
			call(fn(Truth) @fn)WithText() { @fn('test'); }						\n\
			call(fn(Int) @fn)WithText() { @fn('test'); }						\n\
			call(fn(Int) @fn)WithTruth() { @fn(True); }							\n\
			call(fn(Int) @fn)With(MyClass) { @fn(MyClass); }					\n\
			call(fn(Int, Text) @fn)WithTextInt() { @fn('abc', 4); }				\n\
			call(fn(MyClass) @fn)With(UnrelatedClass) { @fn(UnrelatedClass); }	\n\
			call(fn(MyClass) @fn)With(ParentClass) { @fn(ParentClass); }		\n\
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
	"every Truth is: every Int is: every Text is:																			\n\
		every ParentClass is:																								\n\
		every MyClass (a ParentClass) is:																					\n\
																															\n\
			call(fn(Text) @fn)WithText() { @fn('test'); }																	\n\
																															\n\
			call(fn(Truth) @fn)WithTruth() { @fn(True); }																	\n\
																															\n\
			call(fn(Int) @fn)WithInt() { @fn(4); }																			\n\
																															\n\
			call(fn(ParentClass) @fn)With(MyClass) { @fn(MyClass); }														\n\
																															\n\
			Text -- call(Text -- fn(Text, Int, Truth, ParentClass) @fn)WithProperArgsAnd(MyClass) {							\n\
				return @fn('text', 4, True, MyClass);																		\n\
			}																												\n\
																															\n\
			Int -- call(Int -- fn(Int, Truth, ParentClass, Text) @fn)WithProperArgsAnd(MyClass) {							\n\
				return @fn(4, True, MyClass, 'text');																		\n\
			}																												\n\
																															\n\
			Truth -- call(Truth -- fn(Truth, ParentClass, Text, Int) @fn)WitProperArgsAnd(MyClass) {						\n\
				return @fn(True, MyClass, 'text', 4);																		\n\
			}																												\n\
																															\n\
			MyClass -- call(MyClass -- fn(ParentClass, Truth, Text, Int) @fn)WithProperArgsAnd(MyClass) {					\n\
				return @fn(MyClass, True, 'text', 4);																		\n\
			}																												\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InvalidPropertyClassIsNotFoundError,
	"every MyClass is: with NonExist;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	assignNonExistClassToPropertyIsNotFoundError,
	"every MyClass is: with MyClass = NonExist;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignNonSubTypeToPropertyIsTypeError,
	"every Unrelated is: every MyClass is: with MyClass = Unrelated;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignMultiplePropertiesRightOrderIsOK,
	"every Int is: every MyClass is: with Int = 5; with $Int = Int; with $$Int = $Int;",
	PTT_VALID
)

PTT_TEST_CASE(
	assignMultiplePropertiesWrongOrderIsntOK,
	"every Int is: every MyClass is: with Int = $Int; with $Int = $$Int; with $$Int = 5;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	assignPropertyFromCtorIsOK,
	"every MyClass is: with MyClass @too = MyClass; needs MyClass;",
	PTT_VALID
)

PTT_TEST_CASE(
	NotFoundTypeInCtorIsReported,
	"every MyClass is: needs NonExist;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MulipleCtorDefinition,
	"every MyClass is: needs MyClass; needs MyClass;",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
)

PTT_TEST_CASE(
	CtorAndPropertiesUsableInMethods,
	"every MyClass is:							\n\
		needs MyClass @a;						\n\
		with MyClass @b;						\n\
		MyClass -- testScope(MyClass @c) {		\n\
			return @a; return @b; return @c;	\n\
		}",
	PTT_VALID
)

BOOST_AUTO_TEST_SUITE_END()
