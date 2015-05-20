/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ScopeSymbolTable.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ScopeSymbolTable.h"
#include "SemanticError.h"

void ScopeSymbolTable::add(string name, PureType<wake::QUALIFIED>* type) {
	if(table.count(name)) {
		string temp = "Symbol " + name + " already defined in this scope.";
		throw new SemanticError(SYMBOL_ALREADY_DEFINED, temp);
	}

	table[name] = pair<PureType<wake::QUALIFIED>*, string>(type, allocator.allocate());
	scopes.back().push_back(name);
}

void ScopeSymbolTable::add(VarDecl<wake::QUALIFIED>* type) {
	VarRef ref(type->createVarRef());
	add(ref.toString(), &type->typedata);
}

void ScopeSymbolTable::addOverwriting(string name, PureType<wake::QUALIFIED>* type) {
	table[name] = pair<PureType<wake::QUALIFIED>*, string>(type, allocator.allocate());
	//scopes.back().push_back(name);
}

void ScopeSymbolTable::addOverwriting(VarDecl<wake::QUALIFIED>* type) {
	VarRef ref(type->createVarRef());
	addOverwriting(ref.toString(), &type->typedata);
}

boost::optional<PureType<wake::QUALIFIED>*> ScopeSymbolTable::find(string name) {
	if(!table.count(name)) {
		string temp = "Symbol " + name + " not defined in this scope.";
		//throw new SemanticError(SYMBOL_NOT_DEFINED, temp);
		return boost::optional<PureType<wake::QUALIFIED>*>();
	}

	map<string, pair<PureType<wake::QUALIFIED>*, string> >::iterator it = table.find(name);
	return boost::optional<PureType<wake::QUALIFIED>*>(it->second.first);
}

boost::optional<PureType<wake::QUALIFIED>*> ScopeSymbolTable::find(VarRef* type) {
	string name = type->toString();
	if(!table.count(name)) {
		string temp = "Symbol " + name + " not defined in this scope.";
		return boost::optional<PureType<wake::QUALIFIED>*>();
		throw new SemanticError(SYMBOL_NOT_DEFINED, temp);
	}

	map<string, pair<PureType<wake::QUALIFIED>*, string> >::iterator it = table.find(name);
	return boost::optional<PureType<wake::QUALIFIED>*>(it->second.first);
}

string ScopeSymbolTable::getAddress(VarRef* type) {
	string name = type->toString();
	map<string, pair<PureType<wake::QUALIFIED>*, string> >::iterator it = table.find(name);
	return it->second.second;
}

string ScopeSymbolTable::getAddress(string name) {
	map<string, pair<PureType<wake::QUALIFIED>*, string> >::iterator it = table.find(name);
	return it->second.second;
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
