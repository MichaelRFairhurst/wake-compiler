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
		bool isASubtypeOfB(PureType<wake::QUALIFIED>* a, PureType<wake::QUALIFIED>* b);
		bool isAExactlyB(PureType<wake::QUALIFIED>* a, PureType<wake::QUALIFIED>* b);
		void assertFQClassCanProvide(string provider, SpecializablePureType<wake::QUALIFIED>* binding);
		void assertFQClassCanBeProvided(string fullyqualifiedclassname);
		void assertFQNeedIsNotCircular(string classname, string otherclassname);
		boost::optional<PureType<wake::QUALIFIED>*> getCommonSubtypeOf(PureType<wake::QUALIFIED>* a, PureType<wake::QUALIFIED>* b);
		bool isPrimitiveTypeNum(PureType<wake::QUALIFIED>* theint);
		bool isPrimitiveTypeText(PureType<wake::QUALIFIED>* theint);
		bool isPrimitiveTypeBool(PureType<wake::QUALIFIED>* theint);
		bool isPrimitiveTypeChar(PureType<wake::QUALIFIED>* theint);
		bool isAutoboxedType(PureType<wake::QUALIFIED>* type, PureType<wake::QUALIFIED>** boxedtype);
		bool isException(PureType<wake::QUALIFIED>* type);
		bool hasArgParameterization(PureType<wake::QUALIFIED>* type);
		bool hasArgParameterization(PureTypeArray<wake::QUALIFIED>* typearray);

		/**
		 * Text[][] -- 2
		 * Text[]?[] -- 2
		 * Text?[] -- 1
		 */
		int getArrayReferenceLevel(PureType<wake::QUALIFIED>& type);

	private:
		boost::optional<pair<int, string> > getCommonFQClassnamesWithDepth(string a, string b, int depth);

};

#endif
