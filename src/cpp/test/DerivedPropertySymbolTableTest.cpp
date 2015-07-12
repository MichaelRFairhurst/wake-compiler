/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * DerivedPropertySymbolTableTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "boost/test/unit_test.hpp"
#include "DerivedPropertySymbolTable.h"

using namespace wake;

BOOST_AUTO_TEST_SUITE(DerivedPropertySymbolTableTest)

BOOST_AUTO_TEST_CASE(TestGetProperty)
{
	TypeAnalyzer analyzer;
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = new vector<SpecializableVarDecl<QUALIFIED>*>();
	map<string, ObjectProperty*>* properties = new map<string, ObjectProperty*>();
	ObjectProperty* prop = new ObjectProperty();
	prop->decl.typedata = PureType<QUALIFIED>(TYPE_LAMBDA);
	(*properties)["hello"] = prop;
	map<string, bool> parentage;
	DerivedPropertySymbolTable table(analyzer, needs, properties, parentage, "testclass", "testmodule");

	boost::optional<PureType<QUALIFIED>*> method = table.find("hello");
	BOOST_REQUIRE(method);
	BOOST_REQUIRE(*method == &prop->decl.typedata);
}

BOOST_AUTO_TEST_CASE(TestGetIsPublic)
{
	TypeAnalyzer analyzer;
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = new vector<SpecializableVarDecl<QUALIFIED>*>();
	map<string, ObjectProperty*>* properties = new map<string, ObjectProperty*>();
	ObjectProperty* privprop = new ObjectProperty();
	privprop->flags = 0;
	(*properties)["private"] = privprop;
	ObjectProperty* pubprop = new ObjectProperty();
	pubprop->flags = PROPERTY_PUBLIC;
	(*properties)["public"] = pubprop;
	map<string, bool> parentage;
	DerivedPropertySymbolTable table(analyzer, needs, properties, parentage, "testclass", "testmodule");

	BOOST_REQUIRE(table.isPublic(string("public")));
	BOOST_REQUIRE(!table.isPublic(string("private")));
}

BOOST_AUTO_TEST_CASE(TestGetIsAbstract)
{
	TypeAnalyzer analyzer;
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = new vector<SpecializableVarDecl<QUALIFIED>*>();
	map<string, ObjectProperty*>* properties = new map<string, ObjectProperty*>();
	ObjectProperty* prop = new ObjectProperty();
	prop->flags = PROPERTY_ABSTRACT;
	(*properties)["prop"] = prop;
	map<string, bool> parentage;
	DerivedPropertySymbolTable table(analyzer, needs, properties, parentage, "testclass", "testmodule");

	BOOST_REQUIRE(table.isAbstract());
}

BOOST_AUTO_TEST_CASE(TestGetIsNotAbstract)
{
	TypeAnalyzer analyzer;
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = new vector<SpecializableVarDecl<QUALIFIED>*>();
	map<string, ObjectProperty*>* properties = new map<string, ObjectProperty*>();
	ObjectProperty* prop = new ObjectProperty();
	prop->flags = 0;
	(*properties)["prop"] = prop;
	map<string, bool> parentage;
	DerivedPropertySymbolTable table(analyzer, needs, properties, parentage, "testclass", "testmodule");

	BOOST_REQUIRE(!table.isAbstract());
}

BOOST_AUTO_TEST_CASE(TestGetParentage)
{
	TypeAnalyzer analyzer;
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = new vector<SpecializableVarDecl<QUALIFIED>*>();
	map<string, ObjectProperty*>* properties = new map<string, ObjectProperty*>();
	map<string, bool> parentage;
	DerivedPropertySymbolTable table(analyzer, needs, properties, parentage, "testclass", "testmodule");

	BOOST_REQUIRE(&parentage == &table.getParentage());
}

BOOST_AUTO_TEST_CASE(TestGetNeeds)
{
	TypeAnalyzer analyzer;
	vector<SpecializableVarDecl<QUALIFIED>*>* needs = new vector<SpecializableVarDecl<QUALIFIED>*>();
	map<string, ObjectProperty*>* properties = new map<string, ObjectProperty*>();
	map<string, bool> parentage;
	DerivedPropertySymbolTable table(analyzer, needs, properties, parentage, "testclass", "testmodule");

	BOOST_REQUIRE(needs == table.getNeeds());
}

BOOST_AUTO_TEST_SUITE_END()
