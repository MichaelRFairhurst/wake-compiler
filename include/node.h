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
		float number;
		struct Node** nodes;
		struct Type* type;
		struct TypeArray* typearray;
	} node_data;
} Node;

#endif
