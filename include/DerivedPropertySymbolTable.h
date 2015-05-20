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

#include "tree.h"

#include "ReadOnlyPropertySymbolTable.h"
#include "ObjectProperty.h"
#include "TypeAnalyzer.h"

using namespace std;

class DerivedPropertySymbolTable : public ReadOnlyPropertySymbolTable {
	public:
		DerivedPropertySymbolTable(TypeAnalyzer& analyzer, vector<SpecializableVarDecl<wake::QUALIFIED>*>* needs, map<string, ObjectProperty*>* properties, const map<string, bool>& parentage) : analyzer(analyzer), parentage(parentage) {
			this->needs = needs;
			this->properties = properties;
		};
		boost::optional<PureType<wake::QUALIFIED>*> find(string name);
		boost::optional<ObjectProperty*> findByCasing(string casing);
		bool isPublic(string name);
		string getAddress(string name);
		int getFlags(string name);
		vector<SpecializableVarDecl<wake::QUALIFIED>*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments);
		bool isAbstract();
		const map<string, bool>& getParentage();
		~DerivedPropertySymbolTable();

	private:
		TypeAnalyzer& analyzer;
		vector<SpecializableVarDecl<wake::QUALIFIED>*>* needs;
		map<string, ObjectProperty*>* properties;
		const map<string, bool>& parentage;


};

#endif
