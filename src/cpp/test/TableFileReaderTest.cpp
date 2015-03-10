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
#include "type.h"

#define TABLE_FILE_VERSION "\005"

BOOST_AUTO_TEST_SUITE(TableFileReaderTest)

BOOST_AUTO_TEST_CASE(TestReadsSimple)
{
										// module  classname length 9     classname   not abstract begin inheritance	begin parameters	end parameters	begin annotations;
	char* dataptr = TABLE_FILE_VERSION	"\004test" "\011"                 "classname" "\000"       "\000"				"\000"				"\000"			"\000";

	std::stringstream in;
	in.write(dataptr, 21);

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
										// module   classname length 9     classname   not abstract begin inheritance	begin parameters	end parameters	begin annotations;
	char* dataptr = TABLE_FILE_VERSION	"\000" 		"\011"                 "classname" "\000"       "\000"				"\000"				"\000"			"\000";

	std::stringstream in;
	in.write(dataptr, 21);

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
	char* dataptr = TABLE_FILE_VERSION
					"\004" // module len
					"test" // module
					"\011" // classname length of 9
					"classname"
					"\000" // not abstract
					"\013" // method name length of 11
					"print(Text)"
					"\010" // casing length of 8
					"print(#)"
					"\001" // Flagged public
					"\001" // Lambda
						"\000" // No return
						"\002" // Type
						"\004" // class name ength
						"Text"
						"\000" // parameters
						"\000" // shadow
						"\000" // alias length
						"\000" // specialty length
						"\000" // annotations
					"\000" // end arguments
					"\000" // alias length
					"\000" // specialty length
					"\000" // begin inheritance
					"\000" // begin parameters
					"\000" // end parameters
					"\000"; // begin annotations

	std::stringstream in;
	in.write(dataptr, 59);

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
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->typedata._class.shadow == 0);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->alias == 0);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->specialty == 0);
	BOOST_CHECK((*table.find("print(Text)"))->alias == NULL);
	BOOST_CHECK((*table.find("print(Text)"))->specialty == NULL);
}

BOOST_AUTO_TEST_CASE(TestWritesNeed)
{
	char* dataptr = TABLE_FILE_VERSION
					"\004" // module len
					"test" // module
					"\011" // classname length of 9
					"classname"
					"\000" // not abstract
					"\004" // property len
					"Text"
					"\004" // casing len
					"Text"
					"\004" // flags - NEED
					"\002" // type
					"\004" // classname length
					"Text"
					"\000" // parameters
					"\000" // shadow
					"\000" // alias length
					"\000" // spec length
					"\000" // prop annotations
					"\000" // begin inheritance
					"\000" // begin parameters
					"\000" // end parameters
					"\000"; // begin annotations

	std::stringstream in;
	in.write(dataptr, 43);

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 1);
	BOOST_CHECK(table.getNeeds()->at(0)->type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.classname == string("Text"));
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.parameters == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(0)->alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->specialty == NULL);
	BOOST_CHECK(table.properties.size() == 1);
	BOOST_CHECK(table.properties["Text"]->type == table.getNeeds()->at(0));
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.getParameters().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestWritesNeeds)
{
	char* dataptr = TABLE_FILE_VERSION
					"\004" // module len
					"test" // module
					"\011" // classname length of 9
					"classname"
					"\000" // not abstract
					"\004" // property length
					"Text"
					"\004" // casing length
					"Text"
					"\004" // flags - NEED
					"\002" // type
					"\004" // classname length
					"Text"
					"\000" // parameters
					"\000" // shadow
					"\000" // alias length
					"\000" // spec length
					"\000" // prop annotations
					"\007" // property length
					"Printer"
					"\007" // casing length
					"Printer"
					"\004" // flags - NEED
					"\002" // type
					"\007" // classname length
					"Printer"
					"\000" // parameters
					"\000" // shadow
					"\000" // alias length
					"\000" // spec length
					"\000" // prop annotations
					"\000" // begin inheritance
					"\000" // begin parameters
					"\000" // end parameters
					"\000"; // begin annotations

	std::stringstream in;
	in.write(dataptr, 74);

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 2);
	BOOST_CHECK(table.getNeeds()->at(0)->type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.classname == string("Text"));
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.parameters == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(0)->alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->specialty == NULL);
	BOOST_CHECK(table.getNeeds()->at(1)->type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(1)->typedata._class.classname == string("Printer"));
	BOOST_CHECK(table.getNeeds()->at(1)->typedata._class.parameters == NULL);
	BOOST_CHECK(table.getNeeds()->at(1)->typedata._class.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(1)->alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(1)->specialty == NULL);
	BOOST_CHECK(table.properties.size() == 2);
	BOOST_CHECK(table.properties["Text"]->type == table.getNeeds()->at(0));
	BOOST_CHECK(table.properties["Printer"]->type == table.getNeeds()->at(1));
	BOOST_CHECK(table.parentage.size() == 0);
}

