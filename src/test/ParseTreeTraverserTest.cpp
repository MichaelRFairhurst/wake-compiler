#include <boost/test/unit_test.hpp>

#include "MockSemanticErrorPrinter.h"
#include "Parser.h"
#include "ParseTreeTraverser.h"
#include "LibraryLoader.h"

BOOST_AUTO_TEST_SUITE( ObjectSymbolTableTest )

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
		loader.loadToTable(&table); \
		ParseTreeTraverser t(&table); \
		MockSemanticErrorPrinter e; \
		p.parse( CODE ); \
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
	"every ClassB is: needs ClassA, ClassA @also; every ClassA is: provides ClassA, ClassB <- ClassB(ClassA, NonExistClass);",
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
			Child,																					\n\
			Parent <- Child;																		\n\
																									\n\
		Parent -- My(Parent)Function(Child, Parent @b, Parent -- fn(Child) @aliased) {				\n\
			Child; @b; return @aliased(Child);														\n\
		}																							\n\
																									\n\
		Parent -- fn(Child) -- MyFunction(Child, Parent, Parent -- fn(Child) @aliased) {			\n\
			return @aliased;																		\n\
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
	"every MyClass is:											\n\
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
	"every UnrelatedClass is:																\n\
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
	"every RelatedClass (a MyClass) is:												\n\
	every MyClass is:																\n\
		Truth -- returnTruth() { return True; }										\n\
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
	"every ParentClass is:											\n\
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
	"every UnrelatedClass is: every ParentClass is:							\n\
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
	"every ParentClass is:																								\n\
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
	"every MyClass is: with Int = 5; with $Int = Int; with $$Int = $Int;",
	PTT_VALID
)

PTT_TEST_CASE(
	AssignMultiplePropertiesWrongOrderIsntOK,
	"every MyClass is: with Int = $Int; with $Int = $$Int; with $$Int = 5;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignPropertyFromCtorIsOK,
	"every MyClass is: with Int @too = Int; needs Int;",
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
		needs MyClass @a;								\n\
		with MyClass @b;								\n\
		with MyClass @c = @b;							\n\
		MyClass -- testScope(MyClass @d) {				\n\
			return @a; return @b; return @c; return @d;	\n\
		}",
	PTT_VALID
)

PTT_TEST_CASE(
	CtorBodyIsTypeChecked,
	"every MyClass is:			\n\
		needs nothing then {	\n\
			5 + True;			\n\
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
	"every MyClass is: MyClass -- method(MyClass @a)Here(MyClass@b)Yeah() { return @a.method(@a)Here(@b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassNotThisSimple,
	"every MyClass is: MyClass -- method(MyClass @a) { return @a.method(@a); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassNotThisArgumentsAreTypeChecked,
	"every MyClass is:																								\n\
		MyClass -- argIsInvalidType(MyClass @a, Int) { return @a.argIsInvalidType(@a, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass @a, Int) { return @a.argIsInvalidExpression(@a, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass @a, Int) { return @a.argIsInvalidType(@a, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass @a, Int) { return @a.argIsInvalidType(@a); }					\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisComplex,
	"every MyClass is: MyClass -- method(MyClass @a)Here(MyClass@b)Yeah() { return method(@a)Here(@b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisSimple,
	"every MyClass is: MyClass -- method(MyClass @a) { return method(@a); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisArgumentsAreTypeChecked,
	"every MyClass is:																							\n\
		MyClass -- argIsInvalidType(MyClass @a, Int) { return argIsInvalidType(@a, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass @a, Int) { return argIsInvalidExpression(@a, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass @a, Int) { return argIsInvalidType(@a, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass @a, Int) { return argIsInvalidType(@a); }					\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitComplex,
	"every MyClass is: MyClass -- method(MyClass @a)Here(MyClass@b)Yeah() { return this.method(@a)Here(@b)Yeah(); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitSimple,
	"every MyClass is: MyClass -- method(MyClass @a) { return this.method(@a); }",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodInvocationOfClassUsingThisExplicitArgumentsAreTypeChecked,
	"every MyClass is:																								\n\
		MyClass -- argIsInvalidType(MyClass @a, Int) { return this.argIsInvalidType(@a, 'test'); }					\n\
		MyClass -- argIsInvalidExpression(MyClass @a, Int) { return this.argIsInvalidExpression(@a, 5 + 'test'); }	\n\
		Text -- argIsInvalidReturn(MyClass @a, Int) { return this.argIsInvalidType(@a, 5); }							\n\
		MyClass -- argIsInvalidArgumentCount(MyClass @a, Int) { return this.argIsInvalidType(@a); }					\n\
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
*/

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
	"every MyClass is: afn() { while(True) { while(True) { break; continue; } for(5; True; 5) { break; continue; }; break; continue; } }",
	PTT_VALID
)

