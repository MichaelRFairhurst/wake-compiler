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
#include <boost/lexical_cast.hpp>
#include "TableFileDataSets.h"
#include "ConcretePropertySymbolTable.h"

using namespace wake;

#define ASSERT_CHAR(v) \
	BOOST_CHECK_MESSAGE(dataptr[i] == v, \
	string("Expected ") \
	+ v + " | " + boost::lexical_cast<string>((int) v) \
	+ " in stream at pos " \
	+ boost::lexical_cast<string>(i) \
	+ " got " \
	+ string(1, (unsigned char) dataptr[i]) + " | " + boost::lexical_cast<string>((int) dataptr[i])); \
	i++;

#define ASSERT_LENGTH(l) \
	char* dataptr = (char*) malloc(l); \
	out.read(dataptr, l-1); \
	BOOST_CHECK_MESSAGE(!out.eof(), "too short"); \
	out.peek(); \
	BOOST_CHECK_MESSAGE(out.eof(), "too long"); \
	int i = 0;

#define ASSERT_TABLE(bin) \
	ASSERT_LENGTH(sizeof(bin)); \
	for(int z = 0; z < sizeof(bin) - 1; z++) { \
		ASSERT_CHAR(bin[z]); \
	}

BOOST_AUTO_TEST_SUITE(TableFileWriterTest)

BOOST_AUTO_TEST_CASE(TestWritesSimple)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");
	table.setClassname("classname");
	writer.write(out, &table);

	ASSERT_TABLE(SIMPLE_TABLE);
}

BOOST_AUTO_TEST_CASE(TestWritesNoModuleName)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "");
	table.setClassname("classname");
	writer.write(out, &table);

	ASSERT_TABLE(SIMPLE_TABLE_WITH_MODULE);
}

BOOST_AUTO_TEST_CASE(TestWritesPublicMethod)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");

	vector<pair<string, PureTypeArray<QUALIFIED>*> > segments_arguments;
	PureTypeArray<QUALIFIED>* arguments = new PureTypeArray<QUALIFIED>();
	PureType<QUALIFIED>* text = new PureType<QUALIFIED>(TYPE_CLASS);
	text->typedata._class.classname = strdup("Text");
	text->typedata._class.modulename = strdup("lang");
	arguments->addType(text);
	segments_arguments.push_back(pair<string, PureTypeArray<QUALIFIED>*>("print", arguments));
	table.addMethod(NULL, &segments_arguments, PROPERTY_PUBLIC, vector<Annotation*>());
	table.setClassname("classname");
	//freeTypeArray(arguments);

	writer.write(out, &table);

	ASSERT_TABLE(TABLE_WITH_PUBLIC_METHOD);
}

BOOST_AUTO_TEST_CASE(TestWritesNeed)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");

	PureType<QUALIFIED>* text = new PureType<QUALIFIED>(TYPE_CLASS);
	text->typedata._class.classname = strdup("Text");
	text->typedata._class.modulename = strdup("lang");
	VarDecl<QUALIFIED> textNeed; textNeed.typedata = *text;
	SpecializableVarDecl<QUALIFIED> textNeedDecl; textNeedDecl.decl = textNeed;
	textNeedDecl.specialty = strdup("special");

	table.addNeed(&textNeedDecl, PROPERTY_PUBLIC, vector<Annotation*>());
	table.setClassname("classname");

	writer.write(out, &table);

	ASSERT_TABLE(TABLE_WITH_ONE_NEED);
}

BOOST_AUTO_TEST_CASE(TestWritesNeeds)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");

	PureType<QUALIFIED>* text = new PureType<QUALIFIED>(TYPE_CLASS);
	text->typedata._class.classname = strdup("Text");
	text->typedata._class.modulename = strdup("lang");
	PureType<QUALIFIED>* printer = new PureType<QUALIFIED>(TYPE_CLASS);
	printer->typedata._class.classname = strdup("Printer");
	printer->typedata._class.modulename = strdup("io");
	VarDecl<QUALIFIED> textDecl; textDecl.typedata = *text;
	SpecializableVarDecl<QUALIFIED> textNeedDecl; textNeedDecl.decl = textDecl;
	textNeedDecl.specialty = strdup("name");
	VarDecl<QUALIFIED> printerDecl; printerDecl.typedata = *printer;
	SpecializableVarDecl<QUALIFIED> printerNeedDecl; printerNeedDecl.decl = printerDecl;
	printerNeedDecl.specialty = strdup("disabled");
	table.addNeed(&textNeedDecl, PROPERTY_PUBLIC, vector<Annotation*>());
	table.addNeed(&printerNeedDecl, PROPERTY_PUBLIC, vector<Annotation*>());
	table.setClassname("classname");

	writer.write(out, &table);

	ASSERT_TABLE(TABLE_WITH_TWO_NEEDS);
}

BOOST_AUTO_TEST_CASE(TestWritesInheritance)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");
	table.setClassname("classname");
	table.getParentageModifiable()["myinterface"] = false;
	table.getParentageModifiable()["myparent"] = true;
	writer.write(out, &table);

	ASSERT_TABLE(TABLE_WITH_INHERITANCE);
}

