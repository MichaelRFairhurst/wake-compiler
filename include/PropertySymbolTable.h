/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PropertySymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_PROPERTY_SYMBOL
#define HEADER_PROPERTY_SYMBOL
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
#include "SemanticError.h"
#include "ErrorTracker.h"
#include "EntryPointAnalyzer.h"
#include "Annotation.h"

using namespace std;

class PropertySymbolTable : public ReadOnlyPropertySymbolTable {
	public:
		PropertySymbolTable(TypeAnalyzer* tanalyzer);
		~PropertySymbolTable();
		boost::optional<Type*> find(string name);
		bool isPublic(string name);
		string getAddress(string name);
		string getProvisionSymbol(Type* provided, vector<Type*> &arguments);
		boost::optional<SemanticError*> addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, int flags, vector<Annotation*> annotations);
		boost::optional<SemanticError*> addProperty(Type* property, int flags, vector<Annotation*> annotations);
		boost::optional<SemanticError*> addProvision(Type* provision, vector<Type*> &arguments, int flags);
		bool isBehavioralProvision(string name);
		void addNeed(Type* returntype, int flags, vector<Annotation*> annotations);
		void printEntryPoints(EntryPointAnalyzer* entryanalyzer);
		vector<Type*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments);
		string getCasingNameOf(vector<pair<string, TypeArray*> >* segments_arguments);
		bool isAbstract();
		string classname;
		bool abstract;
		map<string, ObjectProperty*> properties;
		const map<string, bool>& getParentage();
		map<string, bool> parentage;
		void setParameters(vector<Type*>* parameters);
		const vector<Type*>& getParameters();
		Type* getAsType();
		void setAnnotations(vector<Annotation*> annotations);
		const boost::ptr_vector<Annotation*>& getAnnotations();

		ReadOnlyPropertySymbolTable* resolveParameters(vector<Type*>& parameters);

		friend void propagateInheritanceTables(PropertySymbolTable* parent, PropertySymbolTable* child, bool extend, ErrorTracker& errors);

	private:
		TypeAnalyzer* analyzer;
		vector<Type*>* needs;
		vector<Type*>* declaredtypeparameters;
		boost::ptr_vector<Annotation*> annotations;
};

#endif
