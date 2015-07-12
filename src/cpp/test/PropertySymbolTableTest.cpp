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

#include "ConcretePropertySymbolTable.h"
#include "PureTypeArray.h"
#include "ClassSpaceSymbolTable.h"

using namespace wake;

BOOST_AUTO_TEST_SUITE(PropertySymbolTableTestSuite);

BOOST_AUTO_TEST_CASE(AddingNeedsAreGotten) {
	TypeAnalyzer analyzer;
	ConcretePropertySymbolTable table(&analyzer, "");
	SpecializableVarDecl<QUALIFIED> thefirsttype;
	thefirsttype.decl.typedata = PureType<QUALIFIED>(TYPE_CLASS);
	SpecializableVarDecl<QUALIFIED> thesecondtype;
	thesecondtype.decl.typedata = PureType<QUALIFIED>(TYPE_CLASS);
	thefirsttype.decl.typedata.typedata._class.classname = strdup("");
	thesecondtype.decl.typedata.typedata._class.classname = strdup("");
	table.addNeed(&thefirsttype, 0, vector<Annotation*>());
	table.addNeed(&thesecondtype, 0, vector<Annotation*>());
	BOOST_REQUIRE(table.getNeeds()->at(0) == &thefirsttype);
	BOOST_REQUIRE(table.getNeeds()->at(1) == &thesecondtype);
}

BOOST_AUTO_TEST_CASE(DerivedSymbolTableChangesReturnTypes) {
	TypeAnalyzer analyzer;
	ConcretePropertySymbolTable table(&analyzer, "");
	ObjectProperty* property = new ObjectProperty();
	vector<PureType<QUALIFIED>*>* parameters = new vector<PureType<QUALIFIED>*>();
	vector<PureType<QUALIFIED>*> parameterizations;
	PureType<QUALIFIED>* methodtype = new PureType<QUALIFIED>(TYPE_LAMBDA);
	PureType<QUALIFIED> parameter(TYPE_PARAMETERIZED);
	PureType<QUALIFIED> parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter.typedata.parameterized.label = strdup("E");
	methodtype->typedata.lambda.returntype = new PureType<QUALIFIED>(parameter);
	property->decl.typedata = *methodtype;
	property->casing = "myMethod";
	parameters->push_back(&parameter);
	parameterizations.push_back(&parameterization);
	table.setParameters(parameters);

	table.getProperties()["myMethod"] = property;

	auto_ptr<ReadOnlyPropertySymbolTable> derived(table.resolveParameters(parameterizations));

	boost::optional<PureType<QUALIFIED>*> method = derived->find("myMethod");
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
	ConcretePropertySymbolTable table(&analyzer, "");
	ObjectProperty* property = new ObjectProperty();
	vector<PureType<QUALIFIED>*>* parameters = new vector<PureType<QUALIFIED>*>();
	vector<PureType<QUALIFIED>*> parameterizations;
	PureType<QUALIFIED>* methodtype = new PureType<QUALIFIED>(TYPE_LAMBDA);
	PureType<QUALIFIED> parameter(TYPE_PARAMETERIZED);
	PureType<QUALIFIED> parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter.typedata.parameterized.label = strdup("E");
	methodtype->typedata.lambda.arguments = new PureTypeArray<QUALIFIED>();
	methodtype->typedata.lambda.arguments->addType(new PureType<QUALIFIED>(parameter));
	property->decl.typedata = *methodtype;
	property->casing = "myMethod(#)";
	parameters->push_back(&parameter);
	parameterizations.push_back(&parameterization);
	table.setParameters(parameters);

	table.getProperties()["myMethod(E)"] = property;

	auto_ptr<ReadOnlyPropertySymbolTable> derived(table.resolveParameters(parameterizations));

	boost::optional<PureType<QUALIFIED>*> method = derived->find("myMethod(AClass)");
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
	ConcretePropertySymbolTable table(&analyzer, "");
	vector<PureType<QUALIFIED>*>* parameters = new vector<PureType<QUALIFIED>*>();
	vector<PureType<QUALIFIED>*> parameterizations;
	PureType<QUALIFIED>* parameter = new PureType<QUALIFIED>(TYPE_PARAMETERIZED);
	PureType<QUALIFIED> parameterization(TYPE_CLASS);

	parameterization.typedata._class.classname = strdup("AClass");
	parameter->typedata.parameterized.label = strdup("E");
	parameters->push_back(parameter);
	parameterizations.push_back(&parameterization);
	SpecializableVarDecl<QUALIFIED> needDecl;
	needDecl.decl.typedata = *parameter;
	table.setParameters(parameters);
	table.addNeed(&needDecl, 0, vector<Annotation*>());

	auto_ptr<ReadOnlyPropertySymbolTable> derived(table.resolveParameters(parameterizations));

	vector<SpecializableVarDecl<QUALIFIED>*>* needs = derived->getNeeds();

	BOOST_REQUIRE(needs->size() == 1);
	BOOST_REQUIRE(needs->at(0)->decl.typedata.type == TYPE_CLASS);
	BOOST_REQUIRE(needs->at(0)->decl.typedata.typedata._class.classname == string("AClass"));
}

