/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TableFileWriterTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "boost/test/unit_test.hpp"
#include "TableFileWriter.h"
#include <sstream>
#include <string>
#include "type.h"
#include <boost/lexical_cast.hpp>

#define ASSERTCHAR(v) BOOST_CHECK_MESSAGE(dataptr[i++] == v, "Expected " #v " in stream at pos " + boost::lexical_cast<string>(i) + " got " + string(1, (unsigned char) dataptr[i]));
#define ASSERTLENGTH(l) char* dataptr = (char*) malloc(l); out.read(dataptr, l); BOOST_CHECK_MESSAGE(!out.eof(), "too short"); out.peek(); BOOST_CHECK_MESSAGE(out.eof(), "too long"); int i = 0;

#define TABLE_FILE_VERSION 4

BOOST_AUTO_TEST_SUITE(TableFileWriterTest)

BOOST_AUTO_TEST_CASE(TestWritesSimple)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	table.classname = "classname";
	writer.write(out, &table);
	ASSERTLENGTH(16);

	ASSERTCHAR(TABLE_FILE_VERSION);
	ASSERTCHAR(9);
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(0); // inheritances
	ASSERTCHAR(0); // begin parameters
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(0); // annotations
}

BOOST_AUTO_TEST_CASE(TestWritesPublicMethod)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");

	vector<pair<string, TypeArray*> > segments_arguments;
	TypeArray* arguments = MakeTypeArray();
	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
	AddTypeToTypeArray(text, arguments);
	segments_arguments.push_back(pair<string, TypeArray*>("print", arguments));
	table.addMethod(NULL, &segments_arguments, PROPERTY_PUBLIC, vector<Annotation*>());
	table.classname = "classname";
	//freeTypeArray(arguments);

	writer.write(out, &table);

	ASSERTLENGTH(54);
	ASSERTCHAR(TABLE_FILE_VERSION);
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
		ASSERTCHAR(0); // alias length
		ASSERTCHAR(0); // specialty length
	ASSERTCHAR(0); // end arguments
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // specialty length
	ASSERTCHAR(0)  // prop annotations
	ASSERTCHAR(0); // inheritances
	ASSERTCHAR(0); // begin parameters
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(0); // class annotations
}

BOOST_AUTO_TEST_CASE(TestWritesNeed)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");

	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
	table.addNeed(text, 0, vector<Annotation*>());
	table.classname = "classname";

	writer.write(out, &table);
	ASSERTLENGTH(38);
	ASSERTCHAR(TABLE_FILE_VERSION);
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
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // prop annotations
	ASSERTCHAR(0); // inheritance
	ASSERTCHAR(0); // begin parameters
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(0); // annotations
}

BOOST_AUTO_TEST_CASE(TestWritesNeeds)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");

	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
	Type* printer = MakeType(TYPE_CLASS);printer->typedata._class.classname = strdup("Printer");
	table.addNeed(text, 0, vector<Annotation*>());
	table.addNeed(printer, 0, vector<Annotation*>());
	table.classname = "classname";

	writer.write(out, &table);

	ASSERTLENGTH(69);
	ASSERTCHAR(TABLE_FILE_VERSION);
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
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // prop annotations
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
	ASSERTCHAR(0); // alias length
	ASSERTCHAR(0); // spec length
	ASSERTCHAR(0); // prop annotations
	ASSERTCHAR(0); // inheritance
	ASSERTCHAR(0); // begin parameters
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(0); // annotations
}

BOOST_AUTO_TEST_CASE(TestWritesInheritance)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	table.classname = "classname";
	table.parentage["myparent"] = true;
	table.parentage["myinterface"] = false;
	writer.write(out, &table);
	ASSERTLENGTH(39);

	ASSERTCHAR(TABLE_FILE_VERSION);
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
	ASSERTCHAR(0); // begin parameters
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(0); // annotations
}

