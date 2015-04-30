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

	PureType realtype(TYPE_CLASS);
	realtype.typedata._class.classname = strdup("test");

	PureType* wrappedtype = new PureType(TYPE_CLASS);
	wrappedtype->typedata._class.classname = strdup("test");
	PureType optionaltype(TYPE_OPTIONAL);
	optionaltype.typedata.optional.contained = wrappedtype;

	BOOST_REQUIRE(analyzer.isASubtypeOfB(&realtype, &optionaltype));
}

BOOST_AUTO_TEST_CASE(OptionalTypeIsNotSubtypeOfRealType) {
	TypeAnalyzer analyzer;

	PureType* wrappedtype = new PureType(TYPE_CLASS);
	wrappedtype->typedata._class.classname = strdup("test");
	PureType optionaltype(TYPE_OPTIONAL);
	optionaltype.typedata.optional.contained = wrappedtype;

	PureType realtype(TYPE_CLASS);
	realtype.typedata._class.classname = strdup("test");

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&optionaltype, &realtype));
}

BOOST_AUTO_TEST_CASE(NothingIsSubtypesOfOptionalType) {
	TypeAnalyzer analyzer;
	PureType optionaltype(TYPE_OPTIONAL);
	optionaltype.typedata.optional.contained = new PureType(TYPE_CLASS);
	PureType nothing(TYPE_OPTIONAL);
	nothing.typedata.optional.contained = new PureType(TYPE_MATCHALL);
	BOOST_REQUIRE(analyzer.isASubtypeOfB(&nothing, &optionaltype));
}

BOOST_AUTO_TEST_CASE(NothingIsNotSubtypeOfRealType) {
	TypeAnalyzer analyzer;
	PureType nothing(TYPE_OPTIONAL);
	nothing.typedata.optional.contained = new PureType(TYPE_MATCHALL);
	PureType realtype(TYPE_CLASS);
	realtype.typedata._class.classname = strdup("test");
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&nothing, &realtype));
}

BOOST_AUTO_TEST_CASE(MatchallNameIsErroneousType) {
	PureType matchall(TYPE_MATCHALL);
	BOOST_REQUIRE(matchall.toString() == "{inferencing failed}");
}

BOOST_AUTO_TEST_CASE(ParameterizedClassTypesUnequalParametersNotSubtypes) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	a.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), a.typedata._class.parameters);
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), a.typedata._class.parameters);
	b.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), b.typedata._class.parameters);

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&a,&b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&b,&a));
}

BOOST_AUTO_TEST_CASE(ParameterizedClassTypesRequireDifferentArgumentsAreSubtypes) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable table;
	analyzer.reference = &table;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);
	PureType* asub = new PureType(TYPE_CLASS);
	PureType* bsub = new PureType(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	a.typedata._class.parameters = new PureTypeArray();
	b.typedata._class.parameters = new PureTypeArray();

	addPureTypeToPureTypeArray(asub, a.typedata._class.parameters);
	addPureTypeToPureTypeArray(bsub, b.typedata._class.parameters);

	BOOST_REQUIRE(analyzer.isASubtypeOfB(&a,&b));
	BOOST_REQUIRE(analyzer.isASubtypeOfB(&b,&a));

	free(asub->typedata._class.classname);
	free(bsub->typedata._class.classname);
	asub->typedata._class.classname = strdup("hellosuba");
	bsub->typedata._class.classname = strdup("hellosubb");

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&a,&b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&b,&a));
}

BOOST_AUTO_TEST_CASE(ParameterizedClassTypesArentCovariantOrContravariant) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable table;
	analyzer.reference = &table;

	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);

	PureType* asubwrapped = new PureType(TYPE_CLASS);
	PureType* asub = new PureType(TYPE_OPTIONAL);
	PureType* bsub = new PureType(TYPE_CLASS);

	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");

	asubwrapped->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	asub->typedata.optional.contained = asubwrapped; // easiest way to make asub a subtype of bsub, and bsub a supertype of asub
	a.typedata._class.parameters = new PureTypeArray();
	b.typedata._class.parameters = new PureTypeArray();

	addPureTypeToPureTypeArray(asub, a.typedata._class.parameters);
	addPureTypeToPureTypeArray(bsub, b.typedata._class.parameters);

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&a,&b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&b,&a));
}

