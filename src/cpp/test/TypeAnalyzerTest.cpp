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

	Type realtype(TYPE_CLASS);
	realtype.typedata._class.classname = strdup("test");

	Type* wrappedtype = new Type(TYPE_CLASS);
	wrappedtype->typedata._class.classname = strdup("test");
	Type optionaltype(TYPE_OPTIONAL);
	optionaltype.typedata.optional.levels = 1;
	optionaltype.typedata.optional.contained = wrappedtype;

	BOOST_REQUIRE(analyzer.isASubtypeOfB(&realtype, &optionaltype));
}

BOOST_AUTO_TEST_CASE(OptionalTypeIsNotSubtypeOfRealType) {
	TypeAnalyzer analyzer;

	Type* wrappedtype = new Type(TYPE_CLASS);
	wrappedtype->typedata._class.classname = strdup("test");
	Type optionaltype(TYPE_OPTIONAL);
	optionaltype.typedata.optional.levels = 1;
	optionaltype.typedata.optional.contained = wrappedtype;

	Type realtype(TYPE_CLASS);
	realtype.typedata._class.classname = strdup("test");

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&optionaltype, &realtype));
}

BOOST_AUTO_TEST_CASE(NothingIsSubtypesOfOptionalType) {
	TypeAnalyzer analyzer;
	Type optionaltype(TYPE_OPTIONAL);
	optionaltype.typedata.optional.levels = 1;
	optionaltype.typedata.optional.contained = NULL;
	Type nothing(TYPE_NOTHING);
	BOOST_REQUIRE(analyzer.isASubtypeOfB(&nothing, &optionaltype));
}

BOOST_AUTO_TEST_CASE(NothingIsNotSubtypeOfRealType) {
	TypeAnalyzer analyzer;
	Type nothing(TYPE_NOTHING);
	Type realtype(TYPE_CLASS);
	realtype.typedata._class.classname = strdup("test");
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&nothing, &realtype));
}

BOOST_AUTO_TEST_CASE(MatchallNameIsErroneousType) {
	TypeAnalyzer analyzer;
	Type matchall(TYPE_MATCHALL);
	BOOST_REQUIRE(analyzer.getNameForType(&matchall) == "{inferencing failed}");
}

BOOST_AUTO_TEST_CASE(ParameterizedClassTypesUnequalParametersNotSubtypes) {
	TypeAnalyzer analyzer;
	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	a.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), a.typedata._class.parameters);
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), a.typedata._class.parameters);
	b.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), b.typedata._class.parameters);

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&a,&b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&b,&a));
}

BOOST_AUTO_TEST_CASE(ParameterizedClassTypesRequireDifferentArgumentsAreSubtypes) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable table;
	analyzer.reference = &table;
	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);
	Type* asub = new Type(TYPE_CLASS);
	Type* bsub = new Type(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	a.typedata._class.parameters = MakeTypeArray();
	b.typedata._class.parameters = MakeTypeArray();

	AddTypeToTypeArray(asub, a.typedata._class.parameters);
	AddTypeToTypeArray(bsub, b.typedata._class.parameters);

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

	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);

	Type* asubwrapped = new Type(TYPE_CLASS);
	Type* asub = new Type(TYPE_OPTIONAL);
	Type* bsub = new Type(TYPE_CLASS);

	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");

	asubwrapped->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	asub->typedata.optional.contained = asubwrapped; // easiest way to make asub a subtype of bsub, and bsub a supertype of asub
	a.typedata._class.parameters = MakeTypeArray();
	b.typedata._class.parameters = MakeTypeArray();

	AddTypeToTypeArray(asub, a.typedata._class.parameters);
	AddTypeToTypeArray(bsub, b.typedata._class.parameters);

	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&a,&b));
	BOOST_REQUIRE(!analyzer.isASubtypeOfB(&b,&a));
}

