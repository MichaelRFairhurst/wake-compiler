/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * VarRef.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_CLASS_VAR_REF
#define HEADER_CLASS_VAR_REF

#include "PureType.h"

#ifdef __cplusplus
#include <string>
#endif

struct ClassVarRef {
	char* classname;
	int shadow;
	int arrayed;

	#ifdef __cplusplus
		ClassVarRef(char* classname, int shadow, int arrayed) : classname(classname), shadow(shadow), arrayed(arrayed) {};
		ClassVarRef(const ClassVarRef& other);
		ClassVarRef& operator=(ClassVarRef& other);
		~ClassVarRef();

		std::string toString();
		bool canRepresentType(PureType<wake::QUALIFIED>& type);
	#endif

};

#ifdef __cplusplus
extern "C" {
#endif

	ClassVarRef* makeClassVarRef(char* classname, int shadow, int arrayed);
	void freeClassVarRef(ClassVarRef* ref);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<>
	void swap(ClassVarRef& lhs, ClassVarRef& rhs);
}
#endif

#endif