BOOST_AUTO_TEST_CASE(TwoClassesAreExactTypes) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	BOOST_REQUIRE(analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedArrayTypesArentExact) {
	TypeAnalyzer analyzer;

	PureType* acontained = new PureType(TYPE_CLASS);
	PureType* bcontained = new PureType(TYPE_CLASS);
	acontained->typedata._class.classname = strdup("hello");
	bcontained->typedata._class.classname = strdup("hello");

	PureType a(TYPE_LIST);
	PureType b(TYPE_LIST);
	PureType* binner = new PureType(TYPE_LIST);
	a.typedata.list.contained = acontained;
	b.typedata.list.contained = binner;
	binner->typedata.list.contained = bcontained;

	BOOST_REQUIRE(!analyzer.isAExactlyB(acontained, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, acontained));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedOptionalTypesArentExact) {
	TypeAnalyzer analyzer;
	PureType* awrapped = new PureType(TYPE_CLASS);
	awrapped->typedata._class.classname = strdup("hello");

	PureType* bwrapped = new PureType(TYPE_CLASS);
	bwrapped->typedata._class.classname = strdup("hello");

	PureType a(TYPE_OPTIONAL);
	PureType* ainner = new PureType(TYPE_OPTIONAL);
	ainner->typedata.optional.contained = awrapped;
	a.typedata.optional.contained = ainner;

	PureType b(TYPE_OPTIONAL);
	b.typedata.optional.contained = bwrapped;

	BOOST_REQUIRE(!analyzer.isAExactlyB(awrapped, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, awrapped));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedNamesArentExact) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedTypesArentExact) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_PARAMETERIZED);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(OnlyOneParameterizedArentExact) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	a.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), a.typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedParameterizationLengthsArentExact) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	a.typedata._class.parameters = new PureTypeArray();
	b.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), a.typedata._class.parameters);
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), a.typedata._class.parameters);
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), b.typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedParameterizationsArentExact) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);
	PureType* asub = new PureType(TYPE_CLASS);
	PureType* bsub = new PureType(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosuba");
	bsub->typedata._class.classname = strdup("hellosubb");
	a.typedata._class.parameters = new PureTypeArray();
	b.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(asub, a.typedata._class.parameters);
	addPureTypeToPureTypeArray(bsub, b.typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(ExactParameterizationsArentExact) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_CLASS);
	PureType b(TYPE_CLASS);
	PureType* asub = new PureType(TYPE_CLASS);
	PureType* bsub = new PureType(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	a.typedata._class.parameters = new PureTypeArray();
	b.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(asub, a.typedata._class.parameters);
	addPureTypeToPureTypeArray(bsub, b.typedata._class.parameters);
	BOOST_REQUIRE(analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(ParameterizedTypesAreExactWithExactLabel) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_PARAMETERIZED);
	PureType b(TYPE_PARAMETERIZED);
	a.typedata.parameterized.label = strdup("hello");
	b.typedata.parameterized.label = strdup("hello");
	BOOST_REQUIRE(analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(ParameterizedTypesAreNotExactWithDifferentLabel) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_PARAMETERIZED);
	PureType b(TYPE_PARAMETERIZED);
	a.typedata.parameterized.label = strdup("hello");
	b.typedata.parameterized.label = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithVoidVsNonVoid) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_LAMBDA);
	PureType b(TYPE_LAMBDA);
	a.typedata.lambda.returntype = new PureType(TYPE_MATCHALL);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentReturnType) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_LAMBDA);
	PureType b(TYPE_LAMBDA);
	a.typedata.lambda.returntype = new PureType(TYPE_CLASS);
	b.typedata.lambda.returntype = new PureType(TYPE_CLASS);
	a.typedata.lambda.returntype->typedata._class.classname = strdup("hello");
	b.typedata.lambda.returntype->typedata._class.classname = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentArgumentCounts) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_LAMBDA);
	PureType b(TYPE_LAMBDA);
	a.typedata.lambda.arguments = new PureTypeArray();
	b.typedata.lambda.arguments = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), a.typedata.lambda.arguments);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithNullNonNullArguments) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_LAMBDA);
	PureType b(TYPE_LAMBDA);
	a.typedata.lambda.arguments = new PureTypeArray();
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentArguments) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_LAMBDA);
	PureType b(TYPE_LAMBDA);
	PureType* aarg = new PureType(TYPE_CLASS);
	aarg->typedata._class.classname = strdup("holl");
	PureType* barg = new PureType(TYPE_CLASS);
	barg->typedata._class.classname = strdup("holle");
	a.typedata.lambda.arguments = new PureTypeArray();
	addPureTypeToPureTypeArray(aarg, a.typedata.lambda.arguments);
	b.typedata.lambda.arguments = new PureTypeArray();
	addPureTypeToPureTypeArray(barg, b.typedata.lambda.arguments);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

