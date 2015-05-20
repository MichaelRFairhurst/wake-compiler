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

		void add(string name, PureType<wake::QUALIFIED>* type);
		void add(VarDecl<wake::QUALIFIED>* type);
		void addOverwriting(string name, PureType<wake::QUALIFIED>* type);
		void addOverwriting(VarDecl<wake::QUALIFIED>* type);
		boost::optional<PureType<wake::QUALIFIED>*> find(string name);
		boost::optional<PureType<wake::QUALIFIED>*> find(VarRef* type);
		string getAddress(VarRef* type);
		string getAddress(string name);
		void pushScope();
		void popScope();

	private:
		AddressAllocator allocator;
		map<string, pair<PureType<wake::QUALIFIED>*, string> > table;
		vector<vector<string> > scopes;
};

#endif
