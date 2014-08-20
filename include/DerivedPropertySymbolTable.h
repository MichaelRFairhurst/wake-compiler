/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * DerivedPropertySymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_DERIVED_PROPERTY_SYMBOL
#define HEADER_DERIVED_PROPERTY_SYMBOL
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
#include "ObjectProperty.h"
#include "TypeAnalyzer.h"

using namespace std;

class DerivedPropertySymbolTable : public ReadOnlyPropertySymbolTable {
	public:
		DerivedPropertySymbolTable(TypeAnalyzer& analyzer, vector<Type*>& needs, map<string, ObjectProperty*>& properties, const map<string, bool>& parentage) : analyzer(analyzer), needs(needs), properties(properties), parentage(parentage) {};
		boost::optional<Type*> find(string name);
		bool isPublic(string name);
		string getAddress(string name);
		string getProvisionSymbol(Type* provided);
		vector<Type*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments);
		bool isAbstract();
		const map<string, bool>& getParentage();

	private:
		TypeAnalyzer& analyzer;
		vector<Type*>& needs;
		map<string, ObjectProperty*>& properties;
		const map<string, bool>& parentage;


};

#endif
