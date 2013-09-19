#include <boost/test/unit_test.hpp>

#include "ObjectSymbolTable.h"
#include "SemanticError.h"

BOOST_AUTO_TEST_SUITE( ObjectSymbolTableTest )

BOOST_AUTO_TEST_CASE( TestClassesAreOwnSubclasses )
{
	ObjectSymbolTable table;

	BOOST_CHECK(table.isASubtypeOfB("MyClass", "MyClass"));
}

BOOST_AUTO_TEST_CASE( TestClassesAreChildsSubclasses )
{
	ObjectSymbolTable table;

	table.addClass("ParentClass");
	table.addClass("ChildClass");
	table.addInheritance("ParentClass", 1);

	BOOST_REQUIRE(table.isASubtypeOfB("ChildClass", "ParentClass"));
}

BOOST_AUTO_TEST_CASE( TestClassesArentSubclasses )
{
	ObjectSymbolTable table;

	BOOST_REQUIRE(!table.isASubtypeOfB("ClassA", "ClassB"));
}

BOOST_AUTO_TEST_CASE( TestParentClassesArentSubclasses )
{
	ObjectSymbolTable table;

	table.addClass("ParentClass");
	table.addClass("ChildClass");
	table.addInheritance("ParentClass", 1);

	BOOST_REQUIRE(!table.isASubtypeOfB("ParentClass", "ChildClass"));
}

BOOST_AUTO_TEST_CASE( TestTransitiveSubclassesAreSubClasses )
{
	ObjectSymbolTable table;

	table.addClass("ParentClass");
	table.addClass("ChildClass");
	table.addInheritance("ParentClass", 1);
	table.addClass("ChildChildClass");
	table.addInheritance("ChildClass", 1);

	BOOST_REQUIRE(table.isASubtypeOfB("ChildChildClass", "ParentClass"));
}

BOOST_AUTO_TEST_SUITE_END()
