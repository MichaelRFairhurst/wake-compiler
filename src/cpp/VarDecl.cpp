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

VarDecl::~VarDecl() {
	if(alias != NULL) {
		free(alias);
	}
}

VarDecl::VarDecl(const VarDecl& other) {
	typedata = other.typedata;
	alias = other.alias == NULL ? NULL : strdup(other.alias);
	shadow = other.shadow;
}

VarDecl& VarDecl::operator=(const VarDecl& other) {
	VarDecl temp(other);
	std::swap(*this, temp);
	return *this;
}

VarRef VarDecl::createVarRef() {
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
	template<>
	void swap(VarDecl& lhs, VarDecl& rhs)
	{
		std::swap(lhs.typedata, rhs.typedata);
		std::swap(lhs.alias, rhs.alias);
		std::swap(lhs.shadow, rhs.shadow);
	}
}

VarDecl* makeVarDecl(PureType* innerType) {
	VarDecl* decl = new VarDecl();
	decl->typedata = *innerType;
	return decl;
}

VarDecl* copyVarDecl(VarDecl* toBeCopied) {
	return new VarDecl(*toBeCopied);
}
