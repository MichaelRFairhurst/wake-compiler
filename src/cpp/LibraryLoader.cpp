/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * LibraryLoader.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "LibraryLoader.h"
#include <fstream>
#include <sstream>
#include "TableFileReader.h"

bool LibraryLoader::loadImport(string importname, string importpath, ClassSpaceSymbolTable& objtable) {
	fstream importfile;
	importfile.open((importpath + "/" + importname + ".table").c_str(), fstream::binary | fstream::in);
	if(!importfile.is_open()) {
		return false;
	}
	TableFileReader reader;

	PropertySymbolTable* table = objtable.getEmptyPropertySymbolTable();
	reader.read(table, importfile);
	objtable.importClass(table);
	return true;
}

void LibraryLoader::loadLangModule(ClassSpaceSymbolTable* table) {
	#ifdef COMPILE_IN_PRIMITIVE_TYPES
		#include "Num.table.h"
		#include "Int.table.h"
		#include "Text.table.h"
		#include "Bool.table.h"
		#include "List.table.h"
		#include "Exception.table.h"
		#include "Char.table.h"

		#define LOAD_MEMORY_TABLE_FILE(tname) \
			stringstream importbin ## tname; \
			importbin ## tname.write((const char*)bin_waketable_lang_ ## tname ## _table, bin_waketable_lang_ ## tname ## _table_len); \
			ptable = table->getEmptyPropertySymbolTable(); \
			reader.read(ptable, importbin ## tname); \
			table->importClass(ptable);

		TableFileReader reader;
		PropertySymbolTable* ptable;

		LOAD_MEMORY_TABLE_FILE(Num);
		LOAD_MEMORY_TABLE_FILE(Int);
		LOAD_MEMORY_TABLE_FILE(Text);
		LOAD_MEMORY_TABLE_FILE(Bool);
		LOAD_MEMORY_TABLE_FILE(List);
		LOAD_MEMORY_TABLE_FILE(Exception);
		LOAD_MEMORY_TABLE_FILE(Char);

	#endif
}

void LibraryLoader::prepLangModule(ClassSpaceSymbolTable* table) {
	#ifdef COMPILE_IN_PRIMITIVE_TYPES
	table->prepImport("lang", "Int");
	table->prepImport("lang", "Num");
	table->prepImport("lang", "Text");
	table->prepImport("lang", "Bool");
	table->prepImport("lang", "List");
	table->prepImport("lang", "Char");
	table->prepImport("lang", "Exception");
	#endif
}
