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

std::string ClassVarRef::toString() {
	std::string ret = "";
	for(int i = 0; i < shadow; ++i) {
		ret += "$";
	}

	ret += classname;

	for(int i = 0; i < arrayed; ++i) {
		ret += "[]";
	}

	return ret;
}

bool ClassVarRef::canRepresentType(PureType<wake::QUALIFIED>& type) {
	switch(type.type) {
		case TYPE_LIST:
			{
				if(!arrayed) {
					return false;
				}

				ClassVarRef lowered = *this;
				lowered.arrayed--;

				return lowered.canRepresentType(*type.typedata.list.contained);
			}

		case TYPE_LAMBDA:
			return false;

		case TYPE_OPTIONAL:
			return canRepresentType(*type.typedata.optional.contained);

		case TYPE_CLASS:
			return std::string(classname) == type.typedata._class.classname;
	}
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
