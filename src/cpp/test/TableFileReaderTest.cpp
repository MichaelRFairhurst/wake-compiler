/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TableFileReaderTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "boost/test/unit_test.hpp"
#include "TableFileReader.h"
#include <sstream>
#include <string>
#include "TableFileDataSets.h"

BOOST_AUTO_TEST_SUITE(TableFileReaderTest)

BOOST_AUTO_TEST_CASE(TestReadsSimple)
{
	std::stringstream in;
	in.write(SIMPLE_TABLE, sizeof(SIMPLE_TABLE));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.getModule() == string("test"));
	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.getParameters().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestReadsNoModuleName)
{
	std::stringstream in;
	in.write(SIMPLE_TABLE_WITH_MODULE, sizeof(SIMPLE_TABLE_WITH_MODULE));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.getModule() == string(""));
	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.getParameters().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestWritesPublicMethod)
{
	std::stringstream in;
	in.write(TABLE_WITH_PUBLIC_METHOD, sizeof(TABLE_WITH_PUBLIC_METHOD));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 1);
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.getParameters().size() == 0);
	BOOST_CHECK(table.isPublic("print(Text)") == true);
	BOOST_CHECK((*table.find("print(Text)"))->type == TYPE_LAMBDA);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.returntype == NULL);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->typecount == 1);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->type == TYPE_CLASS);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->typedata._class.classname == string("Text"));
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->typedata._class.parameters == NULL);
	BOOST_CHECK(table.properties["print(Text)"]->decl.alias == NULL);
}

BOOST_AUTO_TEST_CASE(TestWritesNeed)
{

	std::stringstream in;
	in.write(TABLE_WITH_ONE_NEED, sizeof(TABLE_WITH_ONE_NEED));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 1);
	BOOST_CHECK(table.getNeeds()->at(0)->decl.typedata.type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(0)->decl.typedata.typedata._class.classname == string("Text"));
	BOOST_CHECK(table.getNeeds()->at(0)->decl.typedata.typedata._class.parameters == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->decl.alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->decl.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(0)->specialty == NULL);
	BOOST_CHECK(table.properties.size() == 1);
	BOOST_CHECK(table.properties["Text"]->decl.typedata.type == TYPE_CLASS);
	BOOST_CHECK(table.properties["Text"]->decl.typedata.typedata._class.classname == string("Text"));
	BOOST_CHECK(table.properties["Text"]->decl.typedata.typedata._class.parameters == NULL);
	BOOST_CHECK(table.properties["Text"]->decl.alias == NULL);
	BOOST_CHECK(table.properties["Text"]->decl.shadow == 0);
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.getParameters().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestWritesNeeds)
{
	std::stringstream in;
	in.write(TABLE_WITH_TWO_NEEDS, sizeof(TABLE_WITH_TWO_NEEDS));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 2);
	BOOST_CHECK(table.getNeeds()->at(0)->decl.typedata.type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(0)->decl.typedata.typedata._class.classname == string("Text"));
	BOOST_CHECK(table.getNeeds()->at(0)->decl.typedata.typedata._class.parameters == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->decl.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(0)->decl.alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->specialty == string("name"));
	BOOST_CHECK(table.getNeeds()->at(1)->decl.typedata.type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(1)->decl.typedata.typedata._class.classname == string("Printer"));
	BOOST_CHECK(table.getNeeds()->at(1)->decl.typedata.typedata._class.parameters == NULL);
	BOOST_CHECK(table.getNeeds()->at(1)->decl.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(1)->decl.alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(1)->specialty == string("disabled"));
	BOOST_CHECK(table.properties.size() == 2);
	BOOST_CHECK(table.properties["Text"]->decl.typedata.type == TYPE_CLASS);
	BOOST_CHECK(table.properties["Text"]->decl.typedata.typedata._class.classname == string("Text"));
	BOOST_CHECK(table.properties["Text"]->decl.typedata.typedata._class.parameters == NULL);
	BOOST_CHECK(table.properties["Text"]->decl.shadow == 0);
	BOOST_CHECK(table.properties["Text"]->decl.alias == NULL);
	BOOST_CHECK(table.properties["Printer"]->decl.typedata.type == TYPE_CLASS);
	BOOST_CHECK(table.properties["Printer"]->decl.typedata.typedata._class.classname == string("Printer"));
	BOOST_CHECK(table.properties["Printer"]->decl.typedata.typedata._class.parameters == NULL);
	BOOST_CHECK(table.properties["Printer"]->decl.shadow == 0);
	BOOST_CHECK(table.properties["Printer"]->decl.alias == NULL);
	BOOST_CHECK(table.parentage.size() == 0);
}

