#include "DerivedPropertySymbolTable.h"
#include <stdexcept>

boost::optional<Type*> DerivedPropertySymbolTable::find(string name) {
	map<string, ObjectProperty*>::iterator searcher;
	searcher = properties.find(name);
	if(searcher == properties.end()) return boost::optional<Type*>();
	else return boost::optional<Type*>(searcher->second->type);
}

bool DerivedPropertySymbolTable::isPublic(string name) {
	return properties.find(name)->second->flags & PROPERTY_PUBLIC;
}

string DerivedPropertySymbolTable::getAddress(string name) {
	throw new std::runtime_error("NOT IMPLEMENTED YET");
}

string DerivedPropertySymbolTable::getProvisionSymbol(Type* provided) {
	throw new std::runtime_error("NOT IMPLEMENTED YET");
}

vector<Type*>* DerivedPropertySymbolTable::getNeeds() {
	return &needs;
}

string DerivedPropertySymbolTable::getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments) {
	throw new std::runtime_error("NOT IMPLEMENTED YET");
}

bool DerivedPropertySymbolTable::isAbstract() {
	//@TODO cache this result or something
	for(auto it = properties.begin(); it != properties.end(); ++it) {
		if(it->second->flags & PROPERTY_ABSTRACT) return true;
	}

	return false;
}

const map<string, bool>& DerivedPropertySymbolTable::getParentage() {
	return parentage;
}