BOOST_AUTO_TEST_CASE(TwoClassesAreExactTypes) {
	TypeAnalyzer analyzer;
	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	BOOST_REQUIRE(analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedArrayTypesArentExact) {
	TypeAnalyzer analyzer;

	Type* acontained = new Type(TYPE_CLASS);
	Type* bcontained = new Type(TYPE_CLASS);
	acontained->typedata._class.classname = strdup("hello");
	bcontained->typedata._class.classname = strdup("hello");

	Type a(TYPE_LIST);
	Type b(TYPE_LIST);
	a.typedata.list.contained = acontained;
	b.typedata.list.contained = bcontained;
	b.typedata.list.levels = 1;
	a.typedata.list.levels = 2;

	BOOST_REQUIRE(!analyzer.isAExactlyB(acontained, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, acontained));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedOptionalTypesArentExact) {
	TypeAnalyzer analyzer;
	Type* awrapped = new Type(TYPE_CLASS);
	awrapped->typedata._class.classname = strdup("hello");

	Type* bwrapped = new Type(TYPE_CLASS);
	bwrapped->typedata._class.classname = strdup("hello");

	Type a(TYPE_OPTIONAL);
	a.typedata.optional.levels = 2;
	a.typedata.optional.contained = awrapped;

	Type b(TYPE_OPTIONAL);
	b.typedata.optional.levels = 1;
	b.typedata.optional.contained = bwrapped;

	BOOST_REQUIRE(!analyzer.isAExactlyB(awrapped, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, awrapped));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedNamesArentExact) {
	TypeAnalyzer analyzer;
	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedTypesArentExact) {
	TypeAnalyzer analyzer;
	Type a(TYPE_CLASS);
	Type b(TYPE_PARAMETERIZED);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(OnlyOneParameterizedArentExact) {
	TypeAnalyzer analyzer;
	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	a.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), a.typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedParameterizationLengthsArentExact) {
	TypeAnalyzer analyzer;
	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	a.typedata._class.parameters = MakeTypeArray();
	b.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), a.typedata._class.parameters);
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), a.typedata._class.parameters);
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), b.typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MismatchedParameterizationsArentExact) {
	TypeAnalyzer analyzer;
	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);
	Type* asub = new Type(TYPE_CLASS);
	Type* bsub = new Type(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosuba");
	bsub->typedata._class.classname = strdup("hellosubb");
	a.typedata._class.parameters = MakeTypeArray();
	b.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(asub, a.typedata._class.parameters);
	AddTypeToTypeArray(bsub, b.typedata._class.parameters);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(ExactParameterizationsArentExact) {
	TypeAnalyzer analyzer;
	Type a(TYPE_CLASS);
	Type b(TYPE_CLASS);
	Type* asub = new Type(TYPE_CLASS);
	Type* bsub = new Type(TYPE_CLASS);
	a.typedata._class.classname = strdup("hello");
	b.typedata._class.classname = strdup("hello");
	asub->typedata._class.classname = strdup("hellosub");
	bsub->typedata._class.classname = strdup("hellosub");
	a.typedata._class.parameters = MakeTypeArray();
	b.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(asub, a.typedata._class.parameters);
	AddTypeToTypeArray(bsub, b.typedata._class.parameters);
	BOOST_REQUIRE(analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(ParameterizedTypesAreExactWithExactLabel) {
	TypeAnalyzer analyzer;
	Type a(TYPE_PARAMETERIZED);
	Type b(TYPE_PARAMETERIZED);
	a.typedata.parameterized.label = strdup("hello");
	b.typedata.parameterized.label = strdup("hello");
	BOOST_REQUIRE(analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(ParameterizedTypesAreNotExactWithDifferentLabel) {
	TypeAnalyzer analyzer;
	Type a(TYPE_PARAMETERIZED);
	Type b(TYPE_PARAMETERIZED);
	a.typedata.parameterized.label = strdup("hello");
	b.typedata.parameterized.label = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithVoidVsNonVoid) {
	TypeAnalyzer analyzer;
	Type a(TYPE_LAMBDA);
	Type b(TYPE_LAMBDA);
	a.typedata.lambda.returntype = new Type(TYPE_MATCHALL);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentReturnType) {
	TypeAnalyzer analyzer;
	Type a(TYPE_LAMBDA);
	Type b(TYPE_LAMBDA);
	a.typedata.lambda.returntype = new Type(TYPE_CLASS);
	b.typedata.lambda.returntype = new Type(TYPE_CLASS);
	a.typedata.lambda.returntype->typedata._class.classname = strdup("hello");
	b.typedata.lambda.returntype->typedata._class.classname = strdup("nothello");
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentArgumentCounts) {
	TypeAnalyzer analyzer;
	Type a(TYPE_LAMBDA);
	Type b(TYPE_LAMBDA);
	a.typedata.lambda.arguments = MakeTypeArray();
	b.typedata.lambda.arguments = MakeTypeArray();
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), a.typedata.lambda.arguments);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithNullNonNullArguments) {
	TypeAnalyzer analyzer;
	Type a(TYPE_LAMBDA);
	Type b(TYPE_LAMBDA);
	a.typedata.lambda.arguments = MakeTypeArray();
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(LambdaTypesAreNotExactWithDifferentArguments) {
	TypeAnalyzer analyzer;
	Type a(TYPE_LAMBDA);
	Type b(TYPE_LAMBDA);
	Type* aarg = new Type(TYPE_CLASS);
	aarg->typedata._class.classname = strdup("holl");
	Type* barg = new Type(TYPE_CLASS);
	barg->typedata._class.classname = strdup("holle");
	a.typedata.lambda.arguments = MakeTypeArray();
	AddTypeToTypeArray(aarg, a.typedata.lambda.arguments);
	b.typedata.lambda.arguments = MakeTypeArray();
	AddTypeToTypeArray(barg, b.typedata.lambda.arguments);
	BOOST_REQUIRE(!analyzer.isAExactlyB(&a, &b));
	BOOST_REQUIRE(!analyzer.isAExactlyB(&b, &a));
}

BOOST_AUTO_TEST_CASE(MatchallTypesCommonAreMatchall) {
	TypeAnalyzer analyzer;
	Type a(TYPE_MATCHALL);
	Type b(TYPE_OPTIONAL);
	Type c(TYPE_CLASS);
	Type d(TYPE_LAMBDA);
	Type e(TYPE_LIST);
	Type f(TYPE_NOTHING);
	boost::optional<Type*> ret;

	ret = analyzer.getCommonSubtypeOf(&a, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &b);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &c);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &d);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &e);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&a, &f);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&b, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&c, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&d, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&e, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&f, &a);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_MATCHALL); freeType(*ret);
}

