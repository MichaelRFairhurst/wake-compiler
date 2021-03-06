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
#include "PureTypeArray.h"
#include <stdexcept>

using namespace wake;

boost::optional<PureType<QUALIFIED>*> DerivedPropertySymbolTable::find(string name) {
	map<string, ObjectProperty*>::iterator searcher;
	searcher = properties->find(name);
	if(searcher == properties->end()) return boost::optional<PureType<QUALIFIED>*>();
	else return boost::optional<PureType<QUALIFIED>*>(&searcher->second->decl.typedata);
}

	//@TODO not duplicate code from PropertySymbolTable
boost::optional<ObjectProperty*> DerivedPropertySymbolTable::findByCasing(string casing) {
	ObjectProperty* found = NULL;

	for(std::map<string, ObjectProperty*>::iterator it = properties->begin(); it != properties->end(); ++it)
	if(it->second->casing == casing)
	if(found == NULL) {
		found = it->second;
	} else {
		return boost::optional<ObjectProperty*>();
	}

	if(found) {
		return boost::optional<ObjectProperty*>(found);
	} else {
		return boost::optional<ObjectProperty*>();
	}
}

bool DerivedPropertySymbolTable::isPublic(string name) {
	return properties->find(name)->second->flags & PROPERTY_PUBLIC;
}

string DerivedPropertySymbolTable::getAddress(string name) {
	return properties->find(name)->second->address;
}

int DerivedPropertySymbolTable::getFlags(string name) {
	return properties->find(name)->second->flags;
}

vector<SpecializableVarDecl<QUALIFIED>*>* DerivedPropertySymbolTable::getNeeds() {
	return needs;
}

string DerivedPropertySymbolTable::getSymbolNameOf(vector<pair<string, PureTypeArray<QUALIFIED>*> >* segments_arguments) {
	//@TODO not duplicate code from PropertySymbolTable
	string name;
	for(vector<pair<string, PureTypeArray<QUALIFIED>*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		name += it->first;
		name += "(";
		int i;
		for(i = 0; i < it->second->typecount; i++) {
			if(i) name += ",";
			name += it->second->types[i]->toString();
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

string DerivedPropertySymbolTable::getModule() {
	return module;
}

string DerivedPropertySymbolTable::getClassname() {
	return classname;
}

const map<string, bool>& DerivedPropertySymbolTable::getParentage() {
	return parentage;
}

DerivedPropertySymbolTable::~DerivedPropertySymbolTable() {
	for(vector<SpecializableVarDecl<QUALIFIED>*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		delete *it;
	}
	for(map<string, ObjectProperty*>::iterator it = properties->begin(); it != properties->end(); ++it) {
		delete it->second;
	}
	delete needs; delete properties;
}
