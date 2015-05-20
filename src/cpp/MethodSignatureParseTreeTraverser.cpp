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

using namespace wake;

void MethodSignatureParseTreeTraverser::convertParameterizedTypes(Node* methoddef, const vector<PureType<QUALIFIED>*>& parameterizedtypes) {
	int i;
	Node* methodname;
	TypeParameterizer parameterizer;

	if(methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE) {
		methodname = methoddef->node_data.nodes[2];
		// we have to cast but thats fine, these templates affect the typechecker only
		parameterizer.rewriteClasstypesToParameterizedtypeByLabel((PureType<QUALIFIED>*) methoddef->node_data.nodes[1]->node_data.nodes[0]->node_data.pure_type, parameterizedtypes);
	} else {
		methodname = methoddef->node_data.nodes[1];
	}

	for(i = 0; i < methodname->subnodes; i++) {
		Node* args;
		i++;

		if(i < methodname->subnodes) {
			args = methodname->node_data.nodes[i];
			for(int b = 0; b < args->subnodes; b++) {
				// we have to cast but thats fine, these templates affect the typechecker only
				parameterizer.rewriteClasstypesToParameterizedtypeByLabel((PureType<QUALIFIED>*) &args->node_data.nodes[b]->node_data.var_decl->typedata, parameterizedtypes);
			}
		}
	}
}

vector<pair<string, PureTypeArray<QUALIFIED>*> >* MethodSignatureParseTreeTraverser::getName(Node* methoddef) {
	vector<pair<string, PureTypeArray<QUALIFIED>*> >* arguments_segments = new vector<pair<string, PureTypeArray<QUALIFIED>*> >();
	int i;

	try {
		Node* methodname = methoddef->node_data.nodes[
			methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE ? 2 : 1
		];

		for(i = 0; i < methodname->subnodes; i++) {
			PureTypeArray<QUALIFIED>* argshere = new PureTypeArray<QUALIFIED>();
			string namesegment = methodname->node_data.nodes[i]->node_data.string;
			i++;

			if(i < methodname->subnodes) {
				Node* args = methodname->node_data.nodes[i];
				for(int b = 0; b < args->subnodes; b++) {
					PureType<QUALIFIED>* argType = classestable->setModulesOnType(&args->node_data.nodes[b]->node_data.var_decl->typedata);
					classestable->assertTypeIsValid(argType);
					// we have to cast which is OK because the templating here is purely for the type system
					addPureTypeToPureTypeArray(
						(PureType<UNQUALIFIED>*) new PureType<QUALIFIED>(*argType),
						(PureTypeArray<UNQUALIFIED>*) argshere
					);
				}
			} else {
				argshere = emptytypearray;
			}

			arguments_segments->push_back(pair<string, PureTypeArray<QUALIFIED>*>(namesegment, argshere));
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

vector<VarDecl<QUALIFIED>*> MethodSignatureParseTreeTraverser::getArgDecls(Node* methoddef) {
	vector<VarDecl<QUALIFIED>*> args;
	Node* methodname;

	if(methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE) {
		methodname = methoddef->node_data.nodes[2];
	} else {
		methodname = methoddef->node_data.nodes[1];
	}

	for(int i = 1; i < methodname->subnodes; i += 2) {
		for(int b = 0; b < methodname->node_data.nodes[i]->subnodes; b++) {
			args.push_back(classestable->setModulesOnType(methodname->node_data.nodes[i]->node_data.nodes[b]->node_data.var_decl));
		}
	}

	return args;
}

PureType<QUALIFIED>* MethodSignatureParseTreeTraverser::getReturn(Node* methoddef) {
	PureType<QUALIFIED>* returntype;

	if(methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE) {
		returntype = classestable->setModulesOnType(methoddef->node_data.nodes[1]->node_data.nodes[0]->node_data.pure_type);
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
	emptytypearray = new PureTypeArray<QUALIFIED>();
}

MethodSignatureParseTreeTraverser::~MethodSignatureParseTreeTraverser() {
	delete emptytypearray;
}