PTT_TEST_CASE(
	InexhaustiveReturnFromIfsWithoutElses,
	"every MyClass is: Truth -- afn() { if(True) return True; if(True) return True; if(True) return True; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromIfsWithoutReturns,
	"every MyClass is: Truth -- afn() { if(True) 5; else return True; if(True) 5; else return True; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromElsesWithoutReturns,
	"every MyClass is: Truth -- afn() { if(True) return True; else 5; if(True) return True; else 5; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromReturningWithinLoopsOnly,
	"every MyClass is: Truth -- afn() { while(True) return True; for(1; True; 1) return True; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromNoReturn,
	"every MyClass is: Truth -- afn() { 5 + 5; 6 + 6; 8 + 9 + 7; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnComplicated,
	"every MyClass is:				\n\
		Truth -- afn() {			\n\
			if(True) {				\n\
				if(True) {			\n\
					5 + 5;			\n\
					6 + 6;			\n\
					return True;	\n\
				}					\n\
			} else {				\n\
				5 + 6;				\n\
				7 == 10;			\n\
				while(True) {		\n\
					return True;	\n\
				}					\n\
			}						\n\
			6 + 7;					\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	ExhaustiveReturnComplicated,
	"every MyClass is:				\n\
		Truth -- afn() {			\n\
			if(True) {				\n\
				if(True) {			\n\
					5 + 5;			\n\
					6 + 6;			\n\
					return True;	\n\
				} else {			\n\
					7 / 16;			\n\
				}					\n\
				return True;		\n\
			} else {				\n\
				5 + 6;				\n\
				7 == 10;			\n\
				return True;		\n\
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
	"every MyClass is: Int -- myfn() { :Int = 5; return Int; }",
	PTT_VALID
)

PTT_TEST_CASE(
	TestDeclareAnIntWithBadType,
	"every MyClass is: myfn() { :Int = 'test'; }",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestDeclareAnIntWithUnknownType,
	"every MyClass is: myfn() { :LInt = 'test'; }",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	TestDeclareAnIntIsScoped,
	"every MyClass is: Int -- myfn() { if(True) { :Int = 5; } return Int; }",
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
		needs ClassA{Specified};					\n\
		provides ClassA, ClassB <- ClassB(ClassA);	\n\
	",
	PTT_EXPECT(WARNING)
)

PTT_TEST_CASE(
	InjectCtorSpecializationMatchedNotWarning,
	"every ClassA is:											\n\
	every ClassB is:											\n\
		needs ClassA{Specified};								\n\
		provides												\n\
			ClassA{Specified},									\n\
			ClassB <- ClassB(ClassA{Specified});				\n\
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
		provides ClassA{Poop}, ClassA{Poop};	\n\
	",
	PTT_EXPECT(MULTIPLE_PROVISION_DEFINITION)
)

PTT_TEST_CASE(
	DuplicateProvisionWithDifferentSpecialtyIsOk,
	"every ClassA is:							\n\
		provides ClassA{Poop}, ClassA{NotPoop};	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	RetrieveFromClassValid,
	"every MyClass is: provides MyClass; MyClass -- method() { return :(MyClass <- this); }",
	PTT_VALID
)

