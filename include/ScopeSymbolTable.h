#ifndef HEADER_SCOPE_SYMBOL_TABLE
#define HEADER_SCOPE_SYMBOL_TABLE

#include <string>
#include <vector>
#include <map>
#include <boost/optional/optional.hpp>

extern "C" {
	#include "type.h"
}

#include "AddressAllocator.h"

using namespace std;

class ScopeSymbolTable {

	public:
		ScopeSymbolTable() {
			scopes.push_back(vector<string>());
		}

		void add(string name, Type* type);
		void add(Type* type);
		string getNameForType(Type* type);
		boost::optional<Type*> find(string name);
		boost::optional<Type*> find(Type* type);
		string getAddress(Type* type);
		string getAddress(string name);
		void pushScope();
		void popScope();

	private:
		AddressAllocator allocator;
		map<string, pair<Type*, string> > table;
		vector<vector<string> > scopes;
};

#endif
