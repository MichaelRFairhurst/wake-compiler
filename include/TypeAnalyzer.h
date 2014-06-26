#ifndef HEADER_TYPE_ANALYZER
#define HEADER_TYPE_ANALYZER

extern "C" {
	#include "type.h"
}

#include <string>

class ClassSpaceSymbolTable;

using namespace std;

class TypeAnalyzer {

	public:
		ClassSpaceSymbolTable* reference;
		bool isASubtypeOfB(string a, string b);
		bool isASubtypeOfB(Type* a, Type* b);
		bool isAExactlyB(Type* a, Type* b);
		void assertClassCanProvide(Type* provider, Type* binding);
		void assertClassCanProvide(string provider, Type* binding);
		void assertClassCanBeBound(Type* binding);
		void assertNeedIsNotCircular(string classname, Type* need);
		Type* getCommonSubtypeOf(Type* a, Type* b);
		bool isPrimitiveTypeNum(Type* theint);
		bool isPrimitiveTypeText(Type* theint);
		bool isPrimitiveTypeBool(Type* theint);
		bool isAutoboxedType(Type* type, Type** boxedtype);
		bool isException(Type* type);
		string getNameForType(Type* type);
		string getNameForTypeAsProperty(Type* type);

};

#endif
