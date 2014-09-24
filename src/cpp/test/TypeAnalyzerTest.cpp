/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeAnalyzerTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "boost/test/unit_test.hpp"

#include "TypeAnalyzer.h"
#include "ClassSpaceSymbolTable.h"

/**
 *  NOTE!!! This functionality used to be part of ClassSpaceSymbolTable
 * and therefore many test cases for this class exist in ClassSpaceSymbolTableTest
 */

BOOST_AUTO_TEST_SUITE(TypeAnalyzerTest);

BOOST_AUTO_TEST_CASE(TypesAreTheirOwnSubtypes) {
	TypeAnalyzer analyzer;
	BOOST_REQUIRE(analyzer.isASubtypeOfB("test", "test"));
}

BOOST_AUTO_TEST_CASE(RealTypeIsSubtypesOfOptionalType) {
	TypeAnalyzer analyzer;

	Type realtype = *MakeType(TYPE_CLASS);
	realtype.typedata._class.classname = "test";

	Type wrappedtype = *MakeType(TYPE_CLASS);
	wrappedtype.typedata._class.classname = "test";
	Type optionaltype = *MakeType(TYPE_OPTIONAL);
	optionaltype.typedata.optional.levels = 1;
	optionaltype.typedata.optional.contained = &wrappedtype;

	BOOST_REQUIRE(analyzer.isASubtypeOfB(&realtype, &optionaltype));
}

BOOST_AUTO_TEST_CASE(OptionalTypeIsNotSubtypeOfRealType) {
	TypeAnalyzer analyzer;

	Type wrappedtype = *MakeType(TYPE_CLASS);
	wrappedtype.typedata._class.classname = "test";
	Type optionaltype = *MakeType(TYPE_OPTIONAL);
	optionaltype.typedata.optional.levels = 1;
	optionaltype.typedata.optional.contained = &wrappedtype;

	Type realtype = *MakeType(TYPE_CLASS);
	realtype.typedata._class.classname = "test";

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&optionaltype, &realtype));
}

BOOST_AUTO_TEST_CASE(NothingIsSubtypesOfOptionalType) {
	TypeAnalyzer analyzer;
	Type optionaltype = *MakeType(TYPE_OPTIONAL);
	optionaltype.typedata.optional.levels = 1;
	optionaltype.typedata.optional.contained = NULL;
	Type nothing = *MakeType(TYPE_NOTHING);
	BOOST_REQUIRE(analyzer.isASubtypeOfB(&nothing, &optionaltype));
}

BOOST_AUTO_TEST_CASE(NothingIsNotSubtypeOfRealType) {
	TypeAnalyzer analyzer;
	Type nothing = *MakeType(TYPE_NOTHING);
	Type realtype = *MakeType(TYPE_CLASS);
	realtype.typedata._class.classname = "test";
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
	ClassSpaceSymbolTable table;
	analyzer.reference = &table;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	Type* asub = MakeType(TYPE_CLASS);
	Type* bsub = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	a->typedata._class.parameters = MakeTypeArray();
	b->typedata._class.parameters = MakeTypeArray();

	AddTypeToTypeArray(asub, a->typedata._class.parameters);
	AddTypeToTypeArray(bsub, b->typedata._class.parameters);

	BOOST_REQUIRE(analyzer.isASubtypeOfB(a,b));
	BOOST_REQUIRE(analyzer.isASubtypeOfB(b,a));

	free(asub->typedata._class.classname);
	free(bsub->typedata._class.classname);
	asub->typedata._class.classname = strdup("hellosuba");
	bsub->typedata._class.classname = strdup("hellosubb");

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(a,b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(b,a));

	freeType(a); freeType(b);// subs get freed automatically
}

BOOST_AUTO_TEST_CASE(ParameterizedClassTypesArentCovariantOrContravariant) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable table;
	analyzer.reference = &table;

	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);

	Type* asubwrapped = MakeType(TYPE_CLASS);
	Type* asub = MakeType(TYPE_OPTIONAL);
	Type* bsub = MakeType(TYPE_CLASS);

	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");

	asubwrapped->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	asub->typedata.optional.contained = asubwrapped; // easiest way to make asub a subtype of bsub, and bsub a supertype of asub
	a->typedata._class.parameters = MakeTypeArray();
	b->typedata._class.parameters = MakeTypeArray();

	AddTypeToTypeArray(asub, a->typedata._class.parameters);
	AddTypeToTypeArray(bsub, b->typedata._class.parameters);

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(a,b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(b,a));

	freeType(a); freeType(b);// subs get freed automatically
}

BOOST_AUTO_TEST_CASE(TwoClassesAreExactTypes) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	BOOST_REQUIRE(analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(analyzer.isAExactlyB(b, a));
	freeType(a);
	freeType(b);
}

