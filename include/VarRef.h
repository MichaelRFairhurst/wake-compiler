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

#ifndef HEADER_VAR_REF
#define HEADER_VAR_REF

#ifdef __cplusplus
#include <string>
#endif

#include "ClassVarRef.h"

typedef struct VarRef {
	char* alias;
	ClassVarRef* _class;

	#ifdef __cplusplus
		VarRef(char* alias) : alias(alias), _class(0) {};
		VarRef(ClassVarRef* _class) : alias(0), _class(_class) {};
		VarRef(char* alias, ClassVarRef* _class) : alias(alias), _class(_class) {};
		VarRef(const VarRef& other);
		VarRef& operator=(VarRef& other);
		std::string toString();

		~VarRef();
	#endif

} VarRef;

#ifdef __cplusplus
extern "C" {
#endif

	ClassVarRef* makeClassVarRef(char* classname, int shadow, int arrayed);
	VarRef* makeVarRefFromClass(ClassVarRef* _class);
	VarRef* makeVarRefFromAlias(char* alias);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<>
	void swap(VarRef& lhs, VarRef& rhs)
	{
		std::swap(lhs.alias, rhs.alias);
		std::swap(lhs._class, rhs._class);
	}
}
#endif

#endif
