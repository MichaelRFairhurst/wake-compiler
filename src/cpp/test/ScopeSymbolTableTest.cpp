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
	VarDecl decl;
	decl.typedata = PureType(TYPE_CLASS);
	decl.alias = strdup("bilbo");
	table.add(&decl);

	BOOST_REQUIRE(table.find("bilbo") == &decl.typedata);
}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInLastStackWillGet ) {
	ScopeSymbolTable table;
	VarDecl decl;
	decl.alias = strdup("bilbo");
	table.add(&decl);
	table.pushScope();

	BOOST_REQUIRE(table.find("bilbo") == &decl.typedata);

}

BOOST_AUTO_TEST_CASE( TestSymbolTableNotAddedGetsEmptyOptional) {
	ScopeSymbolTable table;
	boost::optional<PureType*> type;

	type = table.find("bilbo");
	BOOST_CHECK(!type);
}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddTwiceThrowsSemanticError ) {
	ScopeSymbolTable table;
	VarDecl decl;
	decl.typedata = PureType(TYPE_CLASS);
	decl.alias = strdup("bilbo");

	table.add(&decl);
	try {
		table.add(&decl);
		BOOST_CHECK_MESSAGE(false, "No exception thrown");
	} catch(SemanticError* e) {
		BOOST_CHECK(e->code == SYMBOL_ALREADY_DEFINED);
		delete e;
	}

}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInNewScopeStillThrowsSemanticError ) {
	ScopeSymbolTable table;
	VarDecl decl;
	decl.typedata = PureType(TYPE_CLASS);
	decl.alias = strdup("bilbo");

	table.add(&decl);
	table.pushScope();
	try {
		table.add(&decl);
		BOOST_CHECK_MESSAGE(false, "No exception thrown");
	} catch(SemanticError* e) {
		BOOST_CHECK(e->code == SYMBOL_ALREADY_DEFINED);
		delete e;
	}

}

BOOST_AUTO_TEST_CASE( TestSymbolTableAddInNewScopeNotSetOncePopped ) {
	ScopeSymbolTable table;
	boost::optional<PureType*> optionaltype;
	VarDecl decl;
	decl.alias = strdup("bilbo");
	decl.typedata = PureType(TYPE_CLASS);

	table.pushScope();
	table.add(&decl);
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
	VarDecl simple;
	simple.typedata = PureType(TYPE_CLASS);
	simple.shadow = 0;
	simple.typedata.typedata._class.classname = strdup("MyClass");
	VarDecl aliased;
	aliased.typedata = PureType(TYPE_CLASS);
	aliased.alias = strdup("myClass");
	VarDecl shadowed;
	shadowed.typedata = PureType(TYPE_CLASS);
	//shadowed.alias = NULL;
	shadowed.typedata.typedata._class.classname = strdup("MyClass");
	shadowed.shadow = 3;

	table.add(&simple);
	table.add(&aliased);
	table.add(&shadowed);

	BOOST_CHECK(&simple.typedata == table.find("MyClass"));
	BOOST_CHECK(&aliased.typedata == table.find("myClass"));
	BOOST_CHECK(&shadowed.typedata == table.find("$$$MyClass"));
}

BOOST_AUTO_TEST_CASE( TestGetSymbolByType ) {
	ScopeSymbolTable table;
	PureType simplep(TYPE_CLASS);
	PureType shadowedp(TYPE_CLASS);

	table.add("MyClass", &simplep);
	table.add("$$$MyClass", &shadowedp);

	VarRef simple(new ClassVarRef(strdup("MyClass"), 0, 0));
	VarRef shadowed(new ClassVarRef(strdup("MyClass"), 3, 0));

	BOOST_CHECK(&simplep == table.find(&simple));
	BOOST_CHECK(&shadowedp == table.find(&shadowed));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE( TestArrayedSymbolsAddedByType ) {
	ScopeSymbolTable table;
	PureType onearray(TYPE_LIST);
	PureType* onecontained = new PureType(TYPE_CLASS);
	onecontained->typedata._class.classname = strdup("One");
	//onecontained->alias = NULL;
	onearray.type = TYPE_LIST;
	onearray.typedata.list.contained = onecontained;
	PureType twoarray(TYPE_LIST);
	PureType* twoarrayinner = new PureType(TYPE_LIST);
	PureType* twocontained = new PureType(TYPE_CLASS);
	twocontained->typedata._class.classname = strdup("Two");
	twoarrayinner->typedata.list.contained = twocontained;
	twoarray.typedata.list.contained = twoarrayinner;

	VarDecl onearraydecl;
	VarDecl twoarraydecl;
	onearraydecl.typedata = onearray;
	twoarraydecl.typedata = twoarray;
	table.add(&onearraydecl);
	table.add(&twoarraydecl);

	BOOST_CHECK(&onearraydecl.typedata == table.find("One[]"));
	BOOST_CHECK(&twoarraydecl.typedata == table.find("Two[]"));
}
