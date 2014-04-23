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

using namespace std;

class ReadOnlyPropertySymbolTable {
	public:
		virtual boost::optional<Type*> find(string name)=0;
		virtual bool isPublic(string name)=0;
		virtual string getAddress(string name)=0;
		virtual string getProvisionSymbol(Type* provided)=0;
		virtual vector<Type*>* getNeeds()=0;
		virtual string getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments)=0;
		virtual bool isAbstract()=0;
		virtual ~ReadOnlyPropertySymbolTable() {};
		virtual const map<string, bool>& getParentage()=0;
};

#endif
