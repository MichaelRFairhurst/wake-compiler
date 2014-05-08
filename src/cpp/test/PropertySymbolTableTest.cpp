#include <boost/test/unit_test.hpp>

#include "PropertySymbolTable.h"

BOOST_AUTO_TEST_SUITE(PropertySymbolTableTestSuite);

BOOST_AUTO_TEST_CASE(AddingNeedsAreGotten) {
	TypeAnalyzer analyzer;
	PropertySymbolTable table(&analyzer);
	Type* thefirsttype = MakeType(TYPE_CLASS);
	Type* thesecondtype = MakeType(TYPE_CLASS);
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
	Type* parameter = MakeType(TYPE_PARAMETERIZED);
	Type* parameterization = MakeType(TYPE_CLASS);

	parameterization->typedata._class.classname = strdup("AClass");
	parameter->typedata.parameterized.label = strdup("E");
	methodtype->typedata.lambda.returntype = copyType(parameter);
	property->type = methodtype;
	property->casing = "myMethod";
	parameters.push_back(parameter);
	parameterizations.push_back(parameterization);
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

	//freeType(methodtype);
	freeType(parameter); freeType(parameterization);
}

BOOST_AUTO_TEST_CASE(DerivedSymbolTableChangesPropertyName) {
	TypeAnalyzer analyzer;
	PropertySymbolTable table(&analyzer);
	ObjectProperty* property = new ObjectProperty();
	vector<Type*> parameters;
	vector<Type*> parameterizations;
	Type* methodtype = MakeType(TYPE_LAMBDA);
	Type* parameter = MakeType(TYPE_PARAMETERIZED);
	Type* parameterization = MakeType(TYPE_CLASS);

	parameterization->typedata._class.classname = strdup("AClass");
	parameter->typedata.parameterized.label = strdup("E");
	methodtype->typedata.lambda.arguments = MakeTypeArray();
	AddTypeToTypeArray(copyType(parameter), methodtype->typedata.lambda.arguments);
	property->type = methodtype;
	property->casing = "myMethod(#)";
	parameters.push_back(parameter);
	parameterizations.push_back(parameterization);
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

	//freeType(methodtype);
	freeType(parameter); freeType(parameterization);
}

BOOST_AUTO_TEST_SUITE_END();
