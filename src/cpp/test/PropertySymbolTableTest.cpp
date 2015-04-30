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
#include "PureTypeArray.h"

BOOST_AUTO_TEST_SUITE(PropertySymbolTableTestSuite);

BOOST_AUTO_TEST_CASE(AddingNeedsAreGotten) {
	TypeAnalyzer analyzer;
	PropertySymbolTable table(&analyzer, "");
	SpecializableVarDecl thefirsttype;
	thefirsttype.decl.typedata = PureType(TYPE_CLASS);
	SpecializableVarDecl thesecondtype;
	thesecondtype.decl.typedata = PureType(TYPE_CLASS);
	thefirsttype.decl.typedata.typedata._class.classname = strdup("");
	thesecondtype.decl.typedata.typedata._class.classname = strdup("");
	table.addNeed(&thefirsttype, 0, vector<Annotation*>());
	table.addNeed(&thesecondtype, 0, vector<Annotation*>());
	BOOST_REQUIRE(table.getNeeds()->at(0) == &thefirsttype);
	BOOST_REQUIRE(table.getNeeds()->at(1) == &thesecondtype);
}

BOOST_AUTO_TEST_CASE(DerivedSymbolTableChangesReturnTypes) {
	TypeAnalyzer analyzer;
	PropertySymbolTable table(&analyzer, "");
	ObjectProperty* property = new ObjectProperty();
	vector<PureType*>* parameters = new vector<PureType*>();
	vector<PureType*> parameterizations;
	PureType* methodtype = new PureType(TYPE_LAMBDA);
	PureType parameter(TYPE_PARAMETERIZED);
	PureType parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter.typedata.parameterized.label = strdup("E");
	methodtype->typedata.lambda.returntype = new PureType(parameter);
	property->decl.typedata = *methodtype;
	property->casing = "myMethod";
	parameters->push_back(&parameter);
	parameterizations.push_back(&parameterization);
	table.setParameters(parameters);

	table.properties["myMethod"] = property;

	auto_ptr<ReadOnlyPropertySymbolTable> derived(table.resolveParameters(parameterizations));

	boost::optional<PureType*> method = derived->find("myMethod");
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
	PropertySymbolTable table(&analyzer, "");
	ObjectProperty* property = new ObjectProperty();
	vector<PureType*>* parameters = new vector<PureType*>();
	vector<PureType*> parameterizations;
	PureType* methodtype = new PureType(TYPE_LAMBDA);
	PureType parameter(TYPE_PARAMETERIZED);
	PureType parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter.typedata.parameterized.label = strdup("E");
	methodtype->typedata.lambda.arguments = new PureTypeArray();
	addPureTypeToPureTypeArray(new PureType(parameter), methodtype->typedata.lambda.arguments);
	property->decl.typedata = *methodtype;
	property->casing = "myMethod(#)";
	parameters->push_back(&parameter);
	parameterizations.push_back(&parameterization);
	table.setParameters(parameters);

	table.properties["myMethod(E)"] = property;

	auto_ptr<ReadOnlyPropertySymbolTable> derived(table.resolveParameters(parameterizations));

	boost::optional<PureType*> method = derived->find("myMethod(AClass)");
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
	PropertySymbolTable table(&analyzer, "");
	vector<PureType*>* parameters = new vector<PureType*>();
	vector<PureType*> parameterizations;
	PureType* parameter = new PureType(TYPE_PARAMETERIZED);
	PureType parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter->typedata.parameterized.label = strdup("E");
	parameters->push_back(parameter);
	parameterizations.push_back(&parameterization);
	SpecializableVarDecl needDecl;
	needDecl.decl.typedata = *parameter;
	table.setParameters(parameters);
	table.addNeed(&needDecl, 0, vector<Annotation*>());

	auto_ptr<ReadOnlyPropertySymbolTable> derived(table.resolveParameters(parameterizations));

	vector<SpecializableVarDecl*>* needs = derived->getNeeds();

	BOOST_REQUIRE(needs->size() == 1);
	BOOST_REQUIRE(needs->at(0)->decl.typedata.type == TYPE_CLASS);
	BOOST_REQUIRE(needs->at(0)->decl.typedata.typedata._class.classname == string("AClass"));
}

BOOST_AUTO_TEST_SUITE_END();
