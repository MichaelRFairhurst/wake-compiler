#include "boost/test/unit_test.hpp"
#include "DerivedPropertySymbolTable.h"

BOOST_AUTO_TEST_SUITE(DerivedPropertySymbolTableTest)

BOOST_AUTO_TEST_CASE(Test)
{
	TypeAnalyzer analyzer;
	vector<Type*> needs;
	map<string, ObjectProperty*> properties;
	ObjectProperty* prop = new ObjectProperty();
	prop->type = MakeType(TYPE_LAMBDA);
	properties["hello"] = prop;
	map<string, bool> parentage;
	DerivedPropertySymbolTable table(analyzer, needs, properties, parentage);

	boost::optional<Type*> method = table.find("hello");
	BOOST_REQUIRE(method);
	BOOST_REQUIRE(*method == prop->type);
}

BOOST_AUTO_TEST_SUITE_END()
