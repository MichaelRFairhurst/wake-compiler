#include "ScopeSymbolTable.h"
#include "SemanticError.h"

void ScopeSymbolTable::add(string name, Type* type) {
	if(table.count(name)) {
		string temp = "Symbol " + name + " already defined in this scope.";
		throw new SemanticError(SYMBOL_ALREADY_DEFINED, temp);
	}

	table[name] = pair<Type*, string>(type, allocator.allocate());
	scopes.back().push_back(name);
}

void ScopeSymbolTable::add(Type* type) {
	add(getNameForType(type), type);
}

void ScopeSymbolTable::addOverwriting(string name, Type* type) {
	table[name] = pair<Type*, string>(type, allocator.allocate());
	//scopes.back().push_back(name);
}

void ScopeSymbolTable::addOverwriting(Type* type) {
	addOverwriting(getNameForType(type), type);
}

boost::optional<Type*> ScopeSymbolTable::find(string name) {
	if(!table.count(name)) {
		string temp = "Symbol " + name + " not defined in this scope.";
		//throw new SemanticError(SYMBOL_NOT_DEFINED, temp);
		return boost::optional<Type*>();
	}

	map<string, pair<Type*, string> >::iterator it = table.find(name);
	return boost::optional<Type*>(it->second.first);
}

boost::optional<Type*> ScopeSymbolTable::find(Type* type) {
	string name = getNameForType(type);
	if(!table.count(name)) {
		string temp = "Symbol " + name + " not defined in this scope.";
		return boost::optional<Type*>();
		throw new SemanticError(SYMBOL_NOT_DEFINED, temp);
	}

	map<string, pair<Type*, string> >::iterator it = table.find(name);
	return boost::optional<Type*>(it->second.first);
}

string ScopeSymbolTable::getAddress(Type* type) {
	string name = getNameForType(type);
	map<string, pair<Type*, string> >::iterator it = table.find(name);
	return it->second.second;
}

string ScopeSymbolTable::getAddress(string name) {
	map<string, pair<Type*, string> >::iterator it = table.find(name);
	return it->second.second;
}

string ScopeSymbolTable::getNameForType(Type* type) {
	string name;

	if(type->alias != NULL) {
		name = type->alias;
	} else {
		if(type->type == TYPE_PARAMETERIZED){
			name = string(type->typedata.parameterized.shadow, '$') + type->typedata.parameterized.label;
		} else {
			name = string(type->typedata._class.shadow, '$') + type->typedata._class.classname;
		}

		if(type->arrayed) name += "[]";
	}

	return name;
}

void ScopeSymbolTable::pushScope() {
	scopes.push_back(vector<string>());
}

void ScopeSymbolTable::popScope() {
	for(vector<string>::iterator it = scopes.back().begin(); it != scopes.back().end(); ++it) {
		table.erase(*it);
		allocator.deallocate();
	}

	scopes.pop_back();
}
