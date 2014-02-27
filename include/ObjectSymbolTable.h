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

using namespace std;

class ObjectSymbolTable {
	public:
		ObjectSymbolTable();
		~ObjectSymbolTable();

		boost::optional<SemanticError*> addClass(string name);
		boost::optional<SemanticError*> addInheritance(string child, bool issubclass);
		void propagateInheritance();
		void propagateInheritanceToParent(string childname);
		PropertySymbolTable* find(string name);
		void assertTypeIsValid(Type* type);
		TypeAnalyzer* getAnalyzer();
		void printEntryPoints(EntryPointAnalyzer* entryanalyzer);

		// Mabe not great to be public but solves immediate problems
		map<string, PropertySymbolTable*> classes;

	private:
		PropertySymbolTable* addingclass_symbol;
		std::string addingclass_name;
		bool addingclass_hassubclass;
		map<string, pair<PropertySymbolTable*, bool> > inheritances_gathered;
		TypeAnalyzer analyzer;
};

#endif