PTT_TEST_CASE(
	RetrieveFromClassInvalid,
	"every MyClass is: MyClass -- method() { return :(MyClass <- this); }",
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
		needs Int{Port}, Text{Username};				\n\
														\n\
		provides										\n\
			Int{Port} <- 55,							\n\
			Text{Username} <- 'Bilbo',					\n\
			MyDB <- MyDB(Int{Port}, Text{Username});	\n\
														\n\
		MyDB -- cloneMe() {								\n\
			return :(MyDB <- this);						\n\
		}												\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectionsAndProvisionsRequireTransitiveProvidability,
	"every ClassA is:							\n\
		needs Int{Port};						\n\
	every ClassB is:							\n\
		needs ClassA, Text{Stuff};				\n\
	every ClassC is:							\n\
		needs ClassB, Int{ID};					\n\
	every Provider is:							\n\
		provides								\n\
			ClassB,								\n\
			ClassC,								\n\
			Int{ID},							\n\
			Text{Stuff};						\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	InjectionsAndProvisionsRequireTransitiveProvidabilityOK,
	"every ClassA is:					\n\
		needs Int{Port};				\n\
										\n\
	every ClassB is:					\n\
		needs ClassA, Text{Stuff};		\n\
										\n\
	every ClassC is:					\n\
		needs ClassB, Int{ID};			\n\
										\n\
	every Provider is:					\n\
		provides						\n\
			ClassC,						\n\
			Int{ID},					\n\
			Text{Stuff},				\n\
			Int{Port},					\n\
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
			:MyClass[] = this;			\n\
		}								\n\
		assignNumberToClassArray() {	\n\
			:MyClass[] = 4;				\n\
		}								\n\
		assignTextToClassArray() {		\n\
			:MyClass[] = 'test';		\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	DeclareEmptyArraysValid,
	"every MyClass is:								\n\
		declareEmptyClassArrayAndArrayArray() {		\n\
			:MyClass[] = [];						\n\
			:$MyClass[][] = [];						\n\
		}											\n\
		declareEmptyIntArrayAndArrayArray() {		\n\
			:Int[] = [];							\n\
			:$Int[][] = [];							\n\
		}											\n\
		declareEmptyStringArrayAndArrayArray() {	\n\
			:Text[] = [];							\n\
			:$Text[][] = [];						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AssignIndexesWrongTypes,
	"every MyClass is:										\n\
		with MyClass[] = [];								\n\
		with Int[] = [];									\n\
		with Text[] = [];									\n\
		with Truth[] = [];									\n\
		assignIntToMyClasses() { MyClass[1] = 5; }			\n\
		assignTextToMyClasses() { MyClass[1] = 'test'; }	\n\
		assignTruthToMyClasses() { MyClass[1] = True; }		\n\
		assignMyClassToInts() { Int[1] = this; }			\n\
		assignTextToInts() { Int[1] = 'test'; }				\n\
		assignTruthToInts() { Int[1] = True; }				\n\
		assignMyClassToTexts() { Text[1] = this; }			\n\
		assignIntToTexts() { Text[1] = 4; }					\n\
		assignTruthToTexts() { Text[1] = True; }			\n\
		assignMyClassToTruths() { Truth[1] = this; }		\n\
		assignIntToTruths() { Truth[1] = 4; }				\n\
		assignTextToTruths() { Truth[1] = 'test'; }			\n\
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
		with Truth[][] = [];								\n\
		assignToMyClasses() { MyClass[][] = []; }			\n\
		assignToMyClassesIndex() { MyClass[1] = []; }		\n\
		assignToInts() { Int[][] = []; }					\n\
		assignToIntsIndex() { Int[1] = []; }				\n\
		assignToTexts() { Text[][] = []; }					\n\
		assignToTextsIndex() { Text[1] = []; }				\n\
		assignToTruths() { Truth[][] = []; }				\n\
		assignToTruthsIndex() { Truth[1] = []; }			\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	InvalidEmptyArrayAssignments,
	"every MyClass is:										\n\
		with $Int = [];										\n\
		with $Text = [];									\n\
		with $MyClass = [];									\n\
		with $Truth = [];									\n\
		with Int[] = []; //valid							\n\
		with Text[] = []; //valid							\n\
		with Truth[] = []; //valid							\n\
		with MyClass[] = []; //valid						\n\
		assignToIntsIndex() { Int[1] = []; }				\n\
		assignToTextsIndex() { Text[1] = []; }				\n\
		assignToTruthsIndex() { Truth[1] = []; }			\n\
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
			:(MyClass <- this) = 5;			\n\
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
	"every MyClass is:							\n\
		myMethod(Printer?) {					\n\
			Printer.print(\"erroneous usage\");	\n\
		}",
	PTT_EXPECT(DIRECT_USE_OF_OPTIONAL_TYPE)
);

