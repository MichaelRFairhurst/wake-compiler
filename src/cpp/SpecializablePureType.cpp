/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * SpecializablePureType.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "SpecializablePureType.h"

#include <cstdlib>
#include <string.h>

SpecializablePureType::~SpecializablePureType() {
	free(specialty);
}

SpecializablePureType::SpecializablePureType(SpecializablePureType& other) {
	typedata = other.typedata;
	specialty = strdup(other.specialty);
}

SpecializablePureType& SpecializablePureType::operator=(SpecializablePureType& other) {
	SpecializablePureType temp(other);
	std::swap(*this, other);
	return *this;
}

std::string SpecializablePureType::toProvisionSymbol(std::vector<PureType*> &arguments) {
	std::string name = typedata.toString();
	if(arguments.size()) {
		name += "(";
		for(std::vector<PureType*>::iterator it = arguments.begin(); it != arguments.end(); ++it) {
			if(it != arguments.begin()) name += ",";
			name += (*it)->toString();
		}
		name += ")";
	}
	name += "<-";
	if(specialty != NULL) name += specialty;
	return name;
}

namespace std
{
	template<>
	void swap(SpecializablePureType& lhs, SpecializablePureType& rhs)
	{
		std::swap(lhs.typedata, rhs.typedata);
		std::swap(lhs.specialty, rhs.specialty);
	}
}
