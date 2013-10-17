#ifndef HEADER_METHOD_SIGNATURE_PARSE_TREE_TRAVERSER
#define HEADER_METHOD_SIGNATURE_PARSE_TREE_TRAVERSER

#include <utility>
#include <string>
#include <vector>

extern "C" {
	#include "type.h"
	#include "tree.h"
}

#include "ObjectSymbolTable.h"

using namespace std;

class MethodSignatureParseTreeTraverser {

	public:
		MethodSignatureParseTreeTraverser(ObjectSymbolTable* objectsymtable);
		~MethodSignatureParseTreeTraverser();

		vector<pair<string, TypeArray*> >* getName(Node* methoddef);
		Node* getBody(Node* methoddef);
		Type* getReturn(Node* methoddef);
		int getFlags(Node* methoddef);

	private:
		ObjectSymbolTable* objectsymtable;
		TypeArray* emptytypearray;

};

#endif
