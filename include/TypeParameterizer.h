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
		void rewriteClasstypesToParameterizedtypeByLabel(PureType** typeaddr, const std::vector<PureType*>& parameters);
		void rewriteClasstypesToParameterizedtypeByLabel(PureTypeArray* types, const std::vector<PureType*>& parameters);

		void applyParameterizations(PureType** typeaddr, const std::vector<PureType*>& parameters, const std::vector<PureType*>& parameterizations);
		void applyParameterizations(PureTypeArray* types, const std::vector<PureType*>& parameters, const std::vector<PureType*>& parameterizations);

		bool captureArgumentParameterizations(PureType* actual, PureType* argument, std::map<std::string, PureType*>& parameters, TypeAnalyzer* analyzer);
		bool captureArgumentParameterizations(PureTypeArray* actual, PureTypeArray* argument, std::map<std::string, PureType*>& parameters, TypeAnalyzer* analyzer);

};

#endif
