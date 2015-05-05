/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MethodSignatureParseTreeTraverser.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "MethodSignatureParseTreeTraverser.h"
#include "SemanticError.h"
#include "TypeParameterizer.h"

void MethodSignatureParseTreeTraverser::convertParameterizedTypes(Node* methoddef, const vector<PureType*>& parameterizedtypes) {
	int i;
	Node* methodname;
	TypeParameterizer parameterizer;

	if(methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE) {
		methodname = methoddef->node_data.nodes[2];
		parameterizer.rewriteClasstypesToParameterizedtypeByLabel(methoddef->node_data.nodes[1]->node_data.nodes[0]->node_data.pure_type, parameterizedtypes);
	} else {
		methodname = methoddef->node_data.nodes[1];
	}

	for(i = 0; i < methodname->subnodes; i++) {
		Node* args;
		i++;

		if(i < methodname->subnodes) {
			args = methodname->node_data.nodes[i];
			for(int b = 0; b < args->subnodes; b++) {
				parameterizer.rewriteClasstypesToParameterizedtypeByLabel(&args->node_data.nodes[b]->node_data.var_decl->typedata, parameterizedtypes);
			}
		}
	}
}

vector<pair<string, PureTypeArray*> >* MethodSignatureParseTreeTraverser::getName(Node* methoddef) {
	vector<pair<string, PureTypeArray*> >* arguments_segments = new vector<pair<string, PureTypeArray*> >();
	int i;

	try {
		Node* methodname = methoddef->node_data.nodes[
			methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE ? 2 : 1
		];

		for(i = 0; i < methodname->subnodes; i++) {
			PureTypeArray* argshere = makePureTypeArray();
			string namesegment = methodname->node_data.nodes[i]->node_data.string;
			i++;

			if(i < methodname->subnodes) {
				Node* args = methodname->node_data.nodes[i];
				for(int b = 0; b < args->subnodes; b++) {
					classestable->assertTypeIsValid(&args->node_data.nodes[b]->node_data.var_decl->typedata);
					addPureTypeToPureTypeArray(new PureType(args->node_data.nodes[b]->node_data.var_decl->typedata), argshere);
				}
			} else {
				argshere = emptytypearray;
			}

			arguments_segments->push_back(pair<string, PureTypeArray*>(namesegment, argshere));
		}

		return arguments_segments;
	} catch(SemanticError* e) {
		delete arguments_segments;
		throw e;
	}
}

Node* MethodSignatureParseTreeTraverser::getBody(Node* methoddef) {
	if(methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE) {
		return methoddef->node_data.nodes[3];
	} else {
		return methoddef->node_data.nodes[2];
	}
}

vector<VarDecl*> MethodSignatureParseTreeTraverser::getArgDecls(Node* methoddef) {
	return vector<VarDecl*>();
}

PureType* MethodSignatureParseTreeTraverser::getReturn(Node* methoddef) {
	PureType* returntype;

	if(methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE) {
		returntype = methoddef->node_data.nodes[1]->node_data.nodes[0]->node_data.pure_type;
		classestable->assertTypeIsValid(returntype);
	} else {
		returntype = NULL;
	}

	return returntype;
}

int MethodSignatureParseTreeTraverser::getFlags(Node* methoddef) {
	int flags = 0;

	if(getBody(methoddef)->node_type == NT_ABSTRACT_METHOD)
		flags |= PROPERTY_ABSTRACT;

	if(methoddef->node_data.nodes[0]->node_type == NT_PUBLIC)
		flags |= PROPERTY_PUBLIC;

	return flags;
}

MethodSignatureParseTreeTraverser::MethodSignatureParseTreeTraverser(ClassSpaceSymbolTable* classestable) {
	this->classestable = classestable;
	emptytypearray = new PureTypeArray();
}

MethodSignatureParseTreeTraverser::~MethodSignatureParseTreeTraverser() {
	delete emptytypearray;
}
