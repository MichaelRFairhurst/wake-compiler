#include "MethodSignatureParseTreeTraverser.h"
#include "SemanticError.h"
#include "TypeParameterizer.h"

void MethodSignatureParseTreeTraverser::convertParameterizedTypes(Node* methoddef, vector<Type*> parameterizedtypes) {
	int i;
	Node* methodname;
	TypeParameterizer parameterizer;

	if(methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE) {
		methodname = methoddef->node_data.nodes[2];
		parameterizer.applyParameterizations(&methoddef->node_data.nodes[1]->node_data.nodes[0]->node_data.type, parameterizedtypes);
	} else {
		methodname = methoddef->node_data.nodes[1];
	}

	for(i = 0; i < methodname->subnodes; i++) {
		TypeArray* args;
		i++;

		if(i < methodname->subnodes) {
			args = methodname->node_data.nodes[i]->node_data.typearray;
			for(int b = 0; b < args->typecount; b++) {
				parameterizer.applyParameterizations(&args->types[b], parameterizedtypes);
			}
		}
	}
}

vector<pair<string, TypeArray*> >* MethodSignatureParseTreeTraverser::getName(Node* methoddef) {
	vector<pair<string, TypeArray*> >* arguments_segments = new vector<pair<string, TypeArray*> >();
	int i;

	try {
		Node* methodname = methoddef->node_data.nodes[
			methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE ? 2 : 1
		];

		for(i = 0; i < methodname->subnodes; i++) {
			TypeArray* argshere;
			string namesegment = methodname->node_data.nodes[i]->node_data.string;
			i++;

			if(i < methodname->subnodes) {
				argshere = methodname->node_data.nodes[i]->node_data.typearray;
				for(int b = 0; b < argshere->typecount; b++) {
					objectsymtable->assertTypeIsValid(argshere->types[b]);
				}
			} else {
				argshere = emptytypearray;
			}

			arguments_segments->push_back(pair<string, TypeArray*>(namesegment, argshere));
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

Type* MethodSignatureParseTreeTraverser::getReturn(Node* methoddef) {
	Type* returntype;

	if(methoddef->node_data.nodes[1]->node_type == NT_METHOD_RETURN_TYPE) {
		returntype = methoddef->node_data.nodes[1]->node_data.nodes[0]->node_data.type;
		objectsymtable->assertTypeIsValid(returntype);
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

MethodSignatureParseTreeTraverser::MethodSignatureParseTreeTraverser(ObjectSymbolTable* objectsymtable) {
	this->objectsymtable = objectsymtable;
	emptytypearray = MakeTypeArray();
}

MethodSignatureParseTreeTraverser::~MethodSignatureParseTreeTraverser() {
	freeTypeArray(emptytypearray);
}
