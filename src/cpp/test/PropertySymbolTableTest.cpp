/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PropertySymbolTableTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <boost/test/unit_test.hpp>

#include "PropertySymbolTable.h"

BOOST_AUTO_TEST_SUITE(PropertySymbolTableTestSuite);

BOOST_AUTO_TEST_CASE(AddingNeedsAreGotten) {
	TypeAnalyzer analyzer;
	PropertySymbolTable table(&analyzer);
	Type* thefirsttype = MakeType(TYPE_CLASS);
	Type* thesecondtype = MakeType(TYPE_CLASS);
	thefirsttype->typedata._class.classname = strdup("");
	thefirsttype->typedata._class.shadow = 0;
	thesecondtype->typedata._class.classname = strdup("");
	thesecondtype->typedata._class.shadow = 0;
	table.addNeed(thefirsttype, 0);
	table.addNeed(thesecondtype, 0);
	BOOST_REQUIRE(table.getNeeds()->at(0) == thefirsttype);
	BOOST_REQUIRE(table.getNeeds()->at(1) == thesecondtype);
}

BOOST_AUTO_TEST_CASE(DerivedSymbolTableChangesReturnTypes) {
	TypeAnalyzer analyzer;
	PropertySymbolTable table(&analyzer);
	ObjectProperty* property = new ObjectProperty();
	vector<Type*> parameters;
	vector<Type*> parameterizations;
	Type* methodtype = MakeType(TYPE_LAMBDA);
	Type parameter(TYPE_PARAMETERIZED);
	Type parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter.typedata.parameterized.label = strdup("E");
	methodtype->typedata.lambda.returntype = copyType(&parameter);
	property->type = methodtype;
	property->casing = "myMethod";
	parameters.push_back(&parameter);
	parameterizations.push_back(&parameterization);
	table.setParameters(&parameters);

	table.properties["myMethod"] = property;

	ReadOnlyPropertySymbolTable* derived = table.resolveParameters(parameterizations);

	boost::optional<Type*> method = derived->find("myMethod");
	BOOST_REQUIRE(method);
	BOOST_REQUIRE(*method);
	BOOST_REQUIRE((*method)->type == TYPE_LAMBDA);
	BOOST_REQUIRE((*method)->typedata.lambda.returntype != NULL);
	BOOST_REQUIRE((*method)->typedata.lambda.arguments == NULL);
	BOOST_REQUIRE((*method)->typedata.lambda.returntype->type == TYPE_CLASS);
	BOOST_REQUIRE((*method)->typedata.lambda.returntype->typedata._class.classname == string("AClass"));
}

BOOST_AUTO_TEST_CASE(DerivedSymbolTableChangesPropertyName) {
	TypeAnalyzer analyzer;
	PropertySymbolTable table(&analyzer);
	ObjectProperty* property = new ObjectProperty();
	vector<Type*> parameters;
	vector<Type*> parameterizations;
	Type* methodtype = MakeType(TYPE_LAMBDA);
	Type parameter(TYPE_PARAMETERIZED);
	Type parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter.typedata.parameterized.label = strdup("E");
	methodtype->typedata.lambda.arguments = MakeTypeArray();
	AddTypeToTypeArray(copyType(&parameter), methodtype->typedata.lambda.arguments);
	property->type = methodtype;
	property->casing = "myMethod(#)";
	parameters.push_back(&parameter);
	parameterizations.push_back(&parameterization);
	table.setParameters(&parameters);

	table.properties["myMethod(E)"] = property;

	ReadOnlyPropertySymbolTable* derived = table.resolveParameters(parameterizations);

	boost::optional<Type*> method = derived->find("myMethod(AClass)");
	BOOST_REQUIRE(method);
	BOOST_REQUIRE(*method);
	BOOST_REQUIRE((*method)->type == TYPE_LAMBDA);
	BOOST_REQUIRE((*method)->typedata.lambda.returntype == NULL);
	BOOST_REQUIRE((*method)->typedata.lambda.arguments != NULL);
	BOOST_REQUIRE((*method)->typedata.lambda.arguments->typecount == 1);
	BOOST_REQUIRE((*method)->typedata.lambda.arguments->types[0]->type == TYPE_CLASS);
	BOOST_REQUIRE((*method)->typedata.lambda.arguments->types[0]->typedata._class.classname == string("AClass"));
}

BOOST_AUTO_TEST_CASE(DerivedSymbolTableChangesNeedName) {
	TypeAnalyzer analyzer;
	PropertySymbolTable table(&analyzer);
	vector<Type*> parameters;
	vector<Type*> parameterizations;
	Type* parameter = MakeType(TYPE_PARAMETERIZED);
	Type parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter->typedata.parameterized.label = strdup("E");
	parameters.push_back(parameter);
	parameterizations.push_back(&parameterization);
	table.setParameters(&parameters);
	table.addNeed(parameter, 0);

	ReadOnlyPropertySymbolTable* derived = table.resolveParameters(parameterizations);

	vector<Type*>* needs = derived->getNeeds();

	BOOST_REQUIRE(needs->size() == 1);
	BOOST_REQUIRE(needs->at(0)->type == TYPE_CLASS);
	BOOST_REQUIRE(needs->at(0)->typedata._class.classname == string("AClass"));

	freeType(&parameterization);
}

BOOST_AUTO_TEST_SUITE_END();
