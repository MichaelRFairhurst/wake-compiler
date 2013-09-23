#ifndef HEADER_TYPE_ANALYZER
#define HEADER_TYPE_ANALYZER

extern "C" {
	#include "type.h"
}

#include <string>

class ObjectSymbolTable;

using namespace std;

class TypeAnalyzer {

	public:
		ObjectSymbolTable* reference;
		bool isASubtypeOfB(string a, string b);
		bool isASubtypeOfB(Type* a, Type* b);
		Type* getCommonSubtypeOf(Type* a, Type* b);
		bool isPrimitiveTypeInt(Type* theint);
		bool isPrimitiveTypeText(Type* theint);
		bool isPrimitiveTypeTruth(Type* theint);
		string getNameForType(Type* type);

};

#endif
