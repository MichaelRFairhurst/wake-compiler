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
				ClassA,											\n\
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
				ClassA,											\n\
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
			Parent -- My(Parent)Function(Child, Parent, Parent -- fn(Child) @aliased) {					\n\
																										\n\
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

BOOST_AUTO_TEST_SUITE_END()