BOOST_AUTO_TEST_CASE(MismatchedArrayTypesArentExact) {
	TypeAnalyzer analyzer;

	Type* acontained = MakeType(TYPE_CLASS);
	Type* bcontained = MakeType(TYPE_CLASS);
	acontained->typedata._class.classname = strdup("hello");
	bcontained->typedata._class.classname = strdup("hello");

	Type* a = MakeType(TYPE_LIST);
	Type* b = MakeType(TYPE_LIST);
	a->typedata.list.contained = acontained;
	b->typedata.list.contained = bcontained;
	b->typedata.list.levels = 1;
	a->typedata.list.levels = 2;

	BOOST_REQUIRE(!analyzer.isAExactlyB(acontained, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, acontained));
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(MismatchedOptionalTypesArentExact) {
	TypeAnalyzer analyzer;
	Type* awrapped = MakeType(TYPE_CLASS);
	awrapped->typedata._class.classname = strdup("hello");

	Type* bwrapped = MakeType(TYPE_CLASS);
	bwrapped->typedata._class.classname = strdup("hello");

	Type* a = MakeType(TYPE_OPTIONAL);
	a->typedata.optional.levels = 2;
	a->typedata.optional.contained = awrapped;

	Type* b = MakeType(TYPE_OPTIONAL);
	b->typedata.optional.levels = 1;
	b->typedata.optional.contained = bwrapped;

	BOOST_REQUIRE(!analyzer.isAExactlyB(awrapped, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, awrapped));
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(MismatchedNamesArentExact) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(MismatchedTypesArentExact) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_PARAMETERIZED);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(OnlyOneParameterizedArentExact) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	a->typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(MakeType(TYPE_MATCHALL), a->typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(MismatchedParameterizationLengthsArentExact) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	a->typedata._class.parameters = MakeTypeArray();
	b->typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(MakeType(TYPE_MATCHALL), a->typedata._class.parameters);
	AddTypeToTypeArray(MakeType(TYPE_MATCHALL), a->typedata._class.parameters);
	AddTypeToTypeArray(MakeType(TYPE_MATCHALL), b->typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(MismatchedParameterizationsArentExact) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	Type* asub = MakeType(TYPE_CLASS);
	Type* bsub = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosuba");
	bsub->typedata._class.classname = strdup("hellosubb");
	a->typedata._class.parameters = MakeTypeArray();
	b->typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(asub, a->typedata._class.parameters);
	AddTypeToTypeArray(bsub, b->typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(ExactParameterizationsArentExact) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_CLASS);
	Type* b = MakeType(TYPE_CLASS);
	Type* asub = MakeType(TYPE_CLASS);
	Type* bsub = MakeType(TYPE_CLASS);
	a->typedata._class.classname = strdup("hello");
	b->typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	a->typedata._class.parameters = MakeTypeArray();
	b->typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(asub, a->typedata._class.parameters);
	AddTypeToTypeArray(bsub, b->typedata._class.parameters);
	BOOST_REQUIRE(analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(ParameterizedTypesAreExactWithExactLabel) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_PARAMETERIZED);
	Type* b = MakeType(TYPE_PARAMETERIZED);
	a->typedata.parameterized.label = strdup("hello");
	b->typedata.parameterized.label = strdup("hello");
	BOOST_REQUIRE(analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(ParameterizedTypesAreNotExactWithDifferentLabel) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_PARAMETERIZED);
	Type* b = MakeType(TYPE_PARAMETERIZED);
	a->typedata.parameterized.label = strdup("hello");
	b->typedata.parameterized.label = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithVoidVsNonVoid) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_LAMBDA);
	Type* b = MakeType(TYPE_LAMBDA);
	a->typedata.lambda.returntype = MakeType(TYPE_MATCHALL);
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentReturnType) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_LAMBDA);
	Type* b = MakeType(TYPE_LAMBDA);
	a->typedata.lambda.returntype = MakeType(TYPE_CLASS);
	b->typedata.lambda.returntype = MakeType(TYPE_CLASS);
	a->typedata.lambda.returntype->typedata._class.classname = strdup("hello");
	b->typedata.lambda.returntype->typedata._class.classname = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentArgumentCounts) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_LAMBDA);
	Type* b = MakeType(TYPE_LAMBDA);
	a->typedata.lambda.arguments = MakeTypeArray();
	b->typedata.lambda.arguments = MakeTypeArray();
	AddTypeToTypeArray(MakeType(TYPE_MATCHALL), a->typedata.lambda.arguments);
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithNullNonNullArguments) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_LAMBDA);
	Type* b = MakeType(TYPE_LAMBDA);
	a->typedata.lambda.arguments = MakeTypeArray();
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentArguments) {
	TypeAnalyzer analyzer;
	Type* a = MakeType(TYPE_LAMBDA);
	Type* b = MakeType(TYPE_LAMBDA);
	Type* aarg = MakeType(TYPE_CLASS);
	aarg->typedata._class.classname = strdup("holl");
	Type* barg = MakeType(TYPE_CLASS);
	barg->typedata._class.classname = strdup("holle");
	a->typedata.lambda.arguments = MakeTypeArray();
	AddTypeToTypeArray(aarg, a->typedata.lambda.arguments);
	b->typedata.lambda.arguments = MakeTypeArray();
	AddTypeToTypeArray(barg, b->typedata.lambda.arguments);
	BOOST_REQUIRE(!analyzer.isAExactlyB(a, b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(b, a));
	freeType(a); freeType(b);
}

BOOST_AUTO_TEST_SUITE_END();
