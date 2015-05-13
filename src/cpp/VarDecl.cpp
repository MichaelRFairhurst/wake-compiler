/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * VarDecl.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "VarDecl.h"

#include <cstdlib>
#include <string.h>

template<wake::TypeQualification isQualified>
VarDecl<isQualified>::~VarDecl() {
	if(alias != NULL) {
		free(alias);
	}
}

template<wake::TypeQualification isQualified>
VarDecl<isQualified>::VarDecl(const VarDecl<isQualified>& other) {
	typedata = other.typedata;
	alias = other.alias == NULL ? NULL : strdup(other.alias);
	shadow = other.shadow;
}

template<wake::TypeQualification isQualified>
VarDecl<isQualified>& VarDecl<isQualified>::operator=(const VarDecl<isQualified>& other) {
	VarDecl<isQualified> temp(other);
	std::swap(*this, temp);
	return *this;
}

template<wake::TypeQualification isQualified>
VarRef VarDecl<isQualified>::createVarRef() {
	if(alias != NULL) {
		return VarRef(strdup(alias));
	} else {
		ClassVarRef ref = typedata.createClassVarRef();
		ref.shadow = shadow;
		return VarRef(new ClassVarRef(ref));
	}
}

namespace std
{
	template<wake::TypeQualification isQualified>
	void swap(VarDecl<isQualified>& lhs, VarDecl<isQualified>& rhs)
	{
		std::swap(lhs.typedata, rhs.typedata);
		std::swap(lhs.alias, rhs.alias);
		std::swap(lhs.shadow, rhs.shadow);
	}
}

VarDecl<wake::UNQUALIFIED>* makeVarDecl(PureType<wake::UNQUALIFIED>* innerType) {
	VarDecl<wake::UNQUALIFIED>* decl = new VarDecl<wake::UNQUALIFIED>();
	decl->typedata = *innerType;
	return decl;
}

VarDecl<wake::UNQUALIFIED>* copyVarDecl(VarDecl<wake::UNQUALIFIED>* toBeCopied) {
	return new VarDecl<wake::UNQUALIFIED>(*toBeCopied);
}