BOOST_AUTO_TEST_CASE(TestWritesParameters)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	table.classname = "classname";
	vector<Type*>* parameters = new vector<Type*>();
	Type* t = MakeType(TYPE_PARAMETERIZED); t->typedata.parameterized.label = strdup("T");
	Type* b = MakeType(TYPE_PARAMETERIZED); b->typedata.parameterized.label = strdup("B");
	Type* text = MakeType(TYPE_CLASS);
	Type* booL = MakeType(TYPE_CLASS);
	b->typedata.parameterized.upperbound = text;
	text->typedata._class.classname = strdup("Text");
	booL->typedata._class.classname = strdup("Bool");
	b->typedata.parameterized.lowerbound = booL;
	parameters->push_back(t); parameters->push_back(b);
	table.setParameters(parameters);
	writer.write(out, &table);
	ASSERTLENGTH(50);

	ASSERTCHAR(TABLE_FILE_VERSION);
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
	ASSERTCHAR(0); // alias
	ASSERTCHAR(0); // specialty
	ASSERTCHAR(3); // parameterized type
	ASSERTCHAR(1); ASSERTCHAR('B');
	ASSERTCHAR(TYPE_CLASS); // type upper bound
		ASSERTCHAR(4); ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
		ASSERTCHAR(0); // parameters
		ASSERTCHAR(0); // shadow
		ASSERTCHAR(0); // alias
		ASSERTCHAR(0); // specialty
	ASSERTCHAR(TYPE_CLASS); // type lower bound
		ASSERTCHAR(4); ASSERTCHAR('B'); ASSERTCHAR('o'); ASSERTCHAR('o'); ASSERTCHAR('l');
		ASSERTCHAR(0); // parameters
		ASSERTCHAR(0); // shadow
		ASSERTCHAR(0); // alias
		ASSERTCHAR(0); // specialty
	ASSERTCHAR(0); // shadow
	ASSERTCHAR(0); // alias
	ASSERTCHAR(0); // specialty
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(0); // end class annotations

	freeType(t); freeType(b);
}

BOOST_AUTO_TEST_CASE(TestWritesList)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	table.classname = "classname";
	vector<Type*> parameters;
	Type* list = MakeType(TYPE_LIST);
	Type* text = MakeType(TYPE_CLASS);
	list->typedata.list.contained = text;
	text->typedata._class.classname = strdup("Text");
	table.addProperty(list, 1, vector<Annotation*>());
	writer.write(out, &table);

	ASSERTLENGTH(46);
	ASSERTCHAR(TABLE_FILE_VERSION);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(6); // method name length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t'); ASSERTCHAR('['); ASSERTCHAR(']');
	ASSERTCHAR(6); // casing length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t'); ASSERTCHAR('['); ASSERTCHAR(']');
	ASSERTCHAR(1); // Flagged public
	ASSERTCHAR(TYPE_LIST);
		ASSERTCHAR(TYPE_CLASS);
			ASSERTCHAR(4); // class name ength
			ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
			ASSERTCHAR(0); // parameters
			ASSERTCHAR(0); // shadow
			ASSERTCHAR(0); // alias length
			ASSERTCHAR(0); // specialty length
		ASSERTCHAR(1); // backwards compat
		ASSERTCHAR(0); // alias length
		ASSERTCHAR(0); // specialty length
		ASSERTCHAR(0); // prop annotations
	ASSERTCHAR(0); // inheritances
	ASSERTCHAR(0); // begin parameters
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(0); // class annotations
}

