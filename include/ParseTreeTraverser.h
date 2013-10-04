#ifndef HEADER_PARSE_TREE_TRAVERSER
#define HEADER_PARSE_TREE_TRAVERSER

#include <string>
#include <vector>
#include "ObjectSymbolTable.h"
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
		ParseTreeTraverser(ObjectSymbolTable* table) : methodanalyzer(table), typechecker(&errors, table, &scopesymtable, &methodanalyzer) { objectsymtable = table; }
		void traverse(Node* tree);
		void printErrors(SemanticErrorPrinter& p);

	private:
		void secondPass(Node* tree);
		void thirdPass(Node* tree);
		ErrorTracker errors;

		ScopeSymbolTable scopesymtable;
		ObjectSymbolTable* objectsymtable;
		MethodSignatureParseTreeTraverser methodanalyzer;
		TypeChecker typechecker;

};

#endif