/**
 * This is a bit of a tricky one. On the one hand, if someone
 * makes an array [1, TYPE_ERROR], that turns into [1, TYPE_MATCHALL],
 * which we then don't understand and should assume TYPE_MATCHALL[].
 * But on the other hand, this way we can represent `[]` and `nothing`
 * as a TYPE_LIST[TYPE_MATCHALL] and TYPE_OPTIONAL[TYPE_MATCHALL]
 */
BOOST_AUTO_TEST_CASE(MatchallTypesCommonAreTheirNeighbor) {
	TypeAnalyzer analyzer;
	PureType a(TYPE_MATCHALL);
	PureType b(TYPE_OPTIONAL);
	b.typedata.optional.contained = new PureType(TYPE_MATCHALL);
	PureType c(TYPE_CLASS);
	c.typedata._class.classname = strdup("Test");
	PureType d(TYPE_LAMBDA);
	PureType e(TYPE_LIST);
	e.typedata.optional.contained = new PureType(TYPE_MATCHALL);
	boost::optional<PureType*> ret;

	ret = analyzer.getCommonSubtypeOf(&a, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &b);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL); delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &c);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS); delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &d);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_LAMBDA); delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &e);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_LIST); delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&b, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL); delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&c, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS); delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&d, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_LAMBDA); delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&e, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_LIST); delete (*ret);
}

BOOST_AUTO_TEST_CASE(NothingOptionalCommonTypeIsOptionalType) {
	TypeAnalyzer analyzer;
	PureType optional(TYPE_OPTIONAL);
	PureType* contained = new PureType(TYPE_CLASS);
	optional.typedata.optional.contained = contained;
	contained->typedata._class.classname = strdup("Classname");
	PureType nothing(TYPE_OPTIONAL);
	nothing.typedata.optional.contained = new PureType(TYPE_MATCHALL);

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&optional, &nothing);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->typedata._class.classname == string("Classname"));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&nothing, &optional);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->typedata._class.classname == string("Classname"));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(NothingNothingCommonTypeIsNothing) {
	TypeAnalyzer analyzer;
	PureType nothing(TYPE_OPTIONAL);
	nothing.typedata.optional.contained = new PureType(TYPE_MATCHALL);
	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&nothing, &nothing);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_MATCHALL);
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(MismatchedTypesWithNoCommonType) {
	TypeAnalyzer analyzer;
	PureType lambda(TYPE_LAMBDA);
	PureType clazz(TYPE_CLASS); clazz.typedata._class.classname = strdup("Test");
	PureType list(TYPE_LIST);
	list.typedata.list.contained = new PureType(clazz);
	PureType unusable(TYPE_UNUSABLE);

	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&lambda, &clazz));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&lambda, &list));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&lambda, &unusable));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&clazz, &list));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&clazz, &unusable));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&list, &unusable));

	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&clazz, &lambda));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&list, &lambda));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&unusable, &lambda));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&list, &clazz));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&unusable, &clazz));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&unusable, &list));
}

