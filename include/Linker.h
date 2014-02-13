#ifndef HEADER_LINKER
#define HEADER_LINKER

#include <string>
#include <vector>
#include <fstream>

#include "PropertySymbolTable.h"

extern "C" {
	#include "objectfile.h"
}

class Linker {

	public:
		Linker(PropertySymbolTable symtable) : symtable(symtable) {}

		void loadObject(std::string filename);
		void write(std::fstream& objectfile);

	private:
		PropertySymbolTable& symtable;
		objectfile** objectFiles;
		int objectFileCount;
		std::vector<std::string> files;


};

#endif
