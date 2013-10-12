#ifndef HEADER_TYPE_CHECKER
#define HEADER_TYPE_CHECKER

#include <string>
#include <vector>

extern "C" {
	#include "type.h"
	#include "tree.h"
}

#include "ErrorTracker.h"
#include "ObjectSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"

using namespace std;

class TypeChecker {

	public:
		TypeChecker(ErrorTracker* errors, ObjectSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer);
		void setReturnType(Type* returntype);
		void setThisContext(string classname);
		void check(Node* n);

	private:
		Type* typeCheck(Node* n);
		bool exhaustiveReturns(Node* n);
		void flowAnalysis(Node* n, bool breakable, bool caseable, bool continuable);
		bool isValidLValue(Node* N);

		MethodSignatureParseTreeTraverser* methodanalyzer;
		ErrorTracker* errors;
		ObjectSymbolTable* objectsymtable;
		ScopeSymbolTable* scopesymtable;
		Type* returntype;
		string thiscontext;

};

#endif
