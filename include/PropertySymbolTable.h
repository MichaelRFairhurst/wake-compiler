#ifndef HEADER_OBJECT_SYMBOL
#define HEADER_OBJECT_SYMBOL
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <boost/optional/optional.hpp>

extern "C" {
	#include "type.h"
	#include "tree.h"
}

#include "ObjectProperty.h"
#include "TypeAnalyzer.h"
#include "SemanticError.h"
#include "EntryPointAnalyzer.h"

#define PROPERTY_PUBLIC 1
#define PROPERTY_ABSTRACT 2

using namespace std;

class PropertySymbolTable {
	public:
		PropertySymbolTable(TypeAnalyzer* tanalyzer);
		~PropertySymbolTable();
		map<string, bool> parentage;
		boost::optional<Type*> find(string name);
		bool isPublic(string name);
		string getAddress(string name);
		string getProvisionAddress(Type* provided);
		boost::optional<SemanticError*> addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, int flags);
		boost::optional<SemanticError*> addProvision(Type* provision);
		void addNeed(Type* returntype);
		void printEntryPoints(EntryPointAnalyzer* entryanalyzer);
		vector<Type*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments);
		bool isAbstract();

		friend void propagateInheritanceTables(PropertySymbolTable* parent, PropertySymbolTable* child, bool extend);

	private:
		bool abstract;
		TypeAnalyzer* analyzer;
		map<string, ObjectProperty*> properties;
		vector<Type*> needs;
};

#endif
