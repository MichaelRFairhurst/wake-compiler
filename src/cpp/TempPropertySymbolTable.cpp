/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TempPropertySymbolTable.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "TempPropertySymbolTable.h"

boost::optional<Type*> TempPropertySymbolTable::find(string name) {
	return table.find(name);
}

boost::optional<ObjectProperty*> TempPropertySymbolTable::findByCasing(string casing) {
	return table.findByCasing(casing);
}

bool TempPropertySymbolTable::isPublic(string name) {
	return table.isPublic(name);
}

string TempPropertySymbolTable::getAddress(string name) {
	return table.getAddress(name);
}

int TempPropertySymbolTable::getFlags(string name) {
	return table.getFlags(name);
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
