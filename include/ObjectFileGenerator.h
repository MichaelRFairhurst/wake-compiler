#ifndef HEADER_CODE_GENERATOR
#define HEADER_CODE_GENERATOR

#include <iostream>

extern "C" {
	#include "tree.h"
	#include "type.h"
}

#include "ScopeSymbolTable.h"
#include "ObjectSymbolTable.h"
#include "ObjectFileHeaderData.h"
#include "TypeAnalyzer.h"

using namespace std;

class ObjectFileGenerator {

	public:
		ObjectFileGenerator(ostream& file, ObjectSymbolTable* objects, ObjectFileHeaderData* header) : file(file) {this->objects = objects; this->header = header;}
		void generate(Node* tree);
		void generateRecursiveConstructors(string ctorname);
		void setMain(string classname, string methodname);

	private:
		TypeAnalyzer typeanalyzer;
		ObjectFileHeaderData* header;
		ObjectSymbolTable* objects;
		ScopeSymbolTable table;
		ostream& file;
		string classname;

};

#endif
