#include <boost/test/unit_test.hpp>

#include "PropertySymbolTable.h"

BOOST_AUTO_TEST_SUITE(PropertySymbolTableTestSuite);

BOOST_AUTO_TEST_CASE(AddingNeedsAreGotten) {
	PropertySymbolTable table;
	Type* thefirsttype = MakeType(TYPE_CLASS);
	Type* thesecondtype = MakeType(TYPE_CLASS);
	table.addNeed(thefirsttype);
	table.addNeed(thesecondtype);
	BOOST_REQUIRE(table.getNeeds()->at(0) == thefirsttype);
	BOOST_REQUIRE(table.getNeeds()->at(1) == thesecondtype);
}

BOOST_AUTO_TEST_SUITE_END();
