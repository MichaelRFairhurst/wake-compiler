#include "LibraryLoader.h"

void LibraryLoader::loadToTable(ObjectSymbolTable* table) {
	loadStdLibToTable(table);
}

void LibraryLoader::loadStdLibToTable(ObjectSymbolTable* table) {
	table->addClass("Text");
	table->addClass("Int");
	table->addClass("Truth");
}
