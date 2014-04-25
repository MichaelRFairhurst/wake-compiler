#include "boost/test/unit_test.hpp"
#include "DerivedPropertySymbolTable.h"

BOOST_AUTO_TEST_SUITE(DerivedPropertySymbolTableTest)

BOOST_AUTO_TEST_CASE(Test)
{
	TypeAnalyzer analyzer;
	vector<Type*> needs;
	const map<string, bool> parentage;
	DerivedPropertySymbolTable table(analyzer, needs, parentage);
	BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_SUITE_END()
