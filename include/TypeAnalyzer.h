/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeAnalyzer.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TYPE_ANALYZER
#define HEADER_TYPE_ANALYZER

extern "C" {
	#include "type.h"
}

#include <string>
#include <vector>
#include <boost/optional/optional.hpp>

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
		boost::optional<Type*> getCommonSubtypeOf(Type* a, Type* b);
		bool isPrimitiveTypeNum(Type* theint);
		bool isPrimitiveTypeText(Type* theint);
		bool isPrimitiveTypeBool(Type* theint);
		bool isAutoboxedType(Type* type, Type** boxedtype);
		bool isException(Type* type);
		bool hasArgParameterization(Type* type);
		bool hasArgParameterization(TypeArray* typearray);

		/**
		 * Text[][] -- 2
		 * Text[]?[] -- 2
		 * Text?[] -- 1
		 */
		int getArrayReferenceLevel(Type& type);
		string getNameForType(Type* type);
		string getNameForTypeAsProperty(Type* type);
		string getProvisionSymbol(Type* provided, vector<Type*> &arguments);

	private:
		boost::optional<pair<int, Type*> > getCommonClassnamesWithDepth(Type& a, Type& b, int depth);

};

#endif
