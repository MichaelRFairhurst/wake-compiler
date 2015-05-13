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

#include "tree.h"

#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"

using namespace std;

class TypeChecker {

	public:
		TypeChecker(ErrorTracker* errors, ClassSpaceSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer);
		void setReturnType(PureType<wake::QUALIFIED>* returntype);
		void setThisContext(PureType<wake::QUALIFIED>* thiscontext);
		void check(Node* n);
		void setParameterizedTypes(const vector<PureType<wake::QUALIFIED>*>& types);

	private:
		void flowAnalysis(Node* n, bool breakable, bool caseable, bool continuable);

		MethodSignatureParseTreeTraverser* methodanalyzer;
		ErrorTracker* errors;
		ClassSpaceSymbolTable* classestable;
		ScopeSymbolTable* scopesymtable;
		PureType<wake::QUALIFIED>* returntype;
		PureType<wake::QUALIFIED>* thiscontext;
		vector<PureType<wake::QUALIFIED>*> parameterizedtypes;

};

#endif
