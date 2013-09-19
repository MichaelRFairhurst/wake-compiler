#ifndef HEADER_OBJECT_SYM_TABLE
#define HEADER_OBJECT_SYM_TABLE

#include <string>
#include <map>
#include <vector>
#include "SemanticError.h"
#include "PropertySymbolTable.h"

using namespace std;

class ObjectSymbolTable {
	public:
		~ObjectSymbolTable();

		bool isASubtypeOfB(string a, string b);
		void addClass(string name);
		void addInheritance(string child, bool issubclass);
		PropertySymbolTable* find(string name);
		void assertTypeIsValid(Type* type);

	private:
		PropertySymbolTable* addingclass_symbol;
		std::string addingclass_name;
		bool addingclass_hassubclass;
		map<string, PropertySymbolTable*> classes;
};

#endif
