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

extern "C" {
	#include "type.h"
}

class TypeParameterizer {

	public:
		void writeInParameterizations(Type** typeaddr, const std::vector<Type*>& parameters);
		void writeInParameterizations(TypeArray* types, const std::vector<Type*>& parameters);
		void applyParameterizations(Type** typeaddr, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations);
		void applyParameterizations(TypeArray* types, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations);

};

#endif
