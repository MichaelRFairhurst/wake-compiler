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

using namespace std;

class PropertySymbolTable {
	public:
		map<string, bool> parentage;
		Type* get(string name);
		string addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, Node* body);
		string getSymbolNameOf(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments);

	private:
		map<string, Type*> properties;
		string getSymbolNameOf(Type* type);
};

#endif
