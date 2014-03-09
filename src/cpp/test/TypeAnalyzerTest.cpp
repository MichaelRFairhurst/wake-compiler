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

BOOST_AUTO_TEST_CASE(RealTypeIsSubtypesOfOptionalType) {
	TypeAnalyzer analyzer;
	Type optionaltype = *MakeType(TYPE_CLASS);
	optionaltype.typedata._class.classname = "test";
	optionaltype.optional = true;
	Type realtype = *MakeType(TYPE_CLASS);
	realtype.typedata._class.classname = "test";
	realtype.optional = false;
	BOOST_REQUIRE(analyzer.isASubtypeOfB(&realtype, &optionaltype));
}

BOOST_AUTO_TEST_CASE(OptionalTypeIsNotSubtypeOfRealType) {
	TypeAnalyzer analyzer;
	Type optionaltype = *MakeType(TYPE_CLASS);
	optionaltype.typedata._class.classname = "test";
	optionaltype.optional = true;
	Type realtype = *MakeType(TYPE_CLASS);
	realtype.typedata._class.classname = "test";
	realtype.optional = false;
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&optionaltype, &realtype));
}

BOOST_AUTO_TEST_CASE(NothingIsSubtypesOfOptionalType) {
	TypeAnalyzer analyzer;
	Type optionaltype = *MakeType(TYPE_CLASS);
	optionaltype.typedata._class.classname = "test";
	optionaltype.optional = true;
	Type nothing = *MakeType(TYPE_NOTHING);
	BOOST_REQUIRE(analyzer.isASubtypeOfB(&nothing, &optionaltype));
}

BOOST_AUTO_TEST_CASE(NothingIsNotSubtypeOfRealType) {
	TypeAnalyzer analyzer;
	Type nothing = *MakeType(TYPE_NOTHING);
	Type realtype = *MakeType(TYPE_CLASS);
	realtype.typedata._class.classname = "test";
	realtype.optional = false;
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&nothing, &realtype));
}

BOOST_AUTO_TEST_CASE(MatchallNameIsErroneousType) {
	TypeAnalyzer analyzer;
	Type matchall = *MakeType(TYPE_MATCHALL);
	BOOST_REQUIRE(analyzer.getNameForType(&matchall) == "{inferencing failed}");
}

BOOST_AUTO_TEST_SUITE_END();
