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

	#include "tree.h"

#include "ClassSpaceSymbolTable.h"

using namespace std;

class MethodSignatureParseTreeTraverser {

	public:
		MethodSignatureParseTreeTraverser(ClassSpaceSymbolTable* objectsymtable);
		~MethodSignatureParseTreeTraverser();

		vector<pair<string, PureTypeArray*> >* getName(Node* methoddef);
		Node* getBody(Node* methoddef);
		vector<VarDecl*> getArgDecls(Node* methoddef);
		PureType* getReturn(Node* methoddef);
		void convertParameterizedTypes(Node* methoddef, const vector<PureType*>& parameterizedtypes);
		int getFlags(Node* methoddef);

	private:
		ClassSpaceSymbolTable* classestable;
		PureTypeArray* emptytypearray;

};

#endif
