#include "boost/test/unit_test.hpp"
#include "TableFileWriter.h"
#include <sstream>
#include <string>
#include "type.h"

#define ASSERTCHAR(v) BOOST_CHECK_MESSAGE(dataptr[i++] == v, "Expected " #v " in stream at pos " + std::to_string(i) + " got " + std::to_string((unsigned char) dataptr[i]));
#define ASSERTLENGTH(l) char* dataptr = (char*) malloc(l); out.read(dataptr, l); out.peek() /* trigger EOF check*/; BOOST_CHECK_MESSAGE(out.eof(), "wrong length"); int i = 0;

BOOST_AUTO_TEST_SUITE(TableFileWriterTest)

BOOST_AUTO_TEST_CASE(TestWritesSimple)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	table.classname = "classname";
	writer.write(out, &table);
	ASSERTLENGTH(12);

	ASSERTCHAR(9);
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(0); // methoods
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

	ASSERTLENGTH(42);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(0); // method flag
	ASSERTCHAR(11); // method name length
	ASSERTCHAR('p'); ASSERTCHAR('r'); ASSERTCHAR('i'); ASSERTCHAR('n'); ASSERTCHAR('t'); ASSERTCHAR('('); ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t'); ASSERTCHAR(')');
	ASSERTCHAR(1); // Flagged public
	ASSERTCHAR(2); // Lambda
		ASSERTCHAR(1); // Type
		ASSERTCHAR(4); // class name ength
		ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
		ASSERTCHAR(0); // shadow
		ASSERTCHAR(0); // arrayed
		ASSERTCHAR(0); // alias length
		ASSERTCHAR(0); // specialty length
		ASSERTCHAR(0); // optionality
	ASSERTCHAR(4); // end arguments
	ASSERTCHAR(0); // arrayed
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // specialty length
	ASSERTCHAR(0); // optionality
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
	ASSERTLENGTH(23);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(1); // type
	ASSERTCHAR(4); // classname length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // array
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // optional
	ASSERTCHAR(0); // methods
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

	ASSERTLENGTH(37);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(1); // type
	ASSERTCHAR(4); // classname length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // array
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // optional
	ASSERTCHAR(1); // type
	ASSERTCHAR(7); // classname length
	ASSERTCHAR('P'); ASSERTCHAR('r'); ASSERTCHAR('i'); ASSERTCHAR('n'); ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('r');
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // array
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // optional
	ASSERTCHAR(0); // methods
}

BOOST_AUTO_TEST_SUITE_END()
