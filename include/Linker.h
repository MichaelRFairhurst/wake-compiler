#ifndef HEADER_LINKER
#define HEADER_LINKER

#include <string>
#include <vector>
#include <iostream>

#include "SimpleAddressTable.h"
#include "ObjectFileHeaderData.h"
#include "PropertySymbolTable.h"
#include "ClassSpaceSymbolTable.h"

extern "C" {
	#include "objectfile.h"
}

class Linker {

	public:
		Linker(SimpleAddressTable& classTable, SimpleAddressTable& propertyTable) : classTable(classTable), propertyTable(propertyTable) {};
		void loadTables(string dirname, ClassSpaceSymbolTable& table);
		void loadObject(std::string filename);
		void write(std::ostream& objectfile);
		void setMain(ostream& outfile, string classname, string methodname, ClassSpaceSymbolTable& table);
		void writeDebugSymbols(ostream& outfile);
		void generateRecursiveConstructors(ostream& outfile, string ctedclass, ClassSpaceSymbolTable& table);

	private:
		std::vector<ObjectFileHeaderData*> files;
		SimpleAddressTable& classTable;
		SimpleAddressTable& propertyTable;


};

#endif