BOOST_AUTO_TEST_CASE(NothingOptionalCommonTypeIsOptionalType) {
	TypeAnalyzer analyzer;
	Type optional(TYPE_OPTIONAL);
	Type* contained = new Type(TYPE_CLASS);
	optional.typedata.optional.levels = 1;
	optional.typedata.optional.contained = contained;
	contained->typedata._class.classname = strdup("Classname");
	Type nothing(TYPE_NOTHING);

	boost::optional<Type*> ret = analyzer.getCommonSubtypeOf(&optional, &nothing);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.levels == 1);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->typedata._class.classname == string("Classname"));
	freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&nothing, &optional);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.levels == 1);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->typedata._class.classname == string("Classname"));
	freeType(*ret);
}

BOOST_AUTO_TEST_CASE(NothingNothingCommonTypeIsNothing) {
	TypeAnalyzer analyzer;
	Type nothing(TYPE_NOTHING);
	boost::optional<Type*> ret = analyzer.getCommonSubtypeOf(&nothing, &nothing);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_NOTHING);
	freeType(*ret);
}

BOOST_AUTO_TEST_CASE(MismatchedTypesWithNoCommonType) {
	TypeAnalyzer analyzer;
	Type lambda(TYPE_LAMBDA);
	Type clazz(TYPE_CLASS);
	Type list(TYPE_LIST);
	Type unusable(TYPE_UNUSABLE);

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
	Type clazz(TYPE_CLASS);
	clazz.typedata._class.classname = strdup("Classname");
	Type nothing(TYPE_NOTHING);

	boost::optional<Type*> ret = analyzer.getCommonSubtypeOf(&clazz, &nothing);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.levels == 1);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->typedata._class.classname == string("Classname"));
	freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&nothing, &clazz);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE((*ret)->type == TYPE_OPTIONAL);
	BOOST_REQUIRE((*ret)->typedata.optional.levels == 1);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata.optional.contained->typedata._class.classname == string("Classname"));
	freeType(*ret);
}

