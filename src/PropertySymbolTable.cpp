#include "PropertySymbolTable.h"
#include "SemanticError.h"

string PropertySymbolTable::addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, Node* body) {
	string name = getSymbolNameOf(returntype, segments_arguments);
	Functor* f = new Functor();

	if(properties.count(name)) {
		string temp = "duplicate method signature is " + name;
		throw new SemanticError(MULTIPLE_METHOD_DEFINITION, temp);
	}

	f->returntype = returntype;
	f->body = body;

	TypeArray* conglomerate = MakeTypeArray();
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		int i;
		for(i = 0; i < it->second->typecount; i++)
			AddTypeToTypeArray(it->second->types[i], conglomerate);
	}

	f->arguments = conglomerate;

	properties[name] = f;

	return name;
}

string PropertySymbolTable::getSymbolNameOf(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments) {
	string name = getSymbolNameOf(returntype) + "--";
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		name += it->first;
		name += "(";
		int i;
		for(i = 0; i < it->second->typecount; i++) {
			if(i) name += ",";
			name += getSymbolNameOf(it->second->types[i]);
		}
		name += ")";
	}

	return name;
}

string PropertySymbolTable::getSymbolNameOf(Type* type) {
	string name;

	if(type == NULL) {
		name = "VOID";
		return name;
	}

	if(type->type == TYPE_CLASS) {
		name = type->typedata._class.classname;
	} else {
		name = getSymbolNameOf(type->typedata.lambda.returntype);
		name += "--(";
		if(type->typedata.lambda.arguments != NULL) {
			int i;
			for(i = 0; i < type->typedata.lambda.arguments->typecount; i++) {
				if(i) name += ",";
				name += getSymbolNameOf(type->typedata.lambda.arguments->types[i]);
			}
		}
		name += ")";
	}

	return name;
}
