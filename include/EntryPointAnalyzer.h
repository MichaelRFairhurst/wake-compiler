/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * EntryPointAnalyzer.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_ENTRY_POINT_ANALYZER
#define HEADER_ENTRY_POINT_ANALYZER

#include <string>
#include <vector>

extern "C" {
	#include "type.h"
}

using namespace std;

class ClassSpaceSymbolTable;

class EntryPointAnalyzer {

	public:
		bool checkClassNeedsCanBeMain(vector<Type*>* needs);
		bool checkMethodCanBeMain(string methodname, Type* method);
		bool checkMethodCanBeMain(string classname, string methodname, ClassSpaceSymbolTable* table);
		bool printClass(string classname);
		bool printMethod(string methodname);

};

#endif
