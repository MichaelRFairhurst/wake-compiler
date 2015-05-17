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

template<wake::TypeQualification isQualified>
SpecializablePureType<isQualified>::~SpecializablePureType() {
	free(specialty);
}

template<wake::TypeQualification isQualified>
SpecializablePureType<isQualified>::SpecializablePureType(SpecializablePureType<isQualified>& other) {
	typedata = other.typedata;
	specialty = other.specialty == NULL ? NULL : strdup(other.specialty);
}

template<wake::TypeQualification isQualified>
SpecializablePureType<isQualified>& SpecializablePureType<isQualified>::operator=(SpecializablePureType<isQualified>& other) {
	SpecializablePureType<isQualified> temp(other);
	std::swap(*this, temp);
	return *this;
}

template<wake::TypeQualification isQualified>
std::string SpecializablePureType<isQualified>::toProvisionSymbol(std::vector<PureType<wake::QUALIFIED>*> &arguments) {
	std::string name = typedata.toString();
	if(arguments.size()) {
		name += "(";
		for(std::vector<PureType<wake::QUALIFIED>*>::iterator it = arguments.begin(); it != arguments.end(); ++it) {
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
	template<wake::TypeQualification isQualified>
	void swap(SpecializablePureType<isQualified>& lhs, SpecializablePureType<isQualified>& rhs)
	{
		std::swap(lhs.typedata, rhs.typedata);
		std::swap(lhs.specialty, rhs.specialty);
	}
}

SpecializablePureType<wake::UNQUALIFIED>* makeSpecializablePureType(PureType<wake::UNQUALIFIED>* inner) {
	SpecializablePureType<wake::UNQUALIFIED>* type = new SpecializablePureType<wake::UNQUALIFIED>();
	type->typedata = *inner;
	return type;
}

template class SpecializablePureType<wake::QUALIFIED>;
template class SpecializablePureType<wake::UNQUALIFIED>;
