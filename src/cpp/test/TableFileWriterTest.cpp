#include "boost/test/unit_test.hpp"
#include "TableFileWriter.h"
#include <sstream>
#include <string>
#include "type.h"

#define ASSERTCHAR(v) BOOST_CHECK_MESSAGE(dataptr[i++] == v, "Expected " #v " in stream at pos " + string(1, i) + " got " + string(1, (unsigned char) dataptr[i]));
#define ASSERTLENGTH(l) char* dataptr = (char*) malloc(l); out.read(dataptr, l); BOOST_CHECK_MESSAGE(!out.eof(), "too short"); out.peek(); BOOST_CHECK_MESSAGE(out.eof(), "too long"); int i = 0;

BOOST_AUTO_TEST_SUITE(TableFileWriterTest)

BOOST_AUTO_TEST_CASE(TestWritesSimple)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	table.classname = "classname";
	writer.write(out, &table);
	ASSERTLENGTH(13);

	ASSERTCHAR(9);
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(0); // inheritances
	ASSERTCHAR(0); // inheritance
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
	//freeTypeArray(arguments);

	writer.write(out, &table);

	ASSERTLENGTH(54);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(11); // method name length
	ASSERTCHAR('p'); ASSERTCHAR('r'); ASSERTCHAR('i'); ASSERTCHAR('n'); ASSERTCHAR('t'); ASSERTCHAR('('); ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t'); ASSERTCHAR(')');
	ASSERTCHAR(8);
	ASSERTCHAR('p'); ASSERTCHAR('r'); ASSERTCHAR('i'); ASSERTCHAR('n'); ASSERTCHAR('t'); ASSERTCHAR('('); ASSERTCHAR('#'); ASSERTCHAR(')');
	ASSERTCHAR(1); // Flagged public
	ASSERTCHAR(1); // Lambda
		ASSERTCHAR(0); // No return
		ASSERTCHAR(2); // Type
		ASSERTCHAR(4); // class name ength
		ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
		ASSERTCHAR(0); // parameters
		ASSERTCHAR(0); // shadow
		ASSERTCHAR(0); // arrayed
		ASSERTCHAR(0); // alias length
		ASSERTCHAR(0); // specialty length
		ASSERTCHAR(0); // optionality
	ASSERTCHAR(0); // end arguments
	ASSERTCHAR(0); // arrayed
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // specialty length
	ASSERTCHAR(0); // optionality
	ASSERTCHAR(0); // inheritances
	ASSERTCHAR(0); // parameters
}

BOOST_AUTO_TEST_CASE(TestWritesNeed)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);

	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
	table.addNeed(text, 0);
	table.classname = "classname";

	writer.write(out, &table);
	ASSERTLENGTH(36);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(4); // property length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(4); // casing length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(4); // flags - need
	ASSERTCHAR(2); // type
	ASSERTCHAR(4); // classname length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(0); // parameters
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // array
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // optional
	ASSERTCHAR(0); // inheritance
	ASSERTCHAR(0); // parameters
}

BOOST_AUTO_TEST_CASE(TestWritesNeeds)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);

	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
	Type* printer = MakeType(TYPE_CLASS);printer->typedata._class.classname = strdup("Printer");
	table.addNeed(text, 0);
	table.addNeed(printer, 0);
	table.classname = "classname";

	writer.write(out, &table);

	ASSERTLENGTH(68);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(4); // propname length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(4); // casing length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(4); // flags -- PROPERTY_NEED
	ASSERTCHAR(2); // type
	ASSERTCHAR(4); // classname length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(0); // parameters
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // array
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // optional
	ASSERTCHAR(7); // property length
	ASSERTCHAR('P'); ASSERTCHAR('r'); ASSERTCHAR('i'); ASSERTCHAR('n'); ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('r');
	ASSERTCHAR(7); // casing length
	ASSERTCHAR('P'); ASSERTCHAR('r'); ASSERTCHAR('i'); ASSERTCHAR('n'); ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('r');
	ASSERTCHAR(4); // flags -- Type need
	ASSERTCHAR(2); // type
	ASSERTCHAR(7); // classname length
	ASSERTCHAR('P'); ASSERTCHAR('r'); ASSERTCHAR('i'); ASSERTCHAR('n'); ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('r');
	ASSERTCHAR(0); // parameters
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // array
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // optional
	ASSERTCHAR(0); // inheritance
	ASSERTCHAR(0); // parameters
}

