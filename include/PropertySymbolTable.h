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
#include "EntryPointAnalyzer.h"

using namespace std;

class PropertySymbolTable : public ReadOnlyPropertySymbolTable {
	public:
		PropertySymbolTable(TypeAnalyzer* tanalyzer);
		~PropertySymbolTable();
		boost::optional<Type*> find(string name);
		bool isPublic(string name);
		string getAddress(string name);
		string getProvisionSymbol(Type* provided);
		boost::optional<SemanticError*> addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, int flags);
		boost::optional<SemanticError*> addProperty(Type* property, int flags);
		boost::optional<SemanticError*> addProvision(Type* provision);
		void addNeed(Type* returntype);
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

		ReadOnlyPropertySymbolTable* resolveParameters(vector<Type*>& parameters);

		friend void propagateInheritanceTables(PropertySymbolTable* parent, PropertySymbolTable* child, bool extend);

	private:
		TypeAnalyzer* analyzer;
		vector<Type*> needs;
		vector<Type*>* declaredtypeparameters;
};

#endif
