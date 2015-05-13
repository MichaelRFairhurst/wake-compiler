/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ClassParseTreeTraverser.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_CLASS_PARSE_TREE_TRAVERSER
#define HEADER_CLASS_PARSE_TREE_TRAVERSER

#include <string>
#include <vector>
#include <utility>
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "SemanticError.h"
#include "ErrorTracker.h"
#include "TypeChecker.h"
#include "MethodSignatureParseTreeTraverser.h"

extern "C" {
	#include "node.h"
	#include "tree.h"
}

using namespace std;

class ClassParseTreeTraverser {
	public:
		ClassParseTreeTraverser(ErrorTracker* errors, ClassSpaceSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, string classname, TypeChecker* typechecker, MethodSignatureParseTreeTraverser* methodanalyzer, PropertySymbolTable* propertysymtable, bool is_extern);
		void firstPass(Node* tree);
		void secondPass(Node* tree);

	private:
		void checkCtorArgs(Node* tree);
		void typeCheckProperties(Node* tree);
		void typeCheckMethods(Node* tree);
		Node* getMethodBody(Node* methoddef);

		string classname;

		bool is_extern;
		MethodSignatureParseTreeTraverser* methodanalyzer;
		TypeChecker* typechecker;
		ErrorTracker* errors;
		ScopeSymbolTable* scopesymtable;
		ClassSpaceSymbolTable* classestable;
		PropertySymbolTable* propertysymtable;
};

#endif