BOOST_AUTO_TEST_CASE(CommonTypeNothingNotOptionalNotNothingIsOptionalOtherType) {
	TypeAnalyzer analyzer;
	PureType clazz(TYPE_CLASS);
	clazz.typedata._class.classname = strdup("Classname");
	PureType nothing(TYPE_OPTIONAL);
	nothing.typedata.optional.contained = new PureType(TYPE_MATCHALL);

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&clazz, &nothing);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->typedata._class.classname == string("Classname"));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&nothing, &clazz);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->typedata._class.classname == string("Classname"));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(ListTypesNotExactlyEqualHaveNoCommonType) {
	TypeAnalyzer analyzer;
	PureType listOptNums(TYPE_LIST);
	PureType* optNums = new PureType(TYPE_OPTIONAL);
	PureType* num = new PureType(TYPE_CLASS);

	listOptNums.typedata.list.contained = optNums;
	optNums->typedata.optional.contained = num;
	num->typedata._class.classname = strdup("Num");

	PureType listNums(TYPE_LIST);
	PureType* num2 = new PureType(TYPE_CLASS);

	listNums.typedata.list.contained = num2;
	num2->typedata._class.classname = strdup("Num");

	PureType listTexts(TYPE_LIST);
	PureType* text = new PureType(TYPE_CLASS);

	listTexts.typedata.list.contained = text;
	text->typedata._class.classname = strdup("Text");

	PureType listListTexts(TYPE_LIST);
	PureType* listListTextsInner = new PureType(TYPE_LIST);
	PureType* text2 = new PureType(TYPE_CLASS);

	listListTextsInner->typedata.list.contained = text2;
	listListTexts.typedata.list.contained = listListTextsInner;
	text2->typedata._class.classname = strdup("Text");

	PureType listListOptNums(TYPE_LIST);
	PureType* listListOptNumsInner = new PureType(TYPE_LIST);
	PureType* optNums2 = new PureType(TYPE_OPTIONAL);
	PureType* num3 = new PureType(TYPE_CLASS);

	listListOptNums.typedata.list.contained = listListOptNumsInner;
	listListOptNumsInner->typedata.list.contained = optNums2;
	optNums2->typedata.optional.contained = num3;
	num3->typedata._class.classname = strdup("Num");

	PureType listListNums(TYPE_LIST);
	PureType* listListNumsInner = new PureType(TYPE_LIST);
	PureType* num4 = new PureType(TYPE_CLASS);

	listListNums.typedata.list.contained = listListNumsInner;
	listListNumsInner->typedata.list.contained = num4;
	num4->typedata._class.classname = strdup("Num");

	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listOptNums, &listNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listOptNums, &listTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listOptNums, &listListTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listOptNums, &listListOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listOptNums, &listListNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listNums, &listTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listNums, &listListTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listNums, &listListOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listNums, &listListNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listTexts, &listListTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listTexts, &listListOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listTexts, &listListNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListTexts, &listListOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListTexts, &listListNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListOptNums, &listListNums));

	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listNums, &listOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listTexts, &listOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListTexts, &listOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListOptNums, &listOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListNums, &listOptNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listTexts, &listNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListTexts, &listNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListOptNums, &listNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListNums, &listNums));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListTexts, &listTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListOptNums, &listTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListNums, &listTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListOptNums, &listListTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListNums, &listListTexts));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&listListNums, &listListOptNums));

	boost::optional<PureType*> ret;
	ret = analyzer.getCommonSubtypeOf(&listOptNums, &listOptNums);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listOptNums));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&listNums, &listNums);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listNums));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&listTexts, &listTexts);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listTexts));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&listListTexts, &listListTexts);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listListTexts));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&listListOptNums, &listListOptNums);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listListOptNums));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&listListNums, &listListNums);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listListNums));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(TestExactClassesAreTheirOwnCommonType) {
	TypeAnalyzer analyzer;
	PureType text1(TYPE_CLASS);
	text1.typedata._class.classname = strdup("Text");
	PureType text2(TYPE_CLASS);
	text2.typedata._class.classname = strdup("Text");

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&text1, &text2);
	BOOST_REQUIRE(*ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Text"));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&text2, &text1);
	BOOST_REQUIRE(*ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Text"));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(TestClassesWithMismatchedParameterExistenceHaveNoCommonType) {
	TypeAnalyzer analyzer;
	PureType text1(TYPE_CLASS);
	text1.typedata._class.classname = strdup("Text");
	text1.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), text1.typedata._class.parameters);
	PureType text2(TYPE_CLASS);
	text2.typedata._class.classname = strdup("Text");

	boost::optional<PureType*> ret;
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&text1, &text2));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&text2, &text1));
}

