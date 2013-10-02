#include "MethodSignatureParseTreeTraverser.h"
#include "SemanticError.h"

vector<pair<string, TypeArray*> >* MethodSignatureParseTreeTraverser::getName(Node* methoddef) {
	vector<pair<string, TypeArray*> >* arguments_segments = new vector<pair<string, TypeArray*> >();
	int i;

	try {
		Node* methodname = methoddef->node_data.nodes[
			methoddef->node_data.nodes[0]->node_type == NT_METHOD_RETURN_TYPE ? 1 : 0
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
	if(methoddef->node_data.nodes[0]->node_type == NT_METHOD_RETURN_TYPE) {
		return methoddef->node_data.nodes[2];
	} else {
		return methoddef->node_data.nodes[1];
	}
}

Type* MethodSignatureParseTreeTraverser::getReturn(Node* methoddef) {
	Type* returntype;

	if(methoddef->node_data.nodes[0]->node_type == NT_METHOD_RETURN_TYPE) {
		returntype = methoddef->node_data.nodes[0]->node_data.nodes[0]->node_data.type;
		objectsymtable->assertTypeIsValid(returntype);
	} else {
		returntype = NULL;
	}

	return returntype;
}

MethodSignatureParseTreeTraverser::MethodSignatureParseTreeTraverser(ObjectSymbolTable* objectsymtable) {
	this->objectsymtable = objectsymtable;
	emptytypearray = MakeTypeArray();
}

MethodSignatureParseTreeTraverser::~MethodSignatureParseTreeTraverser() {
	freeTypeArray(emptytypearray);
}
