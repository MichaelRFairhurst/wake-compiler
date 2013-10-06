#include "LibraryLoader.h"

void LibraryLoader::loadToTable(ObjectSymbolTable* table) {
	loadStdLibToTable(table);
}

void LibraryLoader::loadStdLibToTable(ObjectSymbolTable* table) {
	table->addClass("Text");
	table->addClass("Int");
	table->addClass("Truth");

	table->addClass("Printer");
	vector<pair<string, TypeArray*> > segments_arguments;
	TypeArray* arguments = MakeTypeArray();
	Type* text = MakeType(TYPE_CLASS);text->typedata._class.classname = "Text";
	AddTypeToTypeArray(text, arguments);
	segments_arguments.push_back(pair<string, TypeArray*>("print", arguments));
	table->find("Printer")->addMethod(NULL, &segments_arguments, NULL);
}
