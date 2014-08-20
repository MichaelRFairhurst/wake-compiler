/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ParseTreeTraverser.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_PARSE_TREE_TRAVERSER
#define HEADER_PARSE_TREE_TRAVERSER

#include <string>
#include <vector>
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "SemanticErrorPrinter.h"
#include "ErrorTracker.h"
#include "TypeChecker.h"

extern "C" {
	#include "type.h"
	#include "node.h"
	#include "tree.h"
}

using namespace std;

class ParseTreeTraverser {

	public:
		ParseTreeTraverser(ClassSpaceSymbolTable* table, ErrorTracker& errors) : errors(errors), methodanalyzer(table), typechecker(&errors, table, &scopesymtable, &methodanalyzer) { objectsymtable = table; }
		void traverse(Node* tree);
		void printErrors(SemanticErrorPrinter& p);
		bool passesForCompilation();

	private:
		void secondPass(Node* tree);
		void thirdPass(Node* tree);
		ErrorTracker& errors;

		ScopeSymbolTable scopesymtable;
		ClassSpaceSymbolTable* objectsymtable;
		MethodSignatureParseTreeTraverser methodanalyzer;
		TypeChecker typechecker;

};

#endif
