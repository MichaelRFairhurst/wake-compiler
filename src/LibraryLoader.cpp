#include "LibraryLoader.h"

void LibraryLoader::loadToTable(ObjectSymbolTable* table) {
	loadStdLibToTable(table);
}

void LibraryLoader::loadStdLibToTable(ObjectSymbolTable* table) {
	table->addClass("Text");
	table->addClass("Truth");
	table->addClass("Int");

	table->addClass("Printer");
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = "Text";
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("print", arguments));
		table->find("Printer")->addMethod(NULL, &segments_arguments, NULL);
	}
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = "Int";
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("print", arguments));
		table->find("Printer")->addMethod(NULL, &segments_arguments, NULL);
	}
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = "Text";
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("printLine", arguments));
		table->find("Printer")->addMethod(NULL, &segments_arguments, NULL);
	}
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = "Int";
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("printLine", arguments));
		table->find("Printer")->addMethod(NULL, &segments_arguments, NULL);
	}

	table->addClass("System");
	{
		vector<pair<string, TypeArray*> > segments_arguments;
		TypeArray* arguments = MakeTypeArray();
		Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = "Int";
		AddTypeToTypeArray(text, arguments);
		segments_arguments.push_back(pair<string, TypeArray*>("exit", arguments));
		table->find("System")->addMethod(NULL, &segments_arguments, NULL);
	}
}
