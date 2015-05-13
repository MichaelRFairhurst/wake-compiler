/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeParameterizer.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TYPE_PARAMETERIZER
#define HEADER_TYPE_PARAMETERIZER

#include <vector>
#include <map>
#include <string>
#include "TypeAnalyzer.h"
#include "PureType.h"
#include "PureTypeArray.h"

class TypeParameterizer {

	public:
		// these work on qualified or unqualified types
		template<wake::TypeQualification isQualified>
		void rewriteClasstypesToParameterizedtypeByLabel(PureType<isQualified>* pType, const std::vector<PureType<isQualified>*>& parameters);
		template<wake::TypeQualification isQualified>
		void rewriteClasstypesToParameterizedtypeByLabel(PureTypeArray<isQualified>* types, const std::vector<PureType<isQualified>*>& parameters);

		// these should really only be done after qualification has completed
		void applyParameterizations(PureType<wake::QUALIFIED>* pType, const std::vector<PureType<wake::QUALIFIED>*>& parameters, const std::vector<PureType<wake::QUALIFIED>*>& parameterizations);
		void applyParameterizations(PureTypeArray<wake::QUALIFIED>* types, const std::vector<PureType<wake::QUALIFIED>*>& parameters, const std::vector<PureType<wake::QUALIFIED>*>& parameterizations);

		// this one definitely should only be done after qualification has completed
		bool captureArgumentParameterizations(PureType<wake::QUALIFIED>* actual, PureType<wake::QUALIFIED>* argument, std::map<std::string, PureType<wake::QUALIFIED>*>& parameters, TypeAnalyzer* analyzer);
		bool captureArgumentParameterizations(PureTypeArray<wake::QUALIFIED>* actual, PureTypeArray<wake::QUALIFIED>* argument, std::map<std::string, PureType<wake::QUALIFIED>*>& parameters, TypeAnalyzer* analyzer);

};

#endif
