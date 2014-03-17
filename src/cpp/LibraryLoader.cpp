#include "LibraryLoader.h"
#include <fstream>
#include "TableFileReader.h"

void LibraryLoader::loadImport(string importname, string importpath, ObjectSymbolTable& objtable) {
	fstream importfile;
	importfile.open(importpath + "/" + importname + ".table");
	TableFileReader reader;

	PropertySymbolTable* table = objtable.getEmptyPropertySymbolTable();
	reader.read(table, importfile);
	objtable.importClass(table);
}

void LibraryLoader::loadStdLibToTable(ObjectSymbolTable* table) {
#ifdef STDLIB_ONLY_DEFINE_PRIMITIVES
	PropertySymbolTable* ptable = table->getEmptyPropertySymbolTable();
	ptable->classname = "Text";
	table->importClass(ptable);

	ptable = table->getEmptyPropertySymbolTable();
	ptable->classname = "Bool";
	table->importClass(ptable);

	ptable = table->getEmptyPropertySymbolTable();
	ptable->classname = "Int";
	table->importClass(ptable);
#else
	sstream importbin;
	TableFileReader reader;

#include "gen/waketables.c"
	PropertySymbolTable* ptable = table.getEmptyPropertySymbolTable();reader.read(ptable, importbin);table.importClass(ptable);
#endif

}