BOOST_AUTO_TEST_CASE(TestWritesParameters)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");
	table.setClassname("classname");
	vector<PureType<QUALIFIED>*>* parameters = new vector<PureType<QUALIFIED>*>();
	PureType<QUALIFIED> t(TYPE_PARAMETERIZED); t.typedata.parameterized.label = strdup("T");
	PureType<QUALIFIED> b(TYPE_PARAMETERIZED); b.typedata.parameterized.label = strdup("B");
	PureType<QUALIFIED>* text = new PureType<QUALIFIED>(TYPE_CLASS);
	PureType<QUALIFIED>* booll = new PureType<QUALIFIED>(TYPE_CLASS);
	b.typedata.parameterized.upperbound = text;
	text->typedata._class.classname = strdup("Text");
	text->typedata._class.modulename = strdup("lang");
	booll->typedata._class.classname = strdup("Bool");
	booll->typedata._class.modulename = strdup("lang");
	b.typedata.parameterized.lowerbound = booll;
	parameters->push_back(&t); parameters->push_back(&b);
	table.setParameters(parameters);
	writer.write(out, &table);

	ASSERT_TABLE(TABLE_WITH_PARAMETERS);
}

/*
BOOST_AUTO_TEST_CASE(TestWritesList)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");
	table.classname = "classname";
	vector<PureType*> parameters;
	PureType* list = new PureType(TYPE_LIST);
	PureType* text = new PureType(TYPE_CLASS);
	list->typedata.list.contained = text;
	text->typedata._class.classname = strdup("Text");
	table.addProperty(list, 1, vector<Annotation*>());
	writer.write(out, &table);

	ASSERTLENGTH(51);
	ASSERTCHAR(TABLE_FILE_VERSION);
	ASSERTCHAR(4); // module len
	ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('s'); ASSERTCHAR('t');
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
	ConcretePropertySymbolTable table(&tanalyzer, "test");
	table.classname = "classname";
	vector<PureType*> parameters;
	PureType* optional = new PureType(TYPE_OPTIONAL);
	PureType* text = new PureType(TYPE_CLASS);
	optional->typedata.optional.contained = text;
	text->typedata._class.classname = strdup("Text");
	table.addProperty(optional, 1, vector<Annotation*>());
	writer.write(out, &table);

	ASSERTLENGTH(47);
	ASSERTCHAR(TABLE_FILE_VERSION);
	ASSERTCHAR(4); // module len
	ASSERTCHAR('t'); ASSERTCHAR('e'); ASSERTCHAR('s'); ASSERTCHAR('t');
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
*/

BOOST_AUTO_TEST_CASE(TestWritesClassAnnotations)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");
	table.setClassname("classname");
	vector<PureType<QUALIFIED>*> parameters;
	vector<Annotation*> annotations;
	annotations.push_back(new Annotation());
	annotations.push_back(new Annotation());
	annotations[0]->name = strdup("Annotated");
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals[0].type = ANNOTATION_VAL_TYPE_TEXT;
	annotations[0]->vals[0].valdata.text = strdup("test");
	annotations[0]->vals[1].type = ANNOTATION_VAL_TYPE_BOOL;
	annotations[0]->vals[1].valdata.num = 0;
	annotations[0]->vals[2].type = ANNOTATION_VAL_TYPE_NOTHING;
	annotations[1]->name = strdup("Annotated2");
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

	ASSERT_TABLE(TABLE_WITH_CLASS_ANNOTATIONS);
}

BOOST_AUTO_TEST_CASE(TestWritesMethodAnnotations)
{
	std::stringstream out;
	TableFileWriter writer;
	TypeAnalyzer tanalyzer;
	ConcretePropertySymbolTable table(&tanalyzer, "test");
	table.setClassname("classname");
	vector<PureType<QUALIFIED>*> parameters;
	vector<Annotation*> annotations;
	annotations.push_back(new Annotation());
	annotations.push_back(new Annotation());
	annotations[0]->name = strdup("Annotated");
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals.push_back(new AnnotationVal());
	annotations[0]->vals[0].type = ANNOTATION_VAL_TYPE_TEXT;
	annotations[0]->vals[0].valdata.text = strdup("test");
	annotations[0]->vals[1].type = ANNOTATION_VAL_TYPE_BOOL;
	annotations[0]->vals[1].valdata.num = 0;
	annotations[0]->vals[2].type = ANNOTATION_VAL_TYPE_NOTHING;
	annotations[1]->name = strdup("Annotated2");
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals.push_back(new AnnotationVal());
	annotations[1]->vals[0].type = ANNOTATION_VAL_TYPE_TEXT;
	annotations[1]->vals[0].valdata.text = strdup("test2");
	annotations[1]->vals[1].type = ANNOTATION_VAL_TYPE_BOOL;
	annotations[1]->vals[1].valdata.num = 1;
	annotations[1]->vals[2].type = ANNOTATION_VAL_TYPE_NOTHING;

	vector<pair<string, PureTypeArray<QUALIFIED>*> > segments_arguments;
	PureTypeArray<QUALIFIED>* arguments = new PureTypeArray<QUALIFIED>();
	segments_arguments.push_back(pair<string, PureTypeArray<QUALIFIED>*>("prop", arguments));
	table.addMethod(NULL, &segments_arguments, 0, annotations);

	writer.write(out, &table);

	ASSERT_TABLE(TABLE_WITH_PROPERTY_ANNOTATIONS);
}

BOOST_AUTO_TEST_SUITE_END()
