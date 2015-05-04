/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * SpecializableVarDecl.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_SPECIALIZABLE_VAR_DECL
#define HEADER_SPECIALIZABLE_VAR_DECL

#include "VarDecl.h"

typedef struct SpecializableVarDecl {
	VarDecl decl;
	char* specialty;

#ifdef __cplusplus
	public:
		~SpecializableVarDecl();
		SpecializableVarDecl(){};
		SpecializableVarDecl(SpecializableVarDecl& other);
		SpecializableVarDecl& operator=(SpecializableVarDecl& other);
	private:
		void releaseData();
		void deepCopy(const SpecializableVarDecl& other);
#endif
} SpecializableVarDecl;

#ifdef __cplusplus
extern "C" {
#endif

SpecializableVarDecl* makeSpecializableVarDecl(PureType* type);
void freeSpecializableVarDecl(SpecializableVarDecl* spt);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<>
	void swap(SpecializableVarDecl& lhs, SpecializableVarDecl& rhs);
}
#endif

#endif
