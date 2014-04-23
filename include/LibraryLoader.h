#ifndef HEADER_LIBRARY_LOADER
#define HEADER_LIBRARY_LOADER

#include "ClassSpaceSymbolTable.h"
#include "PropertySymbolTable.h"

extern "C" {
	#include "type.h"
}

class LibraryLoader {

	public:
		void loadImport(string importname, string importpath, ClassSpaceSymbolTable& objtable);
		void loadStdLibToTable(ClassSpaceSymbolTable* table);

};

#endif
