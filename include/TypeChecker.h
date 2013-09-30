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

using namespace std;

class TypeChecker {

	public:
		TypeChecker(ErrorTracker* errors, ObjectSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable);
		void setReturnType(Type* returntype);
		void check(Node* n);

	private:
		Type* typeCheck(Node* n);
		bool exhaustiveReturns(Node* n);

		ErrorTracker* errors;
		ObjectSymbolTable* objectsymtable;
		ScopeSymbolTable* scopesymtable;
		Type* returntype;

};

#endif
