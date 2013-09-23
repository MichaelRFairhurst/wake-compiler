#ifndef HEADER_OBJECT_SYM_TABLE
#define HEADER_OBJECT_SYM_TABLE

#include <string>
#include <map>
#include <vector>
#include "SemanticError.h"
#include "PropertySymbolTable.h"
#include "TypeAnalyzer.h"

using namespace std;

class ObjectSymbolTable {
	public:
		ObjectSymbolTable();
		~ObjectSymbolTable();

		void addClass(string name);
		void addInheritance(string child, bool issubclass);
		PropertySymbolTable* find(string name);
		void assertTypeIsValid(Type* type);
		TypeAnalyzer* getAnalyzer();

	private:
		PropertySymbolTable* addingclass_symbol;
		std::string addingclass_name;
		bool addingclass_hassubclass;
		map<string, PropertySymbolTable*> classes;
		TypeAnalyzer analyzer;
};

#endif
