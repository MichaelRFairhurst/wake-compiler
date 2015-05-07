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

typedef struct Node {
	int line;
	int col;
	YYLTYPE loc;
	int node_type;
	int subnodes;
	union {
		char* string;
		double number;
		struct Node** nodes;
		struct PureType* pure_type;
		struct PureTypeArray* pure_type_array;
		struct ClassVarRef* class_var_ref;
		struct VarRef* var_ref;
		struct VarDecl* var_decl;
		struct SpecializableVarDecl* specializable_var_decl;
		struct SpecializablePureType* specializable_pure_type;
	} node_data;
} Node;

#endif
