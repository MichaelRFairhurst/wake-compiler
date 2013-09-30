#include "PropertySymbolTable.h"
#include "SemanticError.h"

void PropertySymbolTable::addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, Node* body) {
	string name = getSymbolNameOf(returntype, segments_arguments);
	Type* method = MakeType(TYPE_LAMBDA);

	if(properties.count(name)) {
		string temp = "duplicate method signature is " + name;
		freeType(method);
		throw new SemanticError(MULTIPLE_METHOD_DEFINITION, temp);
	}

	method->typedata.lambda.returntype = copyType(returntype);
	method->typedata.lambda.body = body;

	TypeArray* conglomerate = MakeTypeArray();
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		int i;
		for(i = 0; i < it->second->typecount; i++)
			AddTypeToTypeArray(copyType(it->second->types[i]), conglomerate);
	}

	method->typedata.lambda.arguments = conglomerate;

	properties[name] = method;
}

Type* PropertySymbolTable::get(string name) {
	if(!properties.count(name))
		throw new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Cannot find method with signature " + name);

	return properties.find(name)->second;
}

string PropertySymbolTable::getSymbolNameOf(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments) {
	string name = analyzer->getNameForType(returntype) + "--";
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		name += it->first;
		name += "(";
		int i;
		for(i = 0; i < it->second->typecount; i++) {
			if(i) name += ",";
			name += analyzer->getNameForType(it->second->types[i]);
		}
		name += ")";
	}

	return name;
}

PropertySymbolTable::~PropertySymbolTable() {
	for(map<string, Type*>::iterator it = properties.begin(); it != properties.end(); ++it) {
		freeType(it->second);
	}
}
