/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ClassSpaceSymbolTableTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <boost/test/unit_test.hpp>

#include "ClassSpaceSymbolTable.h"
#include "SemanticError.h"

BOOST_AUTO_TEST_SUITE( ClassSpaceSymbolTableTest )

BOOST_AUTO_TEST_CASE( TestClassesAreOwnSubclasses )
{
	ClassSpaceSymbolTable table;

	BOOST_CHECK(table.getAnalyzer()->isASubtypeOfB("MyClass", "MyClass"));
}

BOOST_AUTO_TEST_CASE( TestClassesAreChildsSubclasses )
{
	ClassSpaceSymbolTable table;

	table.addClass("ParentClass");
	table.addClass("ChildClass");
	table.addInheritance("ParentClass", 1);

	BOOST_REQUIRE(table.getAnalyzer()->isASubtypeOfB("ChildClass", "ParentClass"));
}

BOOST_AUTO_TEST_CASE( TestClassesArentSubclasses )
{
	ClassSpaceSymbolTable table;

	BOOST_REQUIRE(!table.getAnalyzer()->isASubtypeOfB("ClassA", "ClassB"));
}

BOOST_AUTO_TEST_CASE( TestParentClassesArentSubclasses )
{
	ClassSpaceSymbolTable table;

	table.addClass("ParentClass");
	table.addClass("ChildClass");
	table.addInheritance("ParentClass", 1);

	BOOST_REQUIRE(!table.getAnalyzer()->isASubtypeOfB("ParentClass", "ChildClass"));
}

BOOST_AUTO_TEST_CASE( TestTransitiveSubclassesAreSubClasses )
{
	ClassSpaceSymbolTable table;

	table.addClass("ParentClass");
	table.addClass("ChildClass");
	table.addInheritance("ParentClass", 1);
	table.addClass("ChildChildClass");
	table.addInheritance("ChildClass", 1);

	BOOST_REQUIRE(table.getAnalyzer()->isASubtypeOfB("ChildChildClass", "ParentClass"));
}

BOOST_AUTO_TEST_SUITE_END()
