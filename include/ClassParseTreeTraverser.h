#ifndef HEADER_CLASS_PARSE_TREE_TRAVERSER
#define HEADER_CLASS_PARSE_TREE_TRAVERSER

#include <string>
#include <vector>
#include <utility>
#include "ObjectSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "SemanticError.h"
#include "ErrorTracker.h"
#include "TypeChecker.h"
#include "MethodSignatureParseTreeTraverser.h"

extern "C" {
	#include "type.h"
	#include "node.h"
	#include "tree.h"
}

using namespace std;

class ClassParseTreeTraverser {
	public:
		ClassParseTreeTraverser(ErrorTracker* errors, ObjectSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, string classname, TypeChecker* typechecker, MethodSignatureParseTreeTraverser* methodanalyzer);
		void traverse(Node* tree);
		void loadCtorArgs(Node* tree);
		void loadProperties(Node* tree);
		void typeCheckMethods(Node* tree);

	private:
		Node* getMethodBody(Node* methoddef);
		Type* getMethodReturn(Node* methoddef);
		vector<pair<string, TypeArray*> >* getMethodName(Node* methoddef);

		string classname;

		MethodSignatureParseTreeTraverser* methodanalyzer;
		TypeChecker* typechecker;
		ErrorTracker* errors;
		ScopeSymbolTable* scopesymtable;
		ObjectSymbolTable* objectsymtable;
		PropertySymbolTable* propertysymtable;
};

#endif
