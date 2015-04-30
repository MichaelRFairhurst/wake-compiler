/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * SpecializableVarDecl.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "SpecializableVarDecl.h"

#include <cstdlib>
#include <string.h>

SpecializableVarDecl::~SpecializableVarDecl() {
	free(specialty);
}

SpecializableVarDecl::SpecializableVarDecl(SpecializableVarDecl& other) {
	decl = other.decl;
	specialty = strdup(other.specialty);
}

SpecializableVarDecl& SpecializableVarDecl::operator=(SpecializableVarDecl& other) {
	SpecializableVarDecl temp(other);
	std::swap(*this, other);
	return *this;
}

namespace std
{
	template<>
	void swap(SpecializableVarDecl& lhs, SpecializableVarDecl& rhs)
	{
		std::swap(lhs.decl, rhs.decl);
		std::swap(lhs.specialty, rhs.specialty);
	}
}
