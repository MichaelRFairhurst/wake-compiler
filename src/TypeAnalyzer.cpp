#include "TypeAnalyzer.h"
#include "ObjectSymbolTable.h"
#include "PropertySymbolTable.h"
#include "CompilationExceptions.h"

bool TypeAnalyzer::isASubtypeOfB(string a, string b) {
	try {
		if(a == b) return true;

		PropertySymbolTable* a_data = reference->find(a);

		for(map<string, bool>::iterator it = a_data->parentage.begin(); it != a_data->parentage.end(); ++it) {
			if(isASubtypeOfB(it->first, b)) return true;
		}

	} catch(SymbolNotFoundException* e) {
		delete e;
	}

	return false;
}

bool TypeAnalyzer::isASubtypeOfB(Type* a, Type* b) {
	if(a->arrayed != b->arrayed) return false;
	if(a->type != b->type) return false;

	if(a->type == TYPE_LAMBDA) {

		// A fn taking 3 arguments is not a subtype of a fn taking 2 or 4
		if(a->typedata.lambda.arguments->typecount != b->typedata.lambda.arguments->typecount)
			return false;

		// Truth -- fn() is a subtype of void -- fn(), since the subtype will simply ignore the returnval
		// however, void --fn() is not a subtype of Truth -- fn() as you probably guessed
		if(a->typedata.lambda.returntype == NULL && b->typedata.lambda.returntype != NULL)
			return false;
		else if(b->typedata.lambda.returntype != NULL && !isASubtypeOfB(a->typedata.lambda.returntype, b->typedata.lambda.returntype))
			return false;

		int i;
		for(i = 0; i < a->typedata.lambda.arguments->typecount; i++)
		if(!isASubtypeOfB(a->typedata.lambda.arguments->types[i], b->typedata.lambda.arguments->types[i]))
			return false;

		return true;

	} else {
		if(string(a->typedata._class.classname) == b->typedata._class.classname) return true;

		try {

			PropertySymbolTable* a_data = reference->find(a->typedata._class.classname);

			for(map<string, bool>::iterator it = a_data->parentage.begin(); it != a_data->parentage.end(); ++it) {
				if(isASubtypeOfB(it->first, b->typedata._class.classname)) return true;
			}

		} catch(SymbolNotFoundException* e) {
			delete e;
		}

		return false;
	}

}

Type* TypeAnalyzer::getCommonSubtypeOf(Type* a, Type* b) {
	return a;
}

bool TypeAnalyzer::isPrimitiveTypeInt(Type* type) {
	return type->type != TYPE_LAMBDA && type->typedata._class.classname == string("Int") && type->arrayed == 0;
}

bool TypeAnalyzer::isPrimitiveTypeText(Type* type) {
	return type->type != TYPE_LAMBDA && type->typedata._class.classname == string("Text") && type->arrayed == 0;
}

bool TypeAnalyzer::isPrimitiveTypeTruth(Type* type) {
	return type->type != TYPE_LAMBDA && type->typedata._class.classname == string("Truth") && type->arrayed == 0;
}

string TypeAnalyzer::getNameForType(Type* type) {
	string name;

	if(type == NULL) {
		name = "VOID";
		return name;
	}

	if(type->type == TYPE_CLASS) {
		name = type->typedata._class.classname;
	} else {
		name = getNameForType(type->typedata.lambda.returntype);
		name += "--(";

		if(type->typedata.lambda.arguments != NULL) {
			int i;
			for(i = 0; i < type->typedata.lambda.arguments->typecount; i++) {
				if(i) name += ",";
				name += getNameForType(type->typedata.lambda.arguments->types[i]);
			}
		}
		name += ")";
	}

	int i;
	for(i = 0; i < type->arrayed; i++)
		name += "[]";

	return name;
}
