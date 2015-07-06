/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * node.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_NODE
#define HEADER_NODE

#include "wakelltype.h"
#include "PureType.h"
#include "PureTypeArray.h"
#include "VarDecl.h"
#include "SpecializableVarDecl.h"
#include "SpecializablePureType.h"

#ifdef __cplusplus
extern "C" {
#define TEMPL_UNQUALIFIED <wake::UNQUALIFIED>
#define STRUCT
#else
#define TEMPL_UNQUALIFIED
#define STRUCT struct
#endif

typedef
struct Node {
	int line;
	int col;
	YYLTYPE loc;
	int node_type;
	int subnodes;
	union {
		char* string;
		double number;
		int integer;
		struct Node** nodes;
		STRUCT PureType TEMPL_UNQUALIFIED* pure_type;
		STRUCT PureTypeArray TEMPL_UNQUALIFIED* pure_type_array;
		struct ClassVarRef* class_var_ref;
		struct VarRef* var_ref;
		STRUCT VarDecl TEMPL_UNQUALIFIED* var_decl;
		STRUCT SpecializableVarDecl TEMPL_UNQUALIFIED* specializable_var_decl;
		STRUCT SpecializablePureType TEMPL_UNQUALIFIED* specializable_pure_type;
	} node_data;
} Node;

#ifdef __cplusplus
}
#endif
#endif
