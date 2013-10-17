#include "ScopeSymbolTable.h"
#include "SemanticError.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( ScopeSymbolTableTest )

BOOST_AUTO_TEST_CASE( TestSymbolTableAddsAndThenGets ) {
	ScopeSymbolTable table;
	Type* type = new Type;
	table.add("@bilbo", type);

	BOOST_REQUIRE(table.find("@bilbo") == type);

	delete type;
}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInLastStackWillGet ) {
	ScopeSymbolTable table;
	Type* type = new Type;
	table.add("@bilbo", type);
	table.pushScope();

	BOOST_REQUIRE(table.find("@bilbo") == type);

	delete type;
}

BOOST_AUTO_TEST_CASE( TestSymbolTableNotAddThrowsSemanticError ) {
	ScopeSymbolTable table;
	Type* type = new Type;

	try {
		table.find("@bilbo");
		BOOST_CHECK_MESSAGE(false, "No exception thrown");
	} catch(SemanticError* e) {
		BOOST_CHECK(e->code == SYMBOL_NOT_DEFINED);
		delete e;
	}

	delete type;
}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddTwiceThrowsSemanticError ) {
	ScopeSymbolTable table;
	Type* type = new Type;

	table.add("@bilbo", type);
	try {
		table.add("@bilbo", type);
		BOOST_CHECK_MESSAGE(false, "No exception thrown");
	} catch(SemanticError* e) {
		BOOST_CHECK(e->code == SYMBOL_ALREADY_DEFINED);
		delete e;
	}

	delete type;
}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInNewScopeStillThrowsSemanticError ) {
	ScopeSymbolTable table;
	Type* type = new Type;

	table.add("@bilbo", type);
	table.pushScope();
	try {
		table.add("@bilbo", type);
		BOOST_CHECK_MESSAGE(false, "No exception thrown");
	} catch(SemanticError* e) {
		BOOST_CHECK(e->code == SYMBOL_ALREADY_DEFINED);
		delete e;
	}

	delete type;
}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInNewScopeNotSetOncePopped ) {
	ScopeSymbolTable table;
	Type* type = new Type;

	table.pushScope();
	table.add("@bilbo", type);
	table.popScope();
	try {
		table.find("@bilbo");
		BOOST_CHECK_MESSAGE(false, "No exception thrown");
	} catch(SemanticError* e) {
		BOOST_CHECK(e->code == SYMBOL_NOT_DEFINED);
		delete e;
	}

	delete type;
}

BOOST_AUTO_TEST_CASE( TestPushPopEmptyScopeGoesOK ) {
	ScopeSymbolTable table;

	table.pushScope();
	table.popScope();
}

BOOST_AUTO_TEST_CASE( TestSymbolsAddedByType ) {
	ScopeSymbolTable table;
	Type* simple = new Type;
	simple->typedata._class.shadow = 0;
	simple->alias = NULL;
	simple->typedata._class.classname = "MyClass";
	Type* aliased = new Type;
	aliased->alias = "MyClass";
	Type* shadowed = new Type;
	shadowed->alias = NULL;
	shadowed->typedata._class.classname = "MyClass";
	shadowed->typedata._class.shadow = 3;

	table.add(simple);
	table.add(aliased);
	table.add(shadowed);

	BOOST_CHECK(simple == table.find("MyClass"));
	BOOST_CHECK(aliased == table.find("@MyClass"));
	BOOST_CHECK(shadowed == table.find("$$$MyClass"));

	delete simple;
	delete aliased;
	delete shadowed;
}

BOOST_AUTO_TEST_CASE( TestGetSymbolByType ) {
	ScopeSymbolTable table;
	Type* simplep = new Type;
	Type* shadowedp = new Type;

	table.add("MyClass", simplep);
	table.add("$$$MyClass", shadowedp);

	Type* simple = new Type;
	simple->alias = NULL;
	simple->typedata._class.shadow = 0;
	simple->typedata._class.classname = "MyClass";
	Type* shadowed = new Type;
	shadowed->alias = NULL;
	shadowed->typedata._class.classname = "MyClass";
	shadowed->typedata._class.shadow = 3;

	BOOST_CHECK(simplep == table.find(simple));
	BOOST_CHECK(shadowedp == table.find(shadowed));

	delete simple;
	delete shadowed;
	delete simplep;
	delete shadowedp;
}


BOOST_AUTO_TEST_SUITE_END()
