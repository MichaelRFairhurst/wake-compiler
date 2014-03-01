#include "boost/test/unit_test.hpp"
#include "TableFileReader.h"
#include <sstream>
#include <string>
#include "type.h"

BOOST_AUTO_TEST_SUITE(TableFileReaderTest)

BOOST_AUTO_TEST_CASE(TestReadsSimple)
{
					// classname length 9  classname   not abstract begin methods begin inheritance
	char* dataptr = "\011"                 "classname" "\000"       "\000"        "\005";

	std::stringstream in;
	in.write(dataptr, 13);

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 0);
}

BOOST_AUTO_TEST_CASE(TestWritesPublicMethod)
{
	char* dataptr = "\011" // classname length of 9
					"classname"
					"\000" // not abstract
					"\000" // method flag
					"\013" // method name length of 11
					"print(Text)"
					"\001" // Flagged public
					"\001" // Lambda
						"\002" // Type
						"\004" // class name ength
						"Text"
						"\000" // shadow
						"\000" // arrayed
						"\000" // alias length
						"\000" // specialty length
						"\000" // optionality
					"\004" // end arguments
					"\000" // arrayed
					"\000" // alias length
					"\000" // specialty length
					"\000" // optionality
					"\005"; // begin inheritance

	std::stringstream in;
	in.write(dataptr, 43);

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 1);
	BOOST_CHECK(table.parentage.size() == 0);
	BOOST_CHECK(table.isPublic("print(Text)") == true);
	BOOST_CHECK((*table.find("print(Text)"))->type == TYPE_LAMBDA);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.returntype == NULL);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->typecount == 1);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->type == TYPE_CLASS);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->typedata._class.classname == string("Text"));
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->typedata._class.shadow == 0);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->arrayed == 0);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->alias == 0);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->specialty == 0);
	BOOST_CHECK((*table.find("print(Text)"))->typedata.lambda.arguments->types[0]->optional == 0);
	BOOST_CHECK((*table.find("print(Text)"))->arrayed == 0);
	BOOST_CHECK((*table.find("print(Text)"))->alias == NULL);
	BOOST_CHECK((*table.find("print(Text)"))->specialty == NULL);
	BOOST_CHECK((*table.find("print(Text)"))->optional == 0);
}

BOOST_AUTO_TEST_CASE(TestWritesNeed)
{
	char* dataptr = "\011" // classname length of 9
					"classname"
					"\000" // not abstract
					"\002" // type
					"\004" // classname length
					"Text"
					"\000" // shadow
					"\000" // array
					"\000" // alias length
					"\000" // spec length
					"\000" // optional
					"\000" // methods
					"\005"; // begin inheritance

	std::stringstream in;
	in.write(dataptr, 24);

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 1);
	BOOST_CHECK(table.getNeeds()->at(0)->type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.classname == string("Text"));
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(0)->arrayed == 0);
	BOOST_CHECK(table.getNeeds()->at(0)->alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->specialty == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->optional == NULL);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 0);
}

BOOST_AUTO_TEST_CASE(TestWritesNeeds)
{
	char* dataptr = "\011" // classname length of 9
					"classname"
					"\000" // not abstract
					"\002" // type
					"\004" // classname length
					"Text"
					"\000" // shadow
					"\000" // array
					"\000" // alias length
					"\000" // spec length
					"\000" // optional
					"\002" // type
					"\007" // classname length
					"Printer"
					"\000" // shadow
					"\000" // array
					"\000" // alias length
					"\000" // spec length
					"\000" // optional
					"\000" // methods
					"\005"; // begin inheritance

	std::stringstream in;
	in.write(dataptr, 38);

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 2);
	BOOST_CHECK(table.getNeeds()->at(0)->type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.classname == string("Text"));
	BOOST_CHECK(table.getNeeds()->at(0)->typedata._class.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(0)->arrayed == 0);
	BOOST_CHECK(table.getNeeds()->at(0)->alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->specialty == NULL);
	BOOST_CHECK(table.getNeeds()->at(0)->optional == NULL);
	BOOST_CHECK(table.getNeeds()->at(1)->type == TYPE_CLASS);
	BOOST_CHECK(table.getNeeds()->at(1)->typedata._class.classname == string("Printer"));
	BOOST_CHECK(table.getNeeds()->at(1)->typedata._class.shadow == 0);
	BOOST_CHECK(table.getNeeds()->at(1)->arrayed == 0);
	BOOST_CHECK(table.getNeeds()->at(1)->alias == NULL);
	BOOST_CHECK(table.getNeeds()->at(1)->specialty == NULL);
	BOOST_CHECK(table.getNeeds()->at(1)->optional == NULL);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 0);
}

BOOST_AUTO_TEST_CASE(TestReadsInheritance)
{
	char* dataptr = "\011" // classname len
					"classname"
					"\000" // not abstract
					"\000" // begin methods
					"\005" // begin inheritance
					"\010" // parent legnth 8
					"myparent"
					"\001" // does extend
					"\013" // parent length 11
					"myinterface"
					"\000"; // doesn't extend

	std::stringstream in;
	in.write(dataptr, 36);

	TableFileReader reader;

	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	reader.read(&table, in);

	BOOST_CHECK(table.classname == string("classname"));
	BOOST_CHECK(table.isAbstract() == false);
	BOOST_CHECK(table.getNeeds()->size() == 0);
	BOOST_CHECK(table.properties.size() == 0);
	BOOST_CHECK(table.parentage.size() == 2);
	BOOST_CHECK(table.parentage["myparent"] == true);
	BOOST_CHECK(table.parentage["myinterface"] == false);
}

BOOST_AUTO_TEST_SUITE_END()
