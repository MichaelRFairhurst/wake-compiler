#include "boost/test/unit_test.hpp"

#include "TypeAnalyzer.h"

/**
 *  NOTE!!! This functionality used to be part of ObjectSymbolTable
 * and therefore many test cases for this class exist in ObjectSymbolTableTest
 */

BOOST_AUTO_TEST_SUITE(TypeAnalyzerTest);

BOOST_AUTO_TEST_CASE(TypesAreTheirOwnSubtypes) {
	TypeAnalyzer analyzer;
	BOOST_REQUIRE(analyzer.isASubtypeOfB("test", "test"));
}

BOOST_AUTO_TEST_SUITE_END();