BOOST_AUTO_TEST_CASE(TestWritesOptional)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	table.classname = "classname";
	vector<Type*> parameters;
	Type* optional = MakeType(TYPE_OPTIONAL);
	Type* text = MakeType(TYPE_CLASS);
	optional->typedata.optional.contained = text;
	text->typedata._class.classname = strdup("Text");
	table.addProperty(optional, 1, vector<Annotation*>());
	writer.write(out, &table);

	ASSERTLENGTH(42);
	ASSERTCHAR(TABLE_FILE_VERSION);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(4); // method name length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(4); // casing length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(1); // Flagged public
	ASSERTCHAR(TYPE_OPTIONAL);
		ASSERTCHAR(TYPE_CLASS);
			ASSERTCHAR(4); // class name ength
			ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
			ASSERTCHAR(0); // parameters
			ASSERTCHAR(0); // shadow
			ASSERTCHAR(0); // alias length
			ASSERTCHAR(0); // specialty length
		ASSERTCHAR(1); // backwards compat
		ASSERTCHAR(0); // alias length
		ASSERTCHAR(0); // specialty length
		ASSERTCHAR(0); // every annotations
	ASSERTCHAR(0); // inheritances
	ASSERTCHAR(0); // parameters
	ASSERTCHAR(0); // annotations
}

BOOST_AUTO_TEST_CASE(TestWritesClassAnnotations)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	table.classname = "classname";
	vector<Type*> parameters;
	vector<Annotation*> annotations;
	annotations.push_back(new Annotation());
	annotations.push_back(new Annotation());
	annotations[0]->name = strdup("Annotation");
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals[0].type = ANNOTATION_VAL_TYPE_TEXT;
	annotations[0]->vals[0].valdata.text = strdup("test");
	annotations[0]->vals[1].type = ANNOTATION_VAL_TYPE_BOOL;
	annotations[0]->vals[1].valdata.num = 0;
	annotations[0]->vals[2].type = ANNOTATION_VAL_TYPE_NOTHING;
	annotations[1]->name = strdup("Annotation2");
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals[0].type = ANNOTATION_VAL_TYPE_TEXT;
	annotations[1]->vals[0].valdata.text = strdup("test2");
	annotations[1]->vals[1].type = ANNOTATION_VAL_TYPE_BOOL;
	annotations[1]->vals[1].valdata.num = 1;
	annotations[1]->vals[2].type = ANNOTATION_VAL_TYPE_NOTHING;
	table.setAnnotations(annotations);
	writer.write(out, &table);

	ASSERTLENGTH(60);
	ASSERTCHAR(TABLE_FILE_VERSION);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(0); // end properties
	ASSERTCHAR(0); // end inheritances
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(10); // annotation length
	ASSERTCHAR('A'); ASSERTCHAR('n'); ASSERTCHAR('n'); ASSERTCHAR('o'); ASSERTCHAR('t'); ASSERTCHAR('a'); ASSERTCHAR('t'); ASSERTCHAR('i'); ASSERTCHAR('o'); ASSERTCHAR('n');
	ASSERTCHAR(1); // string value
	ASSERTCHAR(4); // string value
	ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('s'); ASSERTCHAR('t');
	ASSERTCHAR(3); // bool value
	ASSERTCHAR(0); // false
	ASSERTCHAR(4); // nothing value
	ASSERTCHAR(0); // end annotation values
	ASSERTCHAR(11); // annotation length
	ASSERTCHAR('A'); ASSERTCHAR('n'); ASSERTCHAR('n'); ASSERTCHAR('o'); ASSERTCHAR('t'); ASSERTCHAR('a'); ASSERTCHAR('t'); ASSERTCHAR('i'); ASSERTCHAR('o'); ASSERTCHAR('n'); ASSERTCHAR('2');
	ASSERTCHAR(1); // string value
	ASSERTCHAR(5); // string value
	ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('s'); ASSERTCHAR('t'); ASSERTCHAR('2');
	ASSERTCHAR(3); // bool value
	ASSERTCHAR(1); // true
	ASSERTCHAR(4); // nothing value
	ASSERTCHAR(0); // end annotation values
	ASSERTCHAR(0); // end annotations
}

