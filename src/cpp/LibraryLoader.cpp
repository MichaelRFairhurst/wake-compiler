#include "LibraryLoader.h"
#include <fstream>
#include <sstream>
#include "TableFileReader.h"


void LibraryLoader::loadImport(string importname, string importpath, ClassSpaceSymbolTable& objtable) {
	fstream importfile;
	importfile.open(importpath + "/" + importname + ".table");
	TableFileReader reader;

	PropertySymbolTable* table = objtable.getEmptyPropertySymbolTable();
	reader.read(table, importfile);
	objtable.importClass(table);
}

void LibraryLoader::loadStdLibToTable(ClassSpaceSymbolTable* table) {
	#ifdef COMPILE_IN_PRIMITIVE_TYPES
		#include "Int.table.h"
		#include "Text.table.h"
		#include "Bool.table.h"

		#define LOAD_MEMORY_TABLE_FILE(tname) \
			stringstream importbin ## tname; \
			importbin ## tname.write((const char*)bin_waketable_ ## tname ## _table, bin_waketable_ ## tname ## _table_len); \
			ptable = table->getEmptyPropertySymbolTable(); \
			reader.read(ptable, importbin ## tname); \
			table->importClass(ptable);

		TableFileReader reader;
		PropertySymbolTable* ptable;

		LOAD_MEMORY_TABLE_FILE(Int);
		LOAD_MEMORY_TABLE_FILE(Text);
		LOAD_MEMORY_TABLE_FILE(Bool);

	#endif
}