BOOST_AUTO_TEST_CASE(TestClassesWithParameterValuesDifferentHaveNoCommonType) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType text1(TYPE_CLASS);
	text1.typedata._class.classname = strdup("Text");
	text1.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), text1.typedata._class.parameters);
	PureType text2(TYPE_CLASS);
	text2.typedata._class.classname = strdup("Text");
	text2.typedata._class.parameters = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(TYPE_MATCHALL), text1.typedata._class.parameters);

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&text1, &text2);
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&text1, &text2));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&text2, &text1));
}

BOOST_AUTO_TEST_CASE(TestClassWithParentIsCommonToParent) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType printer(TYPE_CLASS);
	printer.typedata._class.classname = strdup("Printer");
	PureType disabledPrinter(TYPE_CLASS);
	disabledPrinter.typedata._class.classname = strdup("DisabledPrinter");

	reference.addClass("Printer");
	reference.addClass("DisabledPrinter");
	reference.addInheritance("Printer", true);

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&printer, &disabledPrinter);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Printer"));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&disabledPrinter, &printer);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Printer"));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(TestClassesWithRootParentIsRootParent) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType stdErrPrinter(TYPE_CLASS);
	stdErrPrinter.typedata._class.classname = strdup("StdErrPrinter");
	PureType disabledPrinter(TYPE_CLASS);
	disabledPrinter.typedata._class.classname = strdup("DisabledPrinter");

	reference.addClass("Printer");
	reference.addClass("DisabledPrinter"); reference.addInheritance("Printer", true);
	reference.addClass("StdErrPrinter"); reference.addInheritance("Printer", true);

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&stdErrPrinter, &disabledPrinter);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Printer"));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&disabledPrinter, &stdErrPrinter);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Printer"));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(TestClassesWithDifferenntLengthPathsToRootParentIsRootParent) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType stdErrPrinter(TYPE_CLASS);
	stdErrPrinter.typedata._class.classname = strdup("StdErrPrinter");
	PureType logger(TYPE_CLASS);
	logger.typedata._class.classname = strdup("Logger");

	reference.addClass("OStream");
	reference.addClass("Printer"); reference.addInheritance("OStream", true);
	reference.addClass("StdErrPrinter"); reference.addInheritance("Printer", true);
	reference.addClass("Logger"); reference.addInheritance("OStream", true);

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&stdErrPrinter, &logger);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("OStream"));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&logger, &stdErrPrinter);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("OStream"));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(TestClassesWithRootRootParentIsRootRootParent) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType stdErrPrinter(TYPE_CLASS);
	stdErrPrinter.typedata._class.classname = strdup("StdErrPrinter");
	PureType disabledLogger(TYPE_CLASS);
	disabledLogger.typedata._class.classname = strdup("DisabledLogger");

	reference.addClass("OStream");
	reference.addClass("Printer"); reference.addInheritance("OStream", true);
	reference.addClass("StdErrPrinter"); reference.addInheritance("Printer", true);
	reference.addClass("Logger"); reference.addInheritance("OStream", true);
	reference.addClass("DisabledLogger"); reference.addInheritance("Logger", true);

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&stdErrPrinter, &disabledLogger);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("OStream"));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&disabledLogger, &stdErrPrinter);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("OStream"));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(TestTwoEquallyViableParentClassesHasNoCommonType) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType printer(TYPE_CLASS);
	printer.typedata._class.classname = strdup("Printer");
	PureType logger(TYPE_CLASS);
	logger.typedata._class.classname = strdup("Logger");

	reference.addClass("OStream");
	reference.addClass("Serializable");
	reference.addClass("Printer"); reference.addInheritance("OStream", true); reference.addInheritance("Serializable", false);
	reference.addClass("Logger"); reference.addInheritance("OStream", true); reference.addInheritance("Serializable", false);

	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&printer, &logger));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&logger, &printer));
}

BOOST_AUTO_TEST_CASE(TestTwoDistantEquallyViableParentClassesHasNoCommonType) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType disabledPrinter(TYPE_CLASS);
	disabledPrinter.typedata._class.classname = strdup("DisabledPrinter");
	PureType stdErrLogger(TYPE_CLASS);
	stdErrLogger.typedata._class.classname = strdup("StdErrLogger");

	reference.addClass("OStream");
	reference.addClass("Serializable");
	reference.addClass("Printer"); reference.addInheritance("OStream", true); reference.addInheritance("Serializable", false);
	reference.addClass("Logger"); reference.addInheritance("OStream", true); reference.addInheritance("Serializable", false);
	reference.addClass("DisabledPrinter"); reference.addInheritance("Printer", true);
	reference.addClass("StdErrLogger"); reference.addInheritance("Logger", true);

	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&disabledPrinter, &stdErrLogger));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&stdErrLogger, &disabledPrinter));
}