BOOST_AUTO_TEST_CASE(TestWritesInheritance)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	table.classname = "classname";
	table.parentage["myparent"] = true;
	table.parentage["myinterface"] = false;
	writer.write(out, &table);
	ASSERTLENGTH(36);

	ASSERTCHAR(9);
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(0); // inheritance
	ASSERTCHAR(11); // "myinterface" length
	ASSERTCHAR('m'); ASSERTCHAR('y'); ASSERTCHAR('i'); ASSERTCHAR('n'); ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('r'); ASSERTCHAR('f'); ASSERTCHAR('a'); ASSERTCHAR('c'); ASSERTCHAR('e');
	ASSERTCHAR(0); // extends = false
	ASSERTCHAR(8); // "myparent" length
	ASSERTCHAR('m'); ASSERTCHAR('y'); ASSERTCHAR('p'); ASSERTCHAR('a'); ASSERTCHAR('r'); ASSERTCHAR('e'); ASSERTCHAR('n'); ASSERTCHAR('t');
	ASSERTCHAR(1); // extends = true
	ASSERTCHAR(0); // parameters
}

BOOST_AUTO_TEST_CASE(TestWritesParameters)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer);
	table.classname = "classname";
	vector<Type*> parameters;
	Type* t = MakeType(TYPE_PARAMETERIZED); t->typedata.parameterized.label = strdup("T");
	Type* b = MakeType(TYPE_PARAMETERIZED); b->typedata.parameterized.label = strdup("B");
	Type* text = MakeType(TYPE_CLASS);
	Type* booL = MakeType(TYPE_CLASS);
	b->typedata.parameterized.upperbound = text;
	text->typedata._class.classname = strdup("Text");
	booL->typedata._class.classname = strdup("Bool");
	b->typedata.parameterized.lowerbound = booL;
	parameters.push_back(t); parameters.push_back(b);
	table.setParameters(&parameters);
	writer.write(out, &table);
	ASSERTLENGTH(55);

	ASSERTCHAR(9);
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(0); // inheritance
	ASSERTCHAR(0); // parameters
	ASSERTCHAR(3); // parameterized type
	ASSERTCHAR(1); ASSERTCHAR('T');
	ASSERTCHAR(0); // no upper bound
	ASSERTCHAR(0); // no lower bound
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // arrayed
	ASSERTCHAR(0); // alias
	ASSERTCHAR(0); // specialty
	ASSERTCHAR(0); // optional
	ASSERTCHAR(3); // parameterized type
	ASSERTCHAR(1); ASSERTCHAR('B');
	ASSERTCHAR(TYPE_CLASS); // type upper bound
		ASSERTCHAR(4); ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
		ASSERTCHAR(0); // parameters
		ASSERTCHAR(0); // shadow
		ASSERTCHAR(0); // arrayed
		ASSERTCHAR(0); // alias
		ASSERTCHAR(0); // specialty
		ASSERTCHAR(0); // optional
	ASSERTCHAR(TYPE_CLASS); // type lower bound
		ASSERTCHAR(4); ASSERTCHAR('B'); ASSERTCHAR('o'); ASSERTCHAR('o'); ASSERTCHAR('l');
		ASSERTCHAR(0); // parameters
		ASSERTCHAR(0); // shadow
		ASSERTCHAR(0); // arrayed
		ASSERTCHAR(0); // alias
		ASSERTCHAR(0); // specialty
		ASSERTCHAR(0); // optional
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // arrayed
	ASSERTCHAR(0); // alias
	ASSERTCHAR(0); // specialty
	ASSERTCHAR(0); // optional

	freeType(t); freeType(b);
}

BOOST_AUTO_TEST_SUITE_END()
