/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ClassSpaceSymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_OBJECT_SYM_TABLE
#define HEADER_OBJECT_SYM_TABLE

#include <utility>
#include <string>
#include <map>
#include <vector>
#include <boost/optional/optional.hpp>
#include "SemanticError.h"
#include "PropertySymbolTable.h"
#include "TypeAnalyzer.h"
#include "EntryPointAnalyzer.h"
#include "AddressAllocator.h"
#include "ErrorTracker.h"

using namespace std;

class ClassSpaceSymbolTable {
	public:
		ClassSpaceSymbolTable();
		~ClassSpaceSymbolTable();

		boost::optional<SemanticError*> addClass(string name);
		boost::optional<SemanticError*> addInheritance(string child, bool issubclass);

		PropertySymbolTable* getEmptyPropertySymbolTable();
		boost::optional<SemanticError*> importClass(PropertySymbolTable* table);
		vector<PropertySymbolTable*> getDefinedClasses();

		void propagateInheritance(ErrorTracker& errors);
		void propagateInheritanceToParent(string childname, ErrorTracker& errors);
		ReadOnlyPropertySymbolTable* find(Type* type);
		ReadOnlyPropertySymbolTable* find(string name);
		void assertTypeIsValid(Type* type);
		void assertNoNeedsAreCircular();
		TypeAnalyzer* getAnalyzer();
		void printEntryPoints(EntryPointAnalyzer* entryanalyzer);

		PropertySymbolTable* findModifiable(string name);
		PropertySymbolTable* findModifiable(Type* type);
		void setModule(string module);
		string getModule();

	private:

		// TRUE means defined, FALSE means imported
		string module;
		map<string, pair<PropertySymbolTable*, bool> > classes;
		PropertySymbolTable* addingclass_symbol;
		std::string addingclass_name;
		bool addingclass_hassubclass;
		map<string, pair<PropertySymbolTable*, bool> > inheritances_gathered;
		TypeAnalyzer analyzer;
};

#endif