BOOST_AUTO_TEST_CASE(TestReadsInheritance)
{
	std::stringstream in;
	in.write(TABLE_WITH_INHERITANCE, sizeof(TABLE_WITH_INHERITANCE));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 2);
	BOOST_CHECK(table.parentage["myparent"] == true);
	BOOST_CHECK(table.parentage["myinterface"] == false);
	BOOST_CHECK(table.getParameters().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestReadsParameters)
{
	std::stringstream in;
	in.write(TABLE_WITH_PARAMETERS, sizeof(TABLE_WITH_PARAMETERS));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.getParameters().size() == 2);
	BOOST_CHECK(table.getParameters()[0]->type == TYPE_PARAMETERIZED);
	BOOST_CHECK(table.getParameters()[0]->typedata.parameterized.label == string("T"));
	BOOST_CHECK(table.getParameters()[0]->typedata.parameterized.upperbound == NULL);
	BOOST_CHECK(table.getParameters()[0]->typedata.parameterized.lowerbound == NULL);
	BOOST_CHECK(table.getParameters()[1]->type == TYPE_PARAMETERIZED);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.label == string("B"));
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound != NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->type == TYPE_CLASS);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->typedata._class.classname == string("Text"));
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->typedata._class.parameters == NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound != NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound->typedata._class.classname == string("Bool"));
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound->typedata._class.parameters == NULL);
}

BOOST_AUTO_TEST_CASE(TestReadsClassAnnotations)
{

	std::stringstream in;
	in.write(TABLE_WITH_CLASS_ANNOTATIONS, sizeof(TABLE_WITH_CLASS_ANNOTATIONS));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.getParameters().size() == 0);
	BOOST_CHECK(table.getAnnotations().size() == 2);
	BOOST_CHECK(table.getAnnotations()[0].name == string("Annotated"));
	BOOST_CHECK(table.getAnnotations()[1].name == string("Annotated2"));
	BOOST_CHECK(table.getAnnotations()[0].vals.size() == 3);
	BOOST_CHECK(table.getAnnotations()[0].vals[0].type == ANNOTATION_VAL_TYPE_TEXT);
	BOOST_CHECK(table.getAnnotations()[0].vals[0].valdata.text == string("test"));
	BOOST_CHECK(table.getAnnotations()[0].vals[1].type == ANNOTATION_VAL_TYPE_BOOL);
	BOOST_CHECK(table.getAnnotations()[0].vals[1].valdata.num == 0);
	BOOST_CHECK(table.getAnnotations()[0].vals[2].type == ANNOTATION_VAL_TYPE_NOTHING);
	BOOST_CHECK(table.getAnnotations()[1].vals.size() == 3);
	BOOST_CHECK(table.getAnnotations()[1].vals[0].type == ANNOTATION_VAL_TYPE_TEXT);
	BOOST_CHECK(table.getAnnotations()[1].vals[0].valdata.text == string("test2"));
	BOOST_CHECK(table.getAnnotations()[1].vals[1].type == ANNOTATION_VAL_TYPE_BOOL);
	BOOST_CHECK(table.getAnnotations()[1].vals[1].valdata.num == 1);
	BOOST_CHECK(table.getAnnotations()[1].vals[2].type == ANNOTATION_VAL_TYPE_NOTHING);

}

BOOST_AUTO_TEST_CASE(TestReadsPropertyAnnotations)
{
	std::stringstream in;
	in.write(TABLE_WITH_PROPERTY_ANNOTATIONS, sizeof(TABLE_WITH_PROPERTY_ANNOTATIONS));

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 1);
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.getParameters().size() == 0);
	BOOST_CHECK(table.getAnnotations().size() == 0);
	BOOST_CHECK(table.properties["prop()"]);
	BOOST_CHECK(table.properties["prop()"]->annotations.size() == 2);
	BOOST_CHECK(table.properties["prop()"]->annotations[0].name == string("Annotated"));
	BOOST_CHECK(table.properties["prop()"]->annotations[1].name == string("Annotated2"));
	BOOST_CHECK(table.properties["prop()"]->annotations[0].vals.size() == 3);
	BOOST_CHECK(table.properties["prop()"]->annotations[0].vals[0].type == ANNOTATION_VAL_TYPE_TEXT);
	BOOST_CHECK(table.properties["prop()"]->annotations[0].vals[0].valdata.text == string("test"));
	BOOST_CHECK(table.properties["prop()"]->annotations[0].vals[1].type == ANNOTATION_VAL_TYPE_BOOL);
	BOOST_CHECK(table.properties["prop()"]->annotations[0].vals[1].valdata.num == 0);
	BOOST_CHECK(table.properties["prop()"]->annotations[0].vals[2].type == ANNOTATION_VAL_TYPE_NOTHING);
	BOOST_CHECK(table.properties["prop()"]->annotations[1].vals.size() == 3);
	BOOST_CHECK(table.properties["prop()"]->annotations[1].vals[0].type == ANNOTATION_VAL_TYPE_TEXT);
	BOOST_CHECK(table.properties["prop()"]->annotations[1].vals[0].valdata.text == string("test2"));
	BOOST_CHECK(table.properties["prop()"]->annotations[1].vals[1].type == ANNOTATION_VAL_TYPE_BOOL);
	BOOST_CHECK(table.properties["prop()"]->annotations[1].vals[1].valdata.num == 1);
	BOOST_CHECK(table.properties["prop()"]->annotations[1].vals[2].type == ANNOTATION_VAL_TYPE_NOTHING);

}

BOOST_AUTO_TEST_SUITE_END()
