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
	free(alias);
}

VarDecl::VarDecl(const VarDecl& other) {
	typedata = other.typedata;
	alias = strdup(other.alias);
	shadow = other.shadow;
}

VarDecl& VarDecl::operator=(VarDecl& other) {
	VarDecl temp(other);
	std::swap(*this, other);
	return *this;
}

VarRef VarDecl::createVarRef() {
	if(alias != NULL) {
		return VarRef(alias);
	} else {
		ClassVarRef ref = typedata.createClassVarRef();
		ref.shadow = shadow;
		return VarRef(new ClassVarRef(ref));
	}
}
