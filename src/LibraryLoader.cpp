#include "LibraryLoader.h"
#include <fstream>
#include "TableFileReader.h"

void LibraryLoader::loadImport(string importname, string importpath, ObjectSymbolTable& objtable) {
	fstream importfile;
	importfile.open(importpath + "/" + importname + ".table");
	TableFileReader reader;

	objtable.addClass(importname);
	reader.read(objtable.find(importname), importfile);
}

void LibraryLoader::loadStdLibToTable(ObjectSymbolTable* table) {
	table->addClass("Text");
	table->addClass("Truth");
	table->addClass("Int");

	/*
	table->addClass("Printer");
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("print", arguments));
		table->find("Printer")->addMethod(NULL, &segments_arguments, PROPERTY_PUBLIC);
		freeTypeArray(arguments);
	}
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Int");
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("print", arguments));
		table->find("Printer")->addMethod(NULL, &segments_arguments, PROPERTY_PUBLIC);
		freeTypeArray(arguments);
	}
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Text");
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("printLine", arguments));
		table->find("Printer")->addMethod(NULL, &segments_arguments, PROPERTY_PUBLIC);
		freeTypeArray(arguments);
	}
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Int");
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("printLine", arguments));
		table->find("Printer")->addMethod(NULL, &segments_arguments, PROPERTY_PUBLIC);
		freeTypeArray(arguments);
	}

	table->addClass("System");
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = strdup("Int");
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("exit", arguments));
		table->find("System")->addMethod(NULL, &segments_arguments, PROPERTY_PUBLIC);
		freeTypeArray(arguments);
	}
	*/

}
