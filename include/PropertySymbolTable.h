#ifndef HEADER_OBJECT_SYMBOL
#define HEADER_OBJECT_SYMBOL
#include <map>
#include <string>
#include <vector>
#include <utility>

extern "C" {
	#include "type.h"
	#include "node.h"
}

#include "TypeAnalyzer.h"
#include "EntryPointAnalyzer.h"
#include "AddressAllocator.h"

using namespace std;

class PropertySymbolTable {
	public:
		PropertySymbolTable(TypeAnalyzer* tanalyzer, AddressAllocator* allocator);
		~PropertySymbolTable();
		map<string, bool> parentage;
		Type* get(string name);
		string getAddress(string name);
		string getProvisionAddress(Type* provided);
		void addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, Node* body);
		void addProvision(Type* provision, Node* body);
		void addNeed(Type* returntype);
		void printEntryPoints(EntryPointAnalyzer* entryanalyzer);
		vector<Type*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments);
		void assignAddresses();

		friend void propagateInheritanceTables(PropertySymbolTable* parent, PropertySymbolTable* child, bool extend);

	private:
		TypeAnalyzer* analyzer;
		AddressAllocator* alloc;
		map<string, pair<Type*, string> > properties;
		vector<Type*> needs;
};

#endif
