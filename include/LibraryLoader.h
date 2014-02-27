#ifndef HEADER_LIBRARY_LOADER
#define HEADER_LIBRARY_LOADER

#include "ObjectSymbolTable.h"
#include "PropertySymbolTable.h"

extern "C" {
	#include "type.h"
}

class LibraryLoader {

	public:
		void loadImport(string importname, string importpath, ObjectSymbolTable& objtable);
		void loadStdLibToTable(ObjectSymbolTable* table);

};

#endif
