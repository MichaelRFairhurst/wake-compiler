/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ScopeSymbolTableTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ScopeSymbolTable.h"
#include "SemanticError.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( ScopeSymbolTableTest )

BOOST_AUTO_TEST_CASE( TestSymbolTableAddsAndThenGets ) {
	ScopeSymbolTable table;
	Type type;
	type.type = TYPE_CLASS;
	table.add("bilbo", &type);

	BOOST_REQUIRE(table.find("bilbo") == &type);
}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInLastStackWillGet ) {
	ScopeSymbolTable table;
	Type type;
	type.type = TYPE_CLASS;
	table.add("bilbo", &type);
	table.pushScope();

	BOOST_REQUIRE(table.find("bilbo") == &type);

}

BOOST_AUTO_TEST_CASE( TestSymbolTableNotAddedGetsEmptyOptional) {
	ScopeSymbolTable table;
	boost::optional<Type*> type;

	type = table.find("bilbo");
	BOOST_CHECK(!type);
}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddTwiceThrowsSemanticError ) {
	ScopeSymbolTable table;
	Type type;
	type.type = TYPE_CLASS;

	table.add("bilbo", &type);
	try {
		table.add("bilbo", &type);
		BOOST_CHECK_MESSAGE(false, "No exception thrown");
	} catch(SemanticError* e) {
		BOOST_CHECK(e->code == SYMBOL_ALREADY_DEFINED);
		delete e;
	}

}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInNewScopeStillThrowsSemanticError ) {
	ScopeSymbolTable table;
	Type type;
	type.type = TYPE_CLASS;

	table.add("bilbo", &type);
	table.pushScope();
	try {
		table.add("bilbo", &type);
		BOOST_CHECK_MESSAGE(false, "No exception thrown");
	} catch(SemanticError* e) {
		BOOST_CHECK(e->code == SYMBOL_ALREADY_DEFINED);
		delete e;
	}

}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInNewScopeNotSetOncePopped ) {
	ScopeSymbolTable table;
	boost::optional<Type*> optionaltype;
	Type type;
	type.type = TYPE_CLASS;

	table.pushScope();
	table.add("bilbo", &type);
	table.popScope();
	optionaltype = table.find("bilbo");
	BOOST_CHECK(!optionaltype);
}

BOOST_AUTO_TEST_CASE( TestPushPopEmptyScopeGoesOK ) {
	ScopeSymbolTable table;

	table.pushScope();
	table.popScope();
}

BOOST_AUTO_TEST_CASE( TestSymbolsAddedByType ) {
	ScopeSymbolTable table;
	Type simple;
	simple.type = TYPE_CLASS;
	simple.typedata._class.shadow = 0;
	simple.alias = NULL;
	simple.typedata._class.classname = "MyClass";
	Type aliased;
	aliased.type = TYPE_CLASS;
	aliased.alias = "myClass";
	Type shadowed;
	shadowed.type = TYPE_CLASS;
	shadowed.alias = NULL;
	shadowed.typedata._class.classname = "MyClass";
	shadowed.typedata._class.shadow = 3;

	table.add(&simple);
	table.add(&aliased);
	table.add(&shadowed);

	BOOST_CHECK(&simple == table.find("MyClass"));
	BOOST_CHECK(&aliased == table.find("myClass"));
	BOOST_CHECK(&shadowed == table.find("$$$MyClass"));
}

BOOST_AUTO_TEST_CASE( TestGetSymbolByType ) {
	ScopeSymbolTable table;
	Type simplep;
	simplep.type = TYPE_CLASS;
	Type shadowedp;
	shadowedp.type = TYPE_CLASS;

	table.add("MyClass", &simplep);
	table.add("$$$MyClass", &shadowedp);

	Type simple;
	simple.type = TYPE_CLASS;
	simple.alias = NULL;
	simple.typedata._class.shadow = 0;
	simple.typedata._class.classname = "MyClass";
	Type shadowed;
	shadowed.type = TYPE_CLASS;
	shadowed.alias = NULL;
	shadowed.typedata._class.classname = "MyClass";
	shadowed.typedata._class.shadow = 3;

	BOOST_CHECK(&simplep == table.find(&simple));
	BOOST_CHECK(&shadowedp == table.find(&shadowed));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE( TestArrayedSymbolsAddedByType ) {
	ScopeSymbolTable table;
	Type onearray;
	Type onecontained;
	onecontained.type = TYPE_CLASS;
	onecontained.typedata._class.shadow = 0;
	onecontained.typedata._class.classname = "One";
	onecontained.alias = NULL;
	onearray.type = TYPE_LIST;
	onearray.alias = NULL;
	onearray.typedata.list.levels = 1;
	onearray.typedata.list.contained = &onecontained;
	Type twoarray;
	Type twocontained;
	twocontained.type = TYPE_CLASS;
	twocontained.typedata._class.classname = "Two";
	twocontained.typedata._class.shadow = 0;
	twocontained.alias = NULL;
	twoarray.type = TYPE_LIST;
	twoarray.alias = NULL;
	twoarray.typedata.list.levels = 2;
	twoarray.typedata.list.contained = &twocontained;

	table.add(&onearray);
	table.add(&twoarray);

	BOOST_CHECK(&onearray == table.find("One[]"));
	BOOST_CHECK(&twoarray == table.find("Two[]"));
}
