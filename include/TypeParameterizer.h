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

extern "C" {
	#include "type.h"
}

class TypeParameterizer {

	public:
		void rewriteClasstypesToParameterizedtypeByLabel(Type** typeaddr, const std::vector<Type*>& parameters);
		void rewriteClasstypesToParameterizedtypeByLabel(TypeArray* types, const std::vector<Type*>& parameters);

		void applyParameterizations(Type** typeaddr, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations);
		void applyParameterizations(TypeArray* types, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations);

		bool captureArgumentParameterizations(Type* actual, Type* argument, std::map<std::string, Type*>& parameters, TypeAnalyzer* analyzer);
		bool captureArgumentParameterizations(TypeArray* actual, TypeArray* argument, std::map<std::string, Type*>& parameters, TypeAnalyzer* analyzer);

};

#endif
