#include "TempPropertySymbolTable.h";

boost::optional<Type*> TempPropertySymbolTable::find(string name) {
	return table.find(name);
}

bool TempPropertySymbolTable::isPublic(string name) {
	return table.isPublic(name);
}

string TempPropertySymbolTable::getAddress(string name) {
	return table.getAddress(name);
}

string TempPropertySymbolTable::getProvisionSymbol(Type* provided, vector<Type*> &arguments) {
	return table.getProvisionSymbol(provided, arguments);
}

vector<Type*>* TempPropertySymbolTable::getNeeds() {
	return table.getNeeds();
}

string TempPropertySymbolTable::getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments) {
	return table.getSymbolNameOf(segments_arguments);
}

bool TempPropertySymbolTable::isAbstract() {
	return table.isAbstract();
}

const map<string, bool>& TempPropertySymbolTable::getParentage() {
	return table.getParentage();
}
