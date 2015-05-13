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

#ifdef __cplusplus
template<wake::TypeQualification isQualified>
#define TEMPL_RECURSE <isQualified>
#define TEMPL_UNQUALIFIED <wake::UNQUALIFIED>
#else
#define TEMPL_RECURSE
#define TEMPL_UNQUALIFIED
typedef
#endif
struct SpecializableVarDecl {
	VarDecl TEMPL_RECURSE decl;
	char* specialty;

#ifdef __cplusplus
	public:
		~SpecializableVarDecl();
		SpecializableVarDecl() : specialty(NULL) {};
		SpecializableVarDecl(SpecializableVarDecl<isQualified>& other);
		SpecializableVarDecl<isQualified>& operator=(SpecializableVarDecl<isQualified>& other);
};
#else
} SpecializableVarDecl;
#endif

#ifdef __cplusplus
extern "C" {
#endif

SpecializableVarDecl TEMPL_UNQUALIFIED* makeSpecializableVarDecl(PureType TEMPL_UNQUALIFIED* type);
SpecializableVarDecl TEMPL_UNQUALIFIED* makeSpecializableVarDeclFromVarDecl(VarDecl TEMPL_UNQUALIFIED* decl);
SpecializableVarDecl TEMPL_UNQUALIFIED* copySpecializableVarDecl(SpecializableVarDecl TEMPL_UNQUALIFIED* decl);
void freeSpecializableVarDecl(SpecializableVarDecl TEMPL_UNQUALIFIED* spt);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<wake::TypeQualification isQualified>
	void swap(SpecializableVarDecl<isQualified>& lhs, SpecializableVarDecl<isQualified>& rhs);
}
#endif

#endif
