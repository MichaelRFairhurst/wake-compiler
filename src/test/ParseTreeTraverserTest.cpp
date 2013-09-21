#include <boost/test/unit_test.hpp>

#include "MockSemanticErrorPrinter.h"
#include "Parser.h"
#include "ParseTreeTraverser.h"

BOOST_AUTO_TEST_SUITE( ObjectSymbolTableTest )

BOOST_AUTO_TEST_CASE( MultipleClassDefinition )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: every MyClass is:");
	e.expect(MULTIPLE_CLASS_DEFINITION);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( CircularInheritance )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClassA (a MyClassB) is: every MyClassB (a MyClassA) is:");
	e.expect(CIRCULAR_INHERITANCE);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( TransitiveCircularInheritance )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClassA (a MyClassB) is: every MyClassB (a MyClassC) is: every MyClassC (a MyClassA) is:");
	e.expect(CIRCULAR_INHERITANCE);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ChildClassNotExists )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClassA (a MyClassB) is:");
	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ExtendChildClassTwice )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every Parent is: every Child (a Parent, capable Parent) is:");
	e.expect(MULTIPLE_INHERITANCE);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ExtendTwoClasses )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every ClassA is: every ClassB is: every ClassC (a ClassA, a ClassB) is:");
	e.expect(MORE_THAN_ONE_SUBCLASS);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ProvideClassNotExists )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every ClassA is: provides NonExistClass;");
	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ProvideBindToClassNotExists )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every ClassA is: provides ClassA <- NonExistClass;");
	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ProvideBindToClassConstructorNotExists )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every ClassA is: provides ClassA <- NonExistClass(ClassA);");
	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ProvideBindToClassConstructorArgumentNotExists )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every ClassA is: provides ClassA <- ClassA(NonExistClass);");
	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ProvideBindToClassConstructorArgumentTwoNotExists )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every ClassA is: provides ClassA <- ClassA(ClassA, NonExistClass);");
	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodDeclaredTwiceSimple )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every ClassA is: getName() {} getName() {}");
	e.expect(MULTIPLE_METHOD_DEFINITION);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodDeclaredTwiceComplex )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse(
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
		"
	);

	e.expect(MULTIPLE_METHOD_DEFINITION);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodWithNonExistReturn )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: NonExist -- badMethod() {}");

	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodWithNonExistArgument )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: MyClass -- badMethod(NonExist) {}");

	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodWithLambdaReturnWithInvalidReturn )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: NonExist -- fn() -- badMethod() {}");

	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodWithLambdaReturnWithInvalidArgument )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: MyClass -- fn(NonExist) -- badMethod() {}");

	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodWithLambdaArgumentWithInvalidReturn )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: badMethod( NonExist -- fn() @aliased ) {}");

	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodWithLambdaArgumentWithInvalidArgument )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: badMethod( MyClass -- fn( NonExist ) @aliased ) {}");

	e.expect(CLASSNAME_NOT_FOUND);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ValidFile )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse(
		"																								\n\
		every Parent is:																				\n\
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
		"
	);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodDefinedWithVariablesSameClassname )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: methodWithDualArgs( MyClass, MyClass ) {}");

	e.expect(SYMBOL_ALREADY_DEFINED);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodDefinedWithVariablesSameClassnameAlbeitArrayed )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: methodWithDualArgs( MyClass, MyClass[] ) {}");

	e.expect(SYMBOL_ALREADY_DEFINED);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodDefinedWithVariablesSameAlias )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: methodWithDualArgs( MyClass @a, MyClass @a ) {}");

	e.expect(SYMBOL_ALREADY_DEFINED);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodDefinedWithVariablesSameShadow )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: methodWithDualArgs( $MyClass, $MyClass ) {}");

	e.expect(SYMBOL_ALREADY_DEFINED);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodUsesNonexistAlias )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: methodWithEmptyScope() { @a; }");

	e.expect(SYMBOL_NOT_DEFINED);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodUsesNonInstanceBoundClass )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: methodWithEmptyScope() { MyClass; }");

	e.expect(SYMBOL_NOT_DEFINED);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodUsesNonInstanceBoundShadow )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: methodWithEmptyScope(MyClass) { $MyClass; }");

	e.expect(SYMBOL_NOT_DEFINED);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MethodUsesClassnameForAliasBoundClass )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: methodWithEmptyScope(MyClass @a) { MyClass; }");

	e.expect(SYMBOL_NOT_DEFINED);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( MultiplyIntsAndAnythingElseIsTypeError )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is:									\n\
		multiplyIntByString() { 5 * 'test'; }					\n\
		multiplyStringByInt() { 'test' * 5; }					\n\
		multiplyIntByObject(MyClass) { 5 * MyClass; }			\n\
		multiplyObjectByInt(MyClass) { MyClass * 5; }			\n\
		multiplyLambdaByInt(fn() @lambda) { @lambda * 5; }		\n\
	");

	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( DivideIntsAndAnythingElseIsTypeError )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is:									\n\
		divideIntByString() { 5 / 'test'; }						\n\
		divideStringByInt() { 'test' / 5; }						\n\
		divideIntByObject(MyClass) { 5 / MyClass; }				\n\
		divideObjectByInt(MyClass) { MyClass / 5; }				\n\
		divideLambdaByInt(fn() @lambda) { @lambda / 5; }		\n\
	");

	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( AddIntsAndAnythingElseIsTypeError )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is:									\n\
		addIntByString() { 5 + 'test'; }						\n\
		addStringByInt() { 'test' + 5; }						\n\
		addIntByObject(MyClass) { 5 + MyClass; }				\n\
		addObjectByInt(MyClass) { MyClass + 5; }				\n\
		addLambdaByInt(fn() @lambda) { @lambda + 5; }			\n\
	");

	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( SubtractIntsAndAnythingElseIsTypeError )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is:									\n\
		subtractIntByString() { 5 - 'test'; }					\n\
		subtractStringByInt() { 'test' - 5; }					\n\
		subtractIntByObject(MyClass) { 5 - MyClass; }			\n\
		subtractObjectByInt(MyClass) { MyClass - 5; }			\n\
		subtractLambdaByInt(fn() @lambda) { @lambda - 5; }		\n\
	");

	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ValidArithmeticAndComparisonsReturnProperTypes )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is:														\n\
		validArithmetic() {															\n\
			5 / 5 / 5;					// done thrice to ensure int returned		\n\
			5 * 5 * 5;					// done thrice to ensure int returned		\n\
			5 + 5 + 5;					// done thrice to ensure int returned		\n\
			5 - 5 - 5;					// done thrice to ensure int returned		\n\
			'test' + 'test' + 'test';	// done thrice to ensure string returned	\n\
			('test' == 'test') == True;	// test returns truth						\n\
			('test' != 'test') == True;	// test returns truth						\n\
			(5 == 5) == True;				// test returns truth						\n\
			(5 != 5) == True;				// test returns truth						\n\
			(5 < 5) == True;				// test returns truth						\n\
			(5 > 5) == True;				// test returns truth						\n\
			(5 <= 5) == True;				// test returns truth						\n\
			(5 >= 5) == True;				// test returns truth						\n\
		}																			\n\
	");

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_CASE( ArithmeticOperatorsNotValidOnStringPairss )
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is:							\n\
		subtractStrings() { 'test' - 'test'; }			\n\
		multiplyStrings() { 'test' * 'test'; }			\n\
		divideStrings() { 'test' / 'test'; }			\n\
		lessthanStrings() { 'test' < 'test'; }			\n\
		greaterthanStrings() { 'test' > 'test'; }		\n\
		lessthanequalStrings() { 'test' <= 'test'; }	\n\
		greaterthanequalStrings() { 'test' >= 'test'; }	\n\
	");

	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);
	e.expect(TYPE_ERROR);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_SUITE_END()
