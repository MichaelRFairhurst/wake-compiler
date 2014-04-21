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
	//if(a == NULL || b == NULL) return false;
	if(a->type == TYPE_MATCHALL || b->type == TYPE_MATCHALL) return true;
	if(a->type == TYPE_NOTHING) return b->optional;
	if(a->arrayed != b->arrayed) return false;
	if(a->type != b->type) return false; // TODO: parameterized types don't always follow this logic!

	if(a->type == TYPE_LAMBDA) {

		// A fn taking 3 arguments is not a subtype of a fn taking 2 or 4
		if(a->typedata.lambda.arguments->typecount != b->typedata.lambda.arguments->typecount)
			return false;

		// Bool -- fn() is a subtype of void -- fn(), since the subtype will simply ignore the returnval
		// however, void --fn() is not a subtype of Bool -- fn() as you probably guessed
		if(a->typedata.lambda.returntype == NULL && b->typedata.lambda.returntype != NULL)
			return false;
		else if(b->typedata.lambda.returntype != NULL && !isASubtypeOfB(a->typedata.lambda.returntype, b->typedata.lambda.returntype))
			return false;

		int i;
		for(i = 0; i < a->typedata.lambda.arguments->typecount; i++)
		if(!isASubtypeOfB(a->typedata.lambda.arguments->types[i], b->typedata.lambda.arguments->types[i]))
			return false;

		return true;

	} else if(a->type == TYPE_CLASS) {
		if(string(a->typedata._class.classname) == b->typedata._class.classname) {
			return a->optional <= b->optional;
		}

		try {

			PropertySymbolTable* a_data = reference->find(a->typedata._class.classname);

			for(map<string, bool>::iterator it = a_data->parentage.begin(); it != a_data->parentage.end(); ++it) {
				if(isASubtypeOfB(it->first, b->typedata._class.classname)) return true;
			}

		} catch(SymbolNotFoundException* e) {
			delete e;
		}

		return false;
	} else if(a->type == TYPE_PARAMETERIZED) {
		if(a->typedata.parameterized.label == string(b->typedata.parameterized.label)) return true;
		// TODO: lower/upper bounds comparison
		return false;
	}

}

void TypeAnalyzer::assertNeedIsNotCircular(string classname, Type* need) {
	if(need->typedata._class.classname == classname)
		throw new SemanticError(CIRCULAR_DEPENDENCIES, "Created by the need for class " + getNameForType(need));

	vector<Type*>* recurse = reference->find(need->typedata._class.classname)->getNeeds();
	for(vector<Type*>::iterator it = recurse->begin(); it != recurse->end(); ++it)
	try {
		assertNeedIsNotCircular(classname, *it);
	} catch(SemanticError* e) {
		e->msg += ", as required by " + getNameForType(need);
		throw e;
	}
}

void TypeAnalyzer::assertClassCanBeBound(Type* binding) {
	PropertySymbolTable* bound = reference->find(binding->typedata._class.classname);
	if(bound->isAbstract())
		throw new SemanticError(ABSTRACT_PROVISION);
}

void TypeAnalyzer::assertClassCanProvide(string provider, Type* binding) {
	string symname = getNameForType(binding) + "<-";
	if(binding->specialty != NULL) symname += binding->specialty;
	if(!reference->find(provider)->find(symname))
		throw new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, symname + "not found on class" + provider);

	vector<Type*>* recurse = reference->find(binding->typedata._class.classname)->getNeeds();
	for(vector<Type*>::iterator it = recurse->begin(); it != recurse->end(); ++it)
	try {
		assertClassCanProvide(provider, *it);
	} catch(SemanticError* e) {
		e->msg += ", when trying to provide " + symname;
		throw e;
	}
}

void TypeAnalyzer::assertClassCanProvide(Type* provider, Type* binding) {
	assertClassCanProvide(string(provider->typedata._class.classname), binding);
}

Type* TypeAnalyzer::getCommonSubtypeOf(Type* a, Type* b) {
	return a;
}

bool TypeAnalyzer::isPrimitiveTypeInt(Type* type) {
	if(type->type == TYPE_MATCHALL) return true;
	return type->type != TYPE_LAMBDA && type->typedata._class.classname == string("Int") && type->arrayed == 0 && type->optional == 0;
}

bool TypeAnalyzer::isPrimitiveTypeText(Type* type) {
	if(type->type == TYPE_MATCHALL) return true;
	return type->type != TYPE_LAMBDA && type->typedata._class.classname == string("Text") && type->arrayed == 0 && type->optional == 0;
}

bool TypeAnalyzer::isPrimitiveTypeBool(Type* type) {
	if(type->type == TYPE_MATCHALL) return true;
	return type->type != TYPE_LAMBDA && type->typedata._class.classname == string("Bool") && type->arrayed == 0 && type->optional == 0;
}

bool TypeAnalyzer::isAutoboxedType(Type* type, string** boxed) {
	if(type->type == TYPE_MATCHALL) return false;

	// need to return a generic autoboxed type according to arguments/return
	if(type->type == TYPE_LAMBDA) { *boxed = new string("lambda"); return true; }

	// shouldn't be here...throw exception?
	if(type->type == TYPE_UNUSABLE) return false;

	// shouldn't be here,..unless I decide to make, say, T?.exists() and/or T?.applyIfExists(fn(T))
	if(type->optional) return false;

	// need to return a Array<T>
	if(type->arrayed) { *boxed = new string("Array"); return true; }

	if(isPrimitiveTypeBool(type)) { *boxed = new string("Bool"); return true; }
	if(isPrimitiveTypeText(type)) { *boxed = new string("Text"); return true; }
	if(isPrimitiveTypeInt(type)) { *boxed = new string("Int"); return true; }

	return false;
}

string TypeAnalyzer::getNameForType(Type* type) {
	string name;

	if(type == NULL) {
		name = "VOID";
		return name;
	}

	if(type->type == TYPE_PARAMETERIZED) {
		return type->typedata.parameterized.label;
	}

	if(type->type == TYPE_MATCHALL) {
		return "{inferencing failed}";
	}

	if(type->type == TYPE_NOTHING) {
		return "[NOTHING]";
	}

	if(type->type == TYPE_CLASS) {
		name = type->typedata._class.classname;
		if(type->typedata._class.parameters != NULL) {
			name += "{";
			int i;
			for(i = 0; i < type->typedata._class.parameters->typecount; i++) {
				if(i) name += ",";
				name += getNameForType(type->typedata._class.parameters->types[i]);
			}
			name += "}";
		}
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
	if(type->optional) name += "?";

	return name;
}

string TypeAnalyzer::getNameForTypeAsProperty(Type* type) {
	string name;

	if(type->alias != NULL) {
		name = type->alias;
	} else {
		name = string(type->typedata._class.shadow, '$') + type->typedata._class.classname;
	}

	return name;
}
