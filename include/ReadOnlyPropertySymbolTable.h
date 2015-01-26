/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ReadOnlyPropertySymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_READONLY_PROPERTY_SYMBOL
#define HEADER_READONLY_PROPERTY_SYMBOL
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

#define PROPERTY_PUBLIC 1
#define PROPERTY_ABSTRACT 2
#define PROPERTY_NEED 4
#define PROPERTY_BLOCKPROVISION 8

using namespace std;

class ReadOnlyPropertySymbolTable {
	public:
		virtual boost::optional<Type*> find(string name)=0;
		virtual boost::optional<ObjectProperty*> findByCasing(string casing)=0;
		virtual bool isPublic(string name)=0;
		virtual string getAddress(string name)=0;
		virtual string getProvisionSymbol(Type* provided, vector<Type*> &arguments)=0;
		virtual vector<Type*>* getNeeds()=0;
		virtual string getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments)=0;
		virtual bool isAbstract()=0;
		virtual ~ReadOnlyPropertySymbolTable() {};
		virtual const map<string, bool>& getParentage()=0;
};

#endif
