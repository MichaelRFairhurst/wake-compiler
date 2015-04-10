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

#include "PureType.h"
#include "SpecializablePureType.h"
#include "SpecializableVarDecl.h"
#include "VarRef.h"
#include <string>
#include <vector>
#include <boost/optional/optional.hpp>

class ClassSpaceSymbolTable;

using namespace std;

class TypeAnalyzer {

	public:
		ClassSpaceSymbolTable* reference;
		bool isASubtypeOfB(string a, string b);
		bool isASubtypeOfB(PureType* a, PureType* b);
		bool isAExactlyB(PureType* a, PureType* b);
		void assertFQClassCanProvide(string provider, SpecializablePureType* binding);
		void assertFQClassCanBeProvided(string fullyqualifiedclassname);
		void assertFQNeedIsNotCircular(string classname, string otherclassname);
		boost::optional<PureType*> getCommonSubtypeOf(PureType* a, PureType* b);
		bool isPrimitiveTypeNum(PureType* theint);
		bool isPrimitiveTypeText(PureType* theint);
		bool isPrimitiveTypeBool(PureType* theint);
		bool isAutoboxedType(PureType* type, PureType** boxedtype);
		bool isException(PureType* type);
		bool hasArgParameterization(PureType* type);
		bool hasArgParameterization(PureTypeArray* typearray);

		/**
		 * Text[][] -- 2
		 * Text[]?[] -- 2
		 * Text?[] -- 1
		 */
		int getArrayReferenceLevel(PureType& type);

	private:
		boost::optional<pair<int, string> > getCommonFQClassnamesWithDepth(string a, string b, int depth);

};

#endif
