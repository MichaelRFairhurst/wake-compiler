/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * VarDecl.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_VAR_DECL
#define HEADER_VAR_DECL

#include "PureType.h"
#include "VarRef.h"

#ifdef __cplusplus
#define TEMPL_RECURSE <isQualified>
#define TEMPL_UNQUALIFIED <wake::UNQUALIFIED>
template<wake::TypeQualification isQualified>
#else
typedef
#endif
struct VarDecl {
	PureType TEMPL_RECURSE typedata;
	char* alias;
	int shadow;

#ifdef __cplusplus
	public:
		~VarDecl();
		VarDecl() : alias(NULL), shadow(0) {};
		VarDecl(const VarDecl<isQualified>& other);
		VarDecl<isQualified>& operator=(const VarDecl<isQualified>& other);

		VarRef createVarRef();
#endif
}

#ifndef __cplusplus
VarDecl;
#else
;

extern "C" {
#endif

VarDecl TEMPL_UNQUALIFIED* makeVarDecl(PureType TEMPL_UNQUALIFIED* innerType);
VarDecl TEMPL_UNQUALIFIED* copyVarDecl(VarDecl TEMPL_UNQUALIFIED* toBeCopied);
void freeVarDecl(VarDecl TEMPL_UNQUALIFIED* spt);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<wake::TypeQualification isQualified>
	void swap(VarDecl<isQualified>& lhs, VarDecl<isQualified>& rhs);
}
#endif

#endif
