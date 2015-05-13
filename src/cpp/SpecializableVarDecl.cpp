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

template<wake::TypeQualification isQualified>
SpecializableVarDecl<isQualified>::~SpecializableVarDecl() {
	free(specialty);
}

template<wake::TypeQualification isQualified>
SpecializableVarDecl<isQualified>::SpecializableVarDecl(SpecializableVarDecl<isQualified>& other) {
	decl = other.decl;
	specialty = other.specialty == NULL ? NULL : strdup(other.specialty);
}

template<wake::TypeQualification isQualified>
SpecializableVarDecl<isQualified>& SpecializableVarDecl<isQualified>::operator=(SpecializableVarDecl<isQualified>& other) {
	SpecializableVarDecl<isQualified> temp(other);
	std::swap(*this, other);
	return *this;
}

namespace std
{
	template<wake::TypeQualification isQualified>
	void swap(SpecializableVarDecl<isQualified>& lhs, SpecializableVarDecl<isQualified>& rhs)
	{
		std::swap(lhs.decl, rhs.decl);
		std::swap(lhs.specialty, rhs.specialty);
	}
}

SpecializableVarDecl<wake::UNQUALIFIED>* makeSpecializableVarDecl(PureType<wake::UNQUALIFIED>* type) {
	SpecializableVarDecl<wake::UNQUALIFIED>* spDecl = new SpecializableVarDecl<wake::UNQUALIFIED>();
	spDecl->decl.typedata = *type;
	return spDecl;
}

SpecializableVarDecl<wake::UNQUALIFIED>* makeSpecializableVarDeclFromVarDecl(VarDecl<wake::UNQUALIFIED>* decl) {
	SpecializableVarDecl<wake::UNQUALIFIED>* spDecl = new SpecializableVarDecl<wake::UNQUALIFIED>();
	spDecl->decl = *decl;
	return spDecl;
}

SpecializableVarDecl<wake::UNQUALIFIED>* copySpecializableVarDecl(SpecializableVarDecl<wake::UNQUALIFIED>* decl) {
	return new SpecializableVarDecl<wake::UNQUALIFIED>(*decl);
}

template struct SpecializableVarDecl<wake::QUALIFIED>;
template struct SpecializableVarDecl<wake::UNQUALIFIED>;
