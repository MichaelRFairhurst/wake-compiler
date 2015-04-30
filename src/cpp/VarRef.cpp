/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * VarRef.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "VarRef.h"

#include <cstdlib>
#include <string.h>

VarRef::~VarRef() {
	free(alias);
	free(_class);
}

VarRef::VarRef(const VarRef& other) {
	_class = new ClassVarRef(*other._class);
	alias = strdup(other.alias);
}

VarRef& VarRef::operator=(VarRef& other) {
	VarRef temp(other);
	std::swap(*this, other);
	return *this;
}

std::string VarRef::toString() {
	if(alias != NULL) {
		return alias;
	} else {
		std::string name = std::string(_class->shadow, '$') + _class->classname;
		for (int i = 0; i < _class->arrayed; i++) {
			name += "[]";
		}
		return name;
	}
}

VarRef* makeVarRefFromClass(ClassVarRef* _class) {
	return new VarRef(_class);
}

VarRef* makeVarRefFromAlias(char* alias) {
	return new VarRef(alias);
}

namespace std
{
	template<>
	void swap(VarRef& lhs, VarRef& rhs)
	{
		std::swap(lhs.alias, rhs.alias);
		std::swap(lhs._class, rhs._class);
	}
}
