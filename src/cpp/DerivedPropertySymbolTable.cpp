#include "DerivedPropertySymbolTable.h"

boost::optional<Type*> DerivedPropertySymbolTable::find(string name) {
	map<string, ObjectProperty*>::iterator searcher;
	searcher = properties.find(name);
	if(searcher == properties.end()) return boost::optional<Type*>();
	else return boost::optional<Type*>(searcher->second->type);
}

bool DerivedPropertySymbolTable::isPublic(string name) {

}

string DerivedPropertySymbolTable::getAddress(string name) {

}

string DerivedPropertySymbolTable::getProvisionSymbol(Type* provided) {

}

vector<Type*>* DerivedPropertySymbolTable::getNeeds() {

}

string DerivedPropertySymbolTable::getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments) {

}

bool DerivedPropertySymbolTable::isAbstract() {

}

const map<string, bool>& DerivedPropertySymbolTable::getParentage() {

}
