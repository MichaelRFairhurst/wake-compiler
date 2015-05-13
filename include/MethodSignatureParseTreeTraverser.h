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

		vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* getName(Node* methoddef);
		Node* getBody(Node* methoddef);
		vector<VarDecl<wake::QUALIFIED>*> getArgDecls(Node* methoddef);
		PureType<wake::QUALIFIED>* getReturn(Node* methoddef);
		void convertParameterizedTypes(Node* methoddef, const vector<PureType<wake::QUALIFIED>*>& parameterizedtypes);
		int getFlags(Node* methoddef);

	private:
		ClassSpaceSymbolTable* classestable;
		PureTypeArray<wake::QUALIFIED>* emptytypearray;

};

#endif