BOOST_AUTO_TEST_CASE(TestPropagateInheritanceExtendPreservesNeeds) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable classes;
	analyzer.reference = &classes;
	classes.addClass("SharedNeed");
	ConcretePropertySymbolTable parent(&analyzer, "");
	ConcretePropertySymbolTable child(&analyzer, "");
	SpecializableVarDecl<QUALIFIED> needDecl;
	needDecl.decl.typedata = PureType<QUALIFIED>(TYPE_CLASS);
	needDecl.decl.typedata.typedata._class.classname = strdup("SharedNeed");
	needDecl.specialty = strdup("specialty");
	parent.setParameters(new vector<PureType<QUALIFIED>*>());
	parent.addNeed(&needDecl, 0, vector<Annotation*>());

	ErrorTracker errors;
	propagateInheritanceTables(&child, &parent, true, errors);
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = child.getNeeds();

	BOOST_REQUIRE(needs->size() == 1);
	BOOST_REQUIRE(needs->at(0)->decl.typedata.type == TYPE_CLASS);
	BOOST_REQUIRE(needs->at(0)->decl.typedata.typedata._class.classname == string("SharedNeed"));
	BOOST_REQUIRE(needs->at(0)->specialty == string("specialty"));
}

BOOST_AUTO_TEST_CASE(TestPropagateInheritanceDropsPrivateNeeds) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable classes;
	analyzer.reference = &classes;
	classes.addClass("SharedNeed");
	ConcretePropertySymbolTable parent(&analyzer, "");
	ConcretePropertySymbolTable child(&analyzer, "");
	SpecializableVarDecl<QUALIFIED> needDecl;
	needDecl.decl.typedata = PureType<QUALIFIED>(TYPE_CLASS);
	needDecl.decl.typedata.typedata._class.classname = strdup("SharedNeed");
	needDecl.specialty = strdup("specialty");
	parent.setParameters(new vector<PureType<QUALIFIED>*>());
	parent.addNeed(&needDecl, 0, vector<Annotation*>());

	ErrorTracker errors;
	propagateInheritanceTables(&child, &parent, false, errors);
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = child.getNeeds();

	BOOST_REQUIRE(needs->size() == 0);
}

BOOST_AUTO_TEST_CASE(TestPropagateInheritanceKeepsPublicNeedsButNotAsNeed) {
	TypeAnalyzer analyzer;
	ClassSpaceSymbolTable classes;
	analyzer.reference = &classes;
	classes.addClass("SharedNeed");
	ConcretePropertySymbolTable parent(&analyzer, "");
	ConcretePropertySymbolTable child(&analyzer, "");
	SpecializableVarDecl<QUALIFIED> needDecl;
	needDecl.decl.typedata = PureType<QUALIFIED>(TYPE_CLASS);
	needDecl.decl.typedata.typedata._class.classname = strdup("SharedNeed");
	needDecl.specialty = strdup("specialty");
	parent.setParameters(new vector<PureType<QUALIFIED>*>());
	parent.addNeed(&needDecl, PROPERTY_PUBLIC, vector<Annotation*>());

	ErrorTracker errors;
	propagateInheritanceTables(&child, &parent, false, errors);
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = child.getNeeds();

	BOOST_REQUIRE(needs->size() == 0);
	BOOST_REQUIRE(child.isAbstract());
	boost::optional<PureType<QUALIFIED>*> method = child.find("SharedNeed");
	BOOST_REQUIRE(method);
	BOOST_REQUIRE(*method);
	BOOST_REQUIRE((*method)->type == TYPE_CLASS);
	BOOST_REQUIRE((*method)->typedata._class.classname == string("SharedNeed"));
}

BOOST_AUTO_TEST_SUITE_END();
