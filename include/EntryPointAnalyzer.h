#ifndef HEADER_ENTRY_POINT_ANALYZER
#define HEADER_ENTRY_POINT_ANALYZER

#include <string>
#include <vector>

extern "C" {
	#include "type.h"
}

using namespace std;

class ObjectSymbolTable;

class EntryPointAnalyzer {

	public:
		bool checkClassNeedsCanBeMain(vector<Type*>* needs);
		bool checkMethodCanBeMain(string methodname, Type* method);
		bool checkMethodCanBeMain(string classname, string methodname, ObjectSymbolTable* table);
		bool printClass(string classname);
		bool printMethod(string methodname);

};

#endif