BOOST_AUTO_TEST_CASE(ListTypesNotExactlyEqualHaveNoCommonType) {
	TypeAnalyzer analyzer;
	Type listOptNums(TYPE_LIST);
	Type* optNums = new Type(TYPE_OPTIONAL);
	Type* num = new Type(TYPE_CLASS);

	listOptNums.typedata.list.levels = 1;
	listOptNums.typedata.list.contained = optNums;
	optNums->typedata.optional.levels = 1;
	optNums->typedata.optional.contained = num;
	num->typedata._class.classname = strdup("Num");

	Type listNums(TYPE_LIST);
	Type* num2 = new Type(TYPE_CLASS);

	listNums.typedata.list.levels = 1;
	listNums.typedata.list.contained = num2;
	num2->typedata._class.classname = strdup("Num");

	Type listTexts(TYPE_LIST);
	Type* text = new Type(TYPE_CLASS);

	listTexts.typedata.list.levels = 1;
	listTexts.typedata.list.contained = text;
	text->typedata._class.classname = strdup("Text");

	Type listListTexts(TYPE_LIST);
	Type* text2 = new Type(TYPE_CLASS);

	listListTexts.typedata.list.levels = 2;
	listListTexts.typedata.list.contained = text2;
	text2->typedata._class.classname = strdup("Text");

	Type listListOptNums(TYPE_LIST);
	Type* optNums2 = new Type(TYPE_OPTIONAL);
	Type* num3 = new Type(TYPE_CLASS);

	listListOptNums.typedata.list.levels = 2;
	listListOptNums.typedata.list.contained = optNums2;
	optNums2->typedata.optional.levels = 1;
	optNums2->typedata.optional.contained = num3;
	num3->typedata._class.classname = strdup("Num");

	Type listListNums(TYPE_LIST);
	Type* num4 = new Type(TYPE_CLASS);

	listListNums.typedata.list.levels = 2;
	listListNums.typedata.list.contained = num4;
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

	boost::optional<Type*> ret;
	ret = analyzer.getCommonSubtypeOf(&listOptNums, &listOptNums);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listOptNums));
	freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&listNums, &listNums);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listNums));
	freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&listTexts, &listTexts);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listTexts));
	freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&listListTexts, &listListTexts);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listListTexts));
	freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&listListOptNums, &listListOptNums);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listListOptNums));
	freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&listListNums, &listListNums);
	BOOST_REQUIRE(ret);
	BOOST_REQUIRE(analyzer.isAExactlyB(*ret, &listListNums));
	freeType(*ret);
}

BOOST_AUTO_TEST_CASE(TestExactClassesAreTheirOwnCommonType) {
	TypeAnalyzer analyzer;
	Type text1(TYPE_CLASS);
	text1.typedata._class.classname = strdup("Text");
	text1.typedata._class.shadow = 0;
	Type text2(TYPE_CLASS);
	text2.typedata._class.classname = strdup("Text");
	text2.typedata._class.shadow = 1;

	boost::optional<Type*> ret = analyzer.getCommonSubtypeOf(&text1, &text2);
	BOOST_REQUIRE(*ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Text"));
	freeType(*ret);
	ret = analyzer.getCommonSubtypeOf(&text2, &text1);
	BOOST_REQUIRE(*ret);
	BOOST_REQUIRE((*ret)->type == TYPE_CLASS);
	BOOST_REQUIRE((*ret)->typedata._class.classname == string("Text"));
	freeType(*ret);
}

BOOST_AUTO_TEST_CASE(TestClassesWithMismatchedParameterExistenceHaveNoCommonType) {
	TypeAnalyzer analyzer;
	Type text1(TYPE_CLASS);
	text1.typedata._class.classname = strdup("Text");
	text1.typedata._class.shadow = 0;
	text1.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), text1.typedata._class.parameters);
	Type text2(TYPE_CLASS);
	text2.typedata._class.classname = strdup("Text");
	text2.typedata._class.shadow = 1;

	boost::optional<Type*> ret = analyzer.getCommonSubtypeOf(&text1, &text2);
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&text1, &text2));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&text2, &text1));
}

/*
BOOST_AUTO_TEST_CASE(TestClassesWithParameterValuesDifferentHaveNoCommonType) {
	TypeAnalyzer analyzer;
	Type text1(TYPE_CLASS);
	text1.typedata._class.classname = strdup("Text");
	text1.typedata._class.shadow = 0;
	text1.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), text1.typedata._class.parameters);
	Type text2(TYPE_CLASS);
	text2.typedata._class.classname = strdup("Text");
	text2.typedata._class.shadow = 1;
	text2.typedata._class.parameters = MakeTypeArray();
	AddTypeToTypeArray(new Type(TYPE_MATCHALL), text1.typedata._class.parameters);

	boost::optional<Type*> ret = analyzer.getCommonSubtypeOf(&text1, &text2);
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&text1, &text2));
	BOOST_REQUIRE(!analyzer.getCommonSubtypeOf(&text2, &text1));
}
*/

BOOST_AUTO_TEST_SUITE_END();
