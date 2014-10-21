/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TempPropertySymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TEMP_PROPERTY_SYMBOL
#define HEADER_TEMP_PROPERTY_SYMBOL
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <boost/optional/optional.hpp>

extern "C" {
	#include "type.h"
	#include "tree.h"
}

#include "ReadOnlyPropertySymbolTable.h"
#include "PropertySymbolTable.h"
#include "ObjectProperty.h"
#include "TypeAnalyzer.h"

using namespace std;

class TempPropertySymbolTable : public ReadOnlyPropertySymbolTable {
	public:
		TempPropertySymbolTable(PropertySymbolTable& table) : table(table) {};
		boost::optional<Type*> find(string name);
		bool isPublic(string name);
		string getAddress(string name);
		string getProvisionSymbol(Type* provided, vector<Type*> &arguments);
		vector<Type*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments);
		bool isAbstract();
		const map<string, bool>& getParentage();
		~TempPropertySymbolTable() {};

	private:
		PropertySymbolTable& table;

};

#endif
