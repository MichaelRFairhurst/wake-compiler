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

}
