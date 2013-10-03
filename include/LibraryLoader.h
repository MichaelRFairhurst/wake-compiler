#ifndef HEADER_LIBRARY_LOADER
#define HEADER_LIBRARY_LOADER

#include "ObjectSymbolTable.h"
#include "PropertySymbolTable.h"

extern "C" {
	#include "type.h"
}

class LibraryLoader {

	public:
		void loadToTable(ObjectSymbolTable* table);

	private:
		void loadStdLibToTable(ObjectSymbolTable* table);

};

#endif
