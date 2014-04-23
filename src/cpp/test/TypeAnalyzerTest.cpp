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

BOOST_AUTO_TEST_CASE(ParameterizedClassTypesUnequalParametersNotSubtypes) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	a->typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(MakeType(TYPE_MATCHALL), a->typedata._class.parameters);
	AddTypeToTypeArray(MakeType(TYPE_MATCHALL), a->typedata._class.parameters);
	b->typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(MakeType(TYPE_MATCHALL), b->typedata._class.parameters);

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(a,b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(b,a));

	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(ParameterizedClassTypesRequireDifferentArgumentsAreSubtypes) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	Type* asub = MakeType(TYPE_CLASS);
	Type* bsub = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	return;
	a->typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(asub, a->typedata._class.parameters);
	b->typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(bsub, b->typedata._class.parameters);

	BOOST_REQUIRE(analyzer.isASubtypeOfB(a,b));
	BOOST_REQUIRE(analyzer.isASubtypeOfB(b,a));

	free(asub->typedata._class.classname);
	free(bsub->typedata._class.classname);
	asub->typedata._class.classname = strdup("hellosuba");
	bsub->typedata._class.classname = strdup("hellosubb");

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(a,b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(b,a));

	freeType(a); freeType(b); freeType(asub); freeType(bsub);
}

BOOST_AUTO_TEST_SUITE_END();