PTT_TEST_CASE(
	AssignmentOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:				\n\
		assignment(Printer?) {		\n\
			:$Printer = Printer;	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	AssignmentOfOptionalTypeToOptionalTypeIsValid,
	"every MyClass is:				\n\
		assignment(Printer?) {		\n\
			:$Printer? = Printer;	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ReturnOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:						\n\
		Printer -- returnment(Printer?) {	\n\
			return Printer;					\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	ReturnOfOptionalTypeAsOptionalTypeIsValid,
	"every MyClass is:						\n\
		Printer? -- returnment(Printer?) {	\n\
			return Printer;					\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ArgumentOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:			\n\
		optional(Printer?) {	\n\
			real(Printer);		\n\
		}						\n\
		real(Printer) {			\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	ArgumentOfOptionalTypeAsOptionalTypeIsValid,
	"every MyClass is:			\n\
		optional(Printer?) {	\n\
			real(Printer);		\n\
		}						\n\
		real(Printer?) {		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullAssignmentToRealTypeIsError,
	"every MyClass is:			\n\
		myMethod(Printer) {		\n\
			Printer = nothing;	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	NullAssignmentToOptionalTypeIsValid,
	"every MyClass is:			\n\
		myMethod(Printer?) {	\n\
			Printer = nothing;	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullReturnOfRealTypeIsError,
	"every MyClass is:			\n\
		Printer -- myMethod() { \n\
			return nothing;		\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	NullReturnOfOptionalTypeIsValid,
	"every MyClass is:				\n\
		Printer? -- myMethod() {	\n\
			return nothing;			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullArgumentOfRealArgumentIsError,
	"every MyClass is:		\n\
		real(Printer) {		\n\
			real(nothing);	\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	ExistsClauseOnRealTypeNotAllowed,
	"every MyClass is:			\n\
		myMethod(Printer) {		\n\
			Printer exists {}	\n\
		}",
	PTT_EXPECT(EXISTS_ON_NONOPTIONAL_TYPE)
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeUsable,
	"every MyClass is:						\n\
		myMethod(Printer?) {				\n\
			Printer exists {				\n\
				Printer.print(\"yay!\");	\n\
			}								\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeAssignable,
	"every MyClass is:					\n\
		myMethod(Printer?) {			\n\
			Printer exists {			\n\
				:$Printer = Printer;	\n\
			}							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeReturnable,
	"every MyClass is:					\n\
		needs $Printer;					\n\
		Printer -- myMethod(Printer?) {	\n\
			Printer exists {			\n\
				return Printer;			\n\
			}							\n\
			return $Printer;			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeValidAsArgument,
	"every MyClass is:			\n\
		optional(Printer?) {	\n\
			Printer exists {	\n\
				real(Printer);	\n\
			}					\n\
		}						\n\
		real(Printer) {			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AfterExistClauseOptionalsRemainOptional,
	"every MyClass is:					\n\
		usage(Printer?) {				\n\
			Printer exists {}			\n\
			Printer.print(\"a\");		\n\
		}								\n\
		assignment(Printer?) {			\n\
			Printer exists {}			\n\
			:$Printer = Printer;		\n\
		}								\n\
		Printer -- returnin(Printer?) {	\n\
			Printer exists {}			\n\
			return Printer;				\n\
		}								\n\
		optional(Printer?) {			\n\
			Printer exists {}			\n\
			real(Printer);				\n\
		}								\n\
		real(Printer) {					\n\
		}",
	PTT_EXPECT(DIRECT_USE_OF_OPTIONAL_TYPE)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);


BOOST_AUTO_TEST_SUITE_END()
