/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MethodSignatureParseTreeTraverser.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_METHOD_SIGNATURE_PARSE_TREE_TRAVERSER
#define HEADER_METHOD_SIGNATURE_PARSE_TREE_TRAVERSER

#include <utility>
#include <string>
#include <vector>

extern "C" {
	#include "type.h"
	#include "tree.h"
}

#include "ClassSpaceSymbolTable.h"

using namespace std;

class MethodSignatureParseTreeTraverser {

	public:
		MethodSignatureParseTreeTraverser(ClassSpaceSymbolTable* objectsymtable);
		~MethodSignatureParseTreeTraverser();

		vector<pair<string, TypeArray*> >* getName(Node* methoddef);
		Node* getBody(Node* methoddef);
		Type* getReturn(Node* methoddef);
		void convertParameterizedTypes(Node* methoddef, const vector<Type*>& parameterizedtypes);
		int getFlags(Node* methoddef);

	private:
		ClassSpaceSymbolTable* classestable;
		TypeArray* emptytypearray;

};

#endif
