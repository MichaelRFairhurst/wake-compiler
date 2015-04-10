/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ScopeSymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_SCOPE_SYMBOL_TABLE
#define HEADER_SCOPE_SYMBOL_TABLE

#include <string>
#include <vector>
#include <map>
#include <boost/optional/optional.hpp>

#include "AddressAllocator.h"
#include "PureType.h"
#include "VarDecl.h"

using namespace std;

class ScopeSymbolTable {

	public:
		ScopeSymbolTable() {
			scopes.push_back(vector<string>());
		}

		void add(string name, PureType* type);
		void add(VarDecl* type);
		void addOverwriting(string name, PureType* type);
		void addOverwriting(VarDecl* type);
		boost::optional<PureType*> find(string name);
		boost::optional<PureType*> find(VarRef* type);
		string getAddress(VarRef* type);
		string getAddress(string name);
		void pushScope();
		void popScope();

	private:
		AddressAllocator allocator;
		map<string, pair<PureType*, string> > table;
		vector<vector<string> > scopes;
};

#endif
