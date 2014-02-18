#ifndef HEADER_LINKER
#define HEADER_LINKER

#include <string>
#include <vector>
#include <iostream>

#include "SimpleAddressTable.h"
#include "ObjectFileHeaderData.h"

extern "C" {
	#include "objectfile.h"
}

class Linker {

	public:
		Linker(SimpleAddressTable& classTable, SimpleAddressTable& propertyTable) : classTable(classTable), propertyTable(propertyTable) {};
		void loadObject(std::string filename);
		void write(std::ostream& objectfile);
		void setMain(string classname, string methodname);
		void generateRecursiveConstructors(string ctedclass);

	private:
		std::vector<ObjectFileHeaderData*> files;
		SimpleAddressTable& classTable;
		SimpleAddressTable& propertyTable;


};

#endif
