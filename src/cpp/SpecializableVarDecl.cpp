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
	specialty = other.specialty == NULL ? NULL : strdup(other.specialty);
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

SpecializableVarDecl* makeSpecializableVarDecl(PureType* type) {
	SpecializableVarDecl* spDecl = new SpecializableVarDecl();
	spDecl->decl.typedata = *type;
	return spDecl;
}

SpecializableVarDecl* makeSpecializableVarDeclFromVarDecl(VarDecl* decl) {
	SpecializableVarDecl* spDecl = new SpecializableVarDecl();
	spDecl->decl = *decl;
	return spDecl;
}

SpecializableVarDecl* copySpecializableVarDecl(SpecializableVarDecl* decl) {
	return new SpecializableVarDecl(*decl);
}
