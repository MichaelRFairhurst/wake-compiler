#ifndef HEADER_LINKER
#define HEADER_LINKER

#include <string>
#include <vector>
#include <iostream>
#include <map>

#include "PropertySymbolTable.h"

extern "C" {
	#include "objectfile.h"
}

class Linker {

	public:
		void loadObject(std::string filename);
		void write(std::ostream& objectfile);

	private:
		std::vector<objectfile*> files;

};

#endif
