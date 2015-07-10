/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * integration.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <boost/test/unit_test.hpp>

#include "MockSemanticErrorPrinter.h"
#include "Parser.h"
#include "ParseTreeTraverser.h"
#include "LibraryLoader.h"
#include "ErrorTracker.h"

BOOST_AUTO_TEST_SUITE( ParseTreeTraverserTest )

#define PTT_PRINT_TREE 0

/**
 * These macros lead the charge in the test design
 */
#define PTT_TEST_CASE( NAME, CODE, EXPECTATIONS ) \
	BOOST_AUTO_TEST_CASE( NAME ) \
	{ \
		Parser p; \
		ErrorTracker errors; \
		ClassSpaceSymbolTable table; \
		table.setModule("lang"); \
		LibraryLoader loader; \
		loader.loadStdLibToTable(&table); \
		ParseTreeTraverser t(&table, errors); \
		MockSemanticErrorPrinter e; \
		p.parse("module lang;\n" CODE  "\nevery Num is: every Int is: every Bool is: every Char is: every Text is: every List{T} is: Int -- getSize() { return 0; } every Exception is: Text[] -- getStackTrace() { var Text[] = []; return Text[]; }"); \
		if(PTT_PRINT_TREE) p.print(); \
		t.classGatheringPass(p.getParseTree()); \
		t.methodGatheringPass(p.getParseTree()); \
		t.finalPass(p.getParseTree()); \
		{ EXPECTATIONS } \
		t.printErrors(e); \
		BOOST_REQUIRE(e.passed()); \
	}

#define PTT_VALID ;

#define PTT_EXPECT( ERROR ) e.expect(ERROR);

#include "../src/cpp/test/integration/Arithmetic.cpp"
#include "../src/cpp/test/integration/Lists.cpp"
#include "../src/cpp/test/integration/Booleans.cpp"
#include "../src/cpp/test/integration/Bitwise.cpp"
#include "../src/cpp/test/integration/Lambdas.cpp"
#include "../src/cpp/test/integration/Assignments.cpp"
#include "../src/cpp/test/integration/MethodReturns.cpp"
#include "../src/cpp/test/integration/ControlFlow.cpp"
#include "../src/cpp/test/integration/Constructors.cpp"
#include "../src/cpp/test/integration/Properties.cpp"
#include "../src/cpp/test/integration/ClassCast.cpp"
#include "../src/cpp/test/integration/VarDeclarations.cpp"
#include "../src/cpp/test/integration/Optionals.cpp"
#include "../src/cpp/test/integration/MethodInvocations.cpp"
#include "../src/cpp/test/integration/AbstractClasses.cpp"
#include "../src/cpp/test/integration/GenericClasses.cpp"
#include "../src/cpp/test/integration/GenericMethods.cpp"
#include "../src/cpp/test/integration/ClassDefinitions.cpp"
#include "../src/cpp/test/integration/Provisions.cpp"
#include "../src/cpp/test/integration/MethodDeclarations.cpp"
#include "../src/cpp/test/integration/Annotations.cpp"
#include "../src/cpp/test/integration/Exceptions.cpp"
#include "../src/cpp/test/integration/EarlyBailoutOperators.cpp"

BOOST_AUTO_TEST_SUITE_END()
