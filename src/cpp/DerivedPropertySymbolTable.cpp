/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * DerivedPropertySymbolTable.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "DerivedPropertySymbolTable.h"
#include <stdexcept>

boost::optional<Type*> DerivedPropertySymbolTable::find(string name) {
	map<string, ObjectProperty*>::iterator searcher;
	searcher = properties->find(name);
	if(searcher == properties->end()) return boost::optional<Type*>();
	else return boost::optional<Type*>(searcher->second->type);
}

bool DerivedPropertySymbolTable::isPublic(string name) {
	return properties->find(name)->second->flags & PROPERTY_PUBLIC;
}

string DerivedPropertySymbolTable::getAddress(string name) {
	return properties->find(name)->second->address;
}

string DerivedPropertySymbolTable::getProvisionSymbol(Type* provided, vector<Type*> &arguments) {
	return analyzer.getProvisionSymbol(provided, arguments);
}

vector<Type*>* DerivedPropertySymbolTable::getNeeds() {
	return needs;
}

string DerivedPropertySymbolTable::getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments) {
	//@TODO not duplicate code from PropertySymbolTable
	string name;
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		name += it->first;
		name += "(";
		int i;
		for(i = 0; i < it->second->typecount; i++) {
			if(i) name += ",";
			name += analyzer.getNameForType(it->second->types[i]);
		}
		name += ")";
	}

	return name;
}

bool DerivedPropertySymbolTable::isAbstract() {
	//@TODO cache this result or something
	for(map<string, ObjectProperty*>::iterator it = properties->begin(); it != properties->end(); ++it) {
		if(it->second->flags & PROPERTY_ABSTRACT) return true;
	}

	return false;
}

const map<string, bool>& DerivedPropertySymbolTable::getParentage() {
	return parentage;
}

DerivedPropertySymbolTable::~DerivedPropertySymbolTable() {
	for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		delete *it;
	}
	for(map<string, ObjectProperty*>::iterator it = properties->begin(); it != properties->end(); ++it) {
		delete it->second;
	}
	delete needs; delete properties;
}
