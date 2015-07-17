/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * CompileTargetPrecedenceConfigFactory.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "CompileTargetPrecedenceConfigFactory.h"

using namespace std;

CompileTargetPrecedenceConfig CompileTargetPrecedenceConfigFactory::java() {
	map<int, int> config;

	config[NT_ARRAY_ACCESS] = 0;
	config[NT_TYPESAFE_ARRAY_ACCESS] = 0;
	config[NT_ARRAY_ACCESS_LVAL] = 0;
	config[NT_MEMBER_ACCESS] = 0;
	config[NT_EARLYBAILOUT_MEMBER_ACCESS] = 0;
	config[NT_METHOD_INVOCATION] = 0;
	config[NT_EARLYBAILOUT_METHOD_INVOCATION] = 0;
	config[NT_RETRIEVAL] = 0;
	config[NT_LAMBDA_INVOCATION] = 0;
	// ++ config[NT_INVOCATION] = 0;
	// -- config[NT_INVOCATION] = 0;

	// unary + and - config[NT_INVOCATION] = 1;
	config[NT_INVERT] = 1;
	config[NT_BITNOT] = 1;

	config[NT_CAST] = 2;
	config[NT_UNSAFE_CAST] = 2;
	config[NT_AUTOBOX] = 2; // 'new'

	config[NT_MULTIPLY] = 3;
	config[NT_DIVIDE] = 3;
	config[NT_MOD] = 3;
	config[NT_MODNATIVE] = 3;
	config[NT_MODALT] = 3;

	config[NT_ADD] = 4;
	config[NT_SUBTRACT] = 4;

	config[NT_BITSHIFTLEFT] = 5;
	config[NT_BITSHIFTRIGHT] = 5;

	config[NT_GREATERTHAN] = 6;
	config[NT_GREATERTHANEQUAL] = 6;
	config[NT_LESSTHAN] = 6;
	config[NT_LESSTHANEQUAL] = 6;

	config[NT_EQUALITY] = 7;
	config[NT_INEQUALITY] = 7;

	config[NT_BITAND] = 8;

	config[NT_BITXOR] = 9;

	config[NT_BITOR] = 10;

	config[NT_AND] = 11;
	config[NT_OR] = 12;
	config[NT_IF_THEN_ELSE] = 13;

	config[NT_ASSIGNMENT] = 14;
	config[NT_ADD_ASSIGNMENT] = 14;
	config[NT_DIV_ASSIGNMENT] = 14;
	config[NT_MULT_ASSIGNMENT] = 14;
	config[NT_SUB_ASSIGNMENT] = 14;
	// binary= as well goes here

	config[NT_LAMBDA_DECLARATION] = -1;

	return CompileTargetPrecedenceConfig(config);
}
