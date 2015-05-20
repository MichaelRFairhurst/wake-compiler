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
#include "SpecializableVarDecl.h"

using namespace std;

class ClassSpaceSymbolTable;

class EntryPointAnalyzer {

	public:
		bool checkClassNeedsCanBeMain(vector<SpecializableVarDecl<wake::QUALIFIED>*>* needs);
		bool checkFQClassMethodCanBeMain(string methodname, PureType<wake::QUALIFIED>* method);
		bool checkFQClassMethodCanBeMain(string classname, string methodname, ClassSpaceSymbolTable* table);
		bool printClass(string classname);
		bool printMethod(string methodname);

};

#endif