BOOST_AUTO_TEST_CASE(TestTwoLopsidedEquallyViableParentClassesHasNoCommonType) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType disabledPrinter(TYPE_CLASS);
	disabledPrinter.typedata._class.classname = strdup("DisabledPrinter");
	PureType logger(TYPE_CLASS);
	logger.typedata._class.classname = strdup("Logger");

	reference.addClass("OStream");
	reference.addClass("Serializable");
	reference.addClass("Printer"); reference.addInheritance("OStream", true); reference.addInheritance("Serializable", false);
	reference.addClass("Logger"); reference.addInheritance("OStream", true); reference.addInheritance("Serializable", false);
	reference.addClass("DisabledPrinter"); reference.addInheritance("Printer", true);

	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&disabledPrinter, &logger));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&logger, &disabledPrinter));
}

BOOST_AUTO_TEST_CASE(TestDistantEquallyViableParentClassesBelowValidParentClassIsNotAnIssue) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable reference;
	analyzer.reference = &reference;
	PureType disabledPrinter(TYPE_CLASS);
	disabledPrinter.typedata._class.classname = strdup("DisabledPrinter");
	PureType stdErrLogger(TYPE_CLASS);
	stdErrLogger.typedata._class.classname = strdup("StdErrLogger");

	reference.addClass("OStream");
	reference.addClass("Serializable");
	reference.addClass("Debuggable");
	reference.addClass("Printer"); reference.addInheritance("OStream", true); reference.addInheritance("Serializable", false);
	reference.addClass("Logger"); reference.addInheritance("OStream", true); reference.addInheritance("Serializable", false);
	reference.addClass("DisabledPrinter"); reference.addInheritance("Printer", true); reference.addInheritance("Debuggable", false);
	reference.addClass("StdErrLogger"); reference.addInheritance("Logger", true); reference.addInheritance("Debuggable", false);

	boost::optional<PureType*> ret = analyzer.getCommonSubtypeOf(&stdErrLogger, &disabledPrinter);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Debuggable"));
	delete (*ret);
	ret = analyzer.getCommonSubtypeOf(&disabledPrinter, &stdErrLogger);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Debuggable"));
	delete (*ret);
}

BOOST_AUTO_TEST_CASE(TestGetArrayReferenceLevelForClassIs0) {
	TypeAnalyzer analyzer;
	PureType clazz(TYPE_CLASS);
	BOOST_REQUIRE(analyzer.getArrayReferenceLevel(clazz) == 0);
}

BOOST_AUTO_TEST_CASE(TestGetArrayReferenceLevelForListIs1) {
	TypeAnalyzer analyzer;
	PureType list(TYPE_LIST);
	list.typedata.list.contained = new PureType(TYPE_CLASS);
	BOOST_REQUIRE(analyzer.getArrayReferenceLevel(list) == 1);
}

BOOST_AUTO_TEST_CASE(TestGetArrayReferenceLevelForListListIs2) {
	TypeAnalyzer analyzer;
	PureType list(TYPE_LIST);
	list.typedata.list.contained = new PureType(TYPE_LIST);
	list.typedata.list.contained->typedata.list.contained = new PureType(TYPE_CLASS);
	BOOST_REQUIRE(analyzer.getArrayReferenceLevel(list) == 2);
}

BOOST_AUTO_TEST_CASE(TestGetArrayReferenceLevelForListOptionalListIs2) {
	TypeAnalyzer analyzer;
	PureType list(TYPE_LIST);
	list.typedata.list.contained = new PureType(TYPE_OPTIONAL);
	list.typedata.list.contained->typedata.optional.contained = new PureType(TYPE_LIST);
	list.typedata.list.contained->typedata.optional.contained->typedata.list.contained = new PureType(TYPE_CLASS);
	BOOST_REQUIRE(analyzer.getArrayReferenceLevel(list) == 2);
}

BOOST_AUTO_TEST_SUITE_END();