BOOST_AUTO_TEST_CASE(TestReadsInheritance)
{
	char* dataptr = TABLE_FILE_VERSION
					"\004" // module len
					"test" // module
					"\011" // classname len
					"classname"
					"\000" // not abstract
					"\000" // begin inheritance
					"\010" // parent legnth 8
					"myparent"
					"\001" // does extend
					"\013" // parent length 11
					"myinterface"
					"\000" // doesn't extend
					"\000" // begin parameters
					"\000" // end parameters
					"\000"; // begin annotations

	std::stringstream in;
	in.write(dataptr, 44);

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
	char* dataptr = TABLE_FILE_VERSION
					"\004" // module len
					"test" // module
					"\011" // classname len
					"classname"
					"\000" // not abstract
					"\000" // begin inheritance
					"\000" // begin parameters
					"\003" // parameterized type
					"\001" "T"
					"\000" // no upper bound
					"\000" // no lower bound
					"\000" // shadow
					"\000" // alias
					"\000" // specialty
					"\003" // parameterized type
					"\001" "B"
					"\002" // Type upper bound
						"\004" "Text"
						"\000" // parameters
						"\000" // shadow
						"\000" // alias
						"\000" // specialty
					"\002" // Type lower bound
						"\004" "Bool"
						"\000" // parameters
						"\000" // shadow
						"\000" // alias
						"\000" // specialty
					"\000" // shadow
					"\000" // alias
					"\000" // specialty
					"\000" // begin annotations
					"\000";// end annotations

	std::stringstream in;
	in.write(dataptr, 55);

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
	BOOST_CHECK(table.getParameters()[0]->typedata.parameterized.shadow == 0);
	BOOST_CHECK(table.getParameters()[0]->alias == NULL);
	BOOST_CHECK(table.getParameters()[0]->specialty == NULL);
	BOOST_CHECK(table.getParameters()[1]->type == TYPE_PARAMETERIZED);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.label == string("B"));
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound != NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->type == TYPE_CLASS);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->typedata._class.classname == string("Text"));
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->typedata._class.shadow == 0);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->typedata._class.parameters == NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->alias == NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.upperbound->specialty == NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound != NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound->typedata._class.classname == string("Bool"));
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound->typedata._class.shadow == 0);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound->typedata._class.parameters == NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound->alias == NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.lowerbound->specialty == NULL);
	BOOST_CHECK(table.getParameters()[1]->typedata.parameterized.shadow == 0);
	BOOST_CHECK(table.getParameters()[1]->alias == NULL);
	BOOST_CHECK(table.getParameters()[1]->specialty == NULL);
}

BOOST_AUTO_TEST_CASE(TestReadsClassAnnotations)
{
	char* dataptr = TABLE_FILE_VERSION
					"\004" // module len
					"test" // module
					"\011" // classname len
					"classname"
					"\000" // not abstract
					"\000" // begin inheritance
					"\000" // begin parameters
					"\000" // end parameters
					"\011" // length 9
					"Annotated"
					"\001" // string val
					"\004" // length 4
					"test"
					"\003" // bool val
					"\000" // false
					"\004" // nothing val
					"\000" // end annotation vals
					"\012" // length 10
					"Annotated2"
					"\001" // string val
					"\005" // length 5
					"test2"
					"\003" // bool val
					"\001" // true
					"\004" // nothing val
					"\000" // end annotation vals
					"\000";// end annotations

	std::stringstream in;
	in.write(dataptr, 63);

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
	char* dataptr = TABLE_FILE_VERSION
					"\004" // module len
					"test" // module
					"\011" // classname len
					"classname"
					"\000" // not abstract
					"\006"
					"prop()" // prop name
					"\006"
					"prop()" // prop casing
					"\000" // flags
					"\001" // lambda
					"\000" // no return type
					"\000" // no types
					"\000" // no alias
					"\000" // no specialty
					// begin annotations
					"\011" // length 9
					"Annotated"
					"\001" // string val
					"\004" // length 4
					"test"
					"\003" // bool val
					"\000" // false
					"\004" // nothing val
					"\000" // end annotation vals
					"\012" // length 10
					"Annotated2"
					"\001" // string val
					"\005" // length 5
					"test2"
					"\003" // bool val
					"\001" // true
					"\004" // nothing val
					"\000" // end annotation vals
					"\000" // end annotations
					"\000" // begin inheritance
					"\000" // begin parameters
					"\000" // end parameters
					"\000";// end annotations

	std::stringstream in;
	in.write(dataptr, 84);

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
