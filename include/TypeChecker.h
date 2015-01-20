/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeChecker.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TYPE_CHECKER
#define HEADER_TYPE_CHECKER

#include <string>
#include <vector>

extern "C" {
	#include "type.h"
	#include "tree.h"
}

#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"

using namespace std;

class TypeChecker {

	public:
		TypeChecker(ErrorTracker* errors, ClassSpaceSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer);
		void setReturnType(Type* returntype);
		void setThisContext(Type* thiscontext);
		void check(Node* n);
		void setParameterizedTypes(const vector<Type*>& types);

	private:
		bool exhaustiveReturns(Node* n);
		void flowAnalysis(Node* n, bool breakable, bool caseable, bool continuable);

		MethodSignatureParseTreeTraverser* methodanalyzer;
		ErrorTracker* errors;
		ClassSpaceSymbolTable* classestable;
		ScopeSymbolTable* scopesymtable;
		Type* returntype;
		Type* thiscontext;
		vector<Type*> parameterizedtypes;

};

#endif
