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
#include "AddressAllocator.h"

using namespace std;

class PropertySymbolTable {
	public:
		~PropertySymbolTable();
		map<string, bool> parentage;
		Type* get(string name);
		string getAddress(string name);
		string getProvisionAddress(Type* provided);
		void addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, Node* body);
		void addProvision(Type* provision, Node* body);
		void addNeed(Type* returntype);
		vector<Type*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments);
		TypeAnalyzer* analyzer;

	private:
		AddressAllocator alloc;
		map<string, pair<Type*, string> > properties;
		vector<Type*> needs;
};

#endif
