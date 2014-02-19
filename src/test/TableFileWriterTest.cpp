#include "boost/test/unit_test.hpp"
#include "TableFileWriter.h"
#include <sstream>
#include "type.h"

BOOST_AUTO_TEST_SUITE(TableFileWriterTest)

BOOST_AUTO_TEST_CASE(TestWritesSimple)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	table.classname = "classname";
	writer.write(out, &table);
	BOOST_CHECK(out.str() == "{classname}NEM");
}

BOOST_AUTO_TEST_CASE(TestWritesPublicMethod)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);

	vector<pair<string, TypeArray*> > segments_arguments;
	TypeArray* arguments = MakeTypeArray();
	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
	AddTypeToTypeArray(text, arguments);
	segments_arguments.push_back(pair<string, TypeArray*>("print", arguments));
	table.addMethod(NULL, &segments_arguments, PROPERTY_PUBLIC);
	table.classname = "classname";
	freeTypeArray(arguments);

	writer.write(out, &table);
	BOOST_CHECK_MESSAGE(out.str() == "{classname}NEM{print(Text)}PLT{Text}0 0 {}{}0 0 {}{}0 ", "got " + out.str() + " instead");
}

BOOST_AUTO_TEST_CASE(TestWritesNeed)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);

	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
	table.addNeed(text);
	table.classname = "classname";

	writer.write(out, &table);
	BOOST_CHECK_MESSAGE(out.str() == "{classname}NET{Text}0 0 {}{}0 M", "got " + out.str() + " instead");
}

BOOST_AUTO_TEST_CASE(TestWritesNeeds)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);

	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
	Type* printer = MakeType(TYPE_CLASS);printer->typedata._class.classname = strdup("Printer");
	table.addNeed(text);
	table.addNeed(printer);
	table.classname = "classname";

	writer.write(out, &table);
	BOOST_CHECK_MESSAGE(out.str() == "{classname}NET{Text}0 0 {}{}0 T{Printer}0 0 {}{}0 M", "got " + out.str() + " instead");
}

BOOST_AUTO_TEST_SUITE_END()
