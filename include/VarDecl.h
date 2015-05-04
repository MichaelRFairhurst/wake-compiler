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

typedef struct VarDecl {
	PureType typedata;
	char* alias;
	int shadow;

#ifdef __cplusplus
	public:
		~VarDecl();
		VarDecl(){};
		VarDecl(const VarDecl& other);
		VarDecl& operator=(const VarDecl& other);

		VarRef createVarRef();

	private:
		void releaseData();
		void deepCopy(const VarDecl& other);
#endif
} VarDecl;

#ifdef __cplusplus
extern "C" {
#endif

VarDecl* makeVarDecl(PureType* innerType);
void freeVarDecl(VarDecl* spt);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<>
	void swap(VarDecl& lhs, VarDecl& rhs);
}
#endif

#endif
