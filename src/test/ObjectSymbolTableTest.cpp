#include <boost/test/unit_test.hpp>

#include "MockSemanticErrorPrinter.h"
#include "Parser.h"
#include "ParseTreeTraverser.h"

BOOST_AUTO_TEST_SUITE( ObjectSymbolTableTest )

BOOST_AUTO_TEST_CASE(TestThisFileRuns)
{
	Parser p;
	ParseTreeTraverser t;
	MockSemanticErrorPrinter e;

	p.parse("every MyClass is: every MyClass is:");
	e.expect(MULTIPLE_CLASS_DEFINITION);
	e.expect(MULTIPLE_CLASS_DEFINITION);

	t.traverse(p.getParseTree());
	t.printErrors(e);

	BOOST_REQUIRE(e.passed());
}

BOOST_AUTO_TEST_SUITE_END()