BOOST_AUTO_TEST_CASE(TestWritesMethodAnnotations)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	PropertySymbolTable table(&tanalyzer, "");
	table.classname = "classname";
	vector<Type*> parameters;
	Type* text = MakeType(TYPE_CLASS);
	text->typedata._class.classname = strdup("Text");
	vector<Annotation*> annotations;
	annotations.push_back(new Annotation());
	annotations.push_back(new Annotation());
	annotations[0]->name = strdup("Annotation");
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals[0].type = ANNOTATION_VAL_TYPE_TEXT;
	annotations[0]->vals[0].valdata.text = strdup("test");
	annotations[0]->vals[1].type = ANNOTATION_VAL_TYPE_BOOL;
	annotations[0]->vals[1].valdata.num = 0;
	annotations[0]->vals[2].type = ANNOTATION_VAL_TYPE_NOTHING;
	annotations[1]->name = strdup("Annotation2");
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals[0].type = ANNOTATION_VAL_TYPE_TEXT;
	annotations[1]->vals[0].valdata.text = strdup("test2");
	annotations[1]->vals[1].type = ANNOTATION_VAL_TYPE_BOOL;
	annotations[1]->vals[1].valdata.num = 1;
	annotations[1]->vals[2].type = ANNOTATION_VAL_TYPE_NOTHING;
	table.addProperty(text, 1, annotations);
	writer.write(out, &table);

	ASSERTLENGTH(82);
	ASSERTCHAR(TABLE_FILE_VERSION);
	ASSERTCHAR(9); // classname length
	ASSERTCHAR('c'); ASSERTCHAR('l'); ASSERTCHAR('a'); ASSERTCHAR('s'); ASSERTCHAR('s'); ASSERTCHAR('n'); ASSERTCHAR('a'); ASSERTCHAR('m'); ASSERTCHAR('e');
	ASSERTCHAR(0); // not abstract
	ASSERTCHAR(4); // method name length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(4); // casing length
	ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
	ASSERTCHAR(1); // Flagged public
		ASSERTCHAR(TYPE_CLASS);
		ASSERTCHAR(4); // class name ength
		ASSERTCHAR('T'); ASSERTCHAR('e'); ASSERTCHAR('x'); ASSERTCHAR('t');
		ASSERTCHAR(0); // parameters
		ASSERTCHAR(0); // shadow
		ASSERTCHAR(0); // alias length
		ASSERTCHAR(0); // specialty length
		ASSERTCHAR(10); // annotation length
		ASSERTCHAR('A'); ASSERTCHAR('n'); ASSERTCHAR('n'); ASSERTCHAR('o'); ASSERTCHAR('t'); ASSERTCHAR('a'); ASSERTCHAR('t'); ASSERTCHAR('i'); ASSERTCHAR('o'); ASSERTCHAR('n');
		ASSERTCHAR(1); // string value
		ASSERTCHAR(4); // string value
		ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('s'); ASSERTCHAR('t');
		ASSERTCHAR(3); // bool value
		ASSERTCHAR(0); // false
		ASSERTCHAR(4); // nothing value
		ASSERTCHAR(0); // end annotation values
		ASSERTCHAR(11); // annotation length
		ASSERTCHAR('A'); ASSERTCHAR('n'); ASSERTCHAR('n'); ASSERTCHAR('o'); ASSERTCHAR('t'); ASSERTCHAR('a'); ASSERTCHAR('t'); ASSERTCHAR('i'); ASSERTCHAR('o'); ASSERTCHAR('n'); ASSERTCHAR('2');
		ASSERTCHAR(1); // string value
		ASSERTCHAR(5); // string value
		ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('s'); ASSERTCHAR('t'); ASSERTCHAR('2');
		ASSERTCHAR(3); // bool value
		ASSERTCHAR(1); // true
		ASSERTCHAR(4); // nothing value
		ASSERTCHAR(0); // end annotation values
		ASSERTCHAR(0); // end annotations
	ASSERTCHAR(0); // begin inheritances
	ASSERTCHAR(0); // end inheritances
	ASSERTCHAR(0); // end parameters
	ASSERTCHAR(0); // end annotations
}

BOOST_AUTO_TEST_SUITE_END()
