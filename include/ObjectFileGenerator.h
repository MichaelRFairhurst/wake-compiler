#ifndef HEADER_CODE_GENERATOR
#define HEADER_CODE_GENERATOR

#include <fstream>

extern "C" {
	#include "tree.h"
	#include "type.h"
}

#include "ScopeSymbolTable.h"
#include "ObjectSymbolTable.h"

using namespace std;

class ObjectFileGenerator {

	public:
		ObjectFileGenerator(fstream& file, ObjectSymbolTable* objects) : file(file) {this->objects = objects;}
		~ObjectFileGenerator();
		void generate(Node* tree);
		void generateRecursiveConstructors(string ctorname);
		void setMain(string classname, string methodname);

	private:
		ObjectSymbolTable* objects;
		ScopeSymbolTable table;
		fstream& file;
		string classname;

};

#endif
