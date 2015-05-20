/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ClassVarRef.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ClassVarRef.h"

#include <cstdlib>
#include <string.h>

ClassVarRef::ClassVarRef(const ClassVarRef& other) {
	classname = strdup(other.classname);
	shadow = other.shadow;
	arrayed = other.arrayed;
}

ClassVarRef::~ClassVarRef() {
	free(classname);
}

ClassVarRef& ClassVarRef::operator=(ClassVarRef& other) {
	ClassVarRef temp(other);
	std::swap(*this, other);
	return *this;
}

ClassVarRef* makeClassVarRef(char* classname, int shadow, int arrayed) {
	return new ClassVarRef(classname, shadow, arrayed);
}

void freeClassVarRef(ClassVarRef* ref) {
	delete ref;
}

namespace std
{
	template<>
	void swap(ClassVarRef& lhs, ClassVarRef& rhs)
	{
		std::swap(lhs.classname, rhs.classname);
		std::swap(lhs.shadow, rhs.shadow);
		std::swap(lhs.arrayed, rhs.arrayed);
	}
}
