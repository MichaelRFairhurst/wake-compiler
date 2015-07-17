/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * CompileTargetPrecedenceConfig.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "CompileTargetPrecedenceConfig.h"

using namespace std;

bool CompileTargetPrecedenceConfig::binAssoc(int type) {
	return type != NT_SUBTRACT && type != NT_DIVIDE && type != NT_MOD && type != NT_MODNATIVE && type != NT_MODALT && type != NT_BITSHIFTLEFT && type != NT_BITSHIFTRIGHT;
}

/**
 * Note on ternary operator associativity. In wake, the left side is not associative, ie,
 *
 *   true if false else true if false else true
 *
 * is equivalent to
 *
 *   true if false else (true if false else true)
 *
 * Since the middle is unambiguous, its the left side that requires parens IN WAKE
 *
 *   (true if false else true) if false else true
 *
 * BUT
 *
 * in js/java/etc, that "troubled" example compiles to
 *
 *   false ? (false ? true : true) : true
 *
 * which doesn't require parens! Since this is all about our target language,
 * we're really looking for
 *
 *   true if (false if true else true) else true
 *
 * so we know to insert the parens in the left of our generated code
 *
 *   (true ? false : true) ? false : true
 *
 * Note that this must be configurable as python & PHP require different behavior.
 * Python matches wake while PHP has broken right associativity.
 */
bool CompileTargetPrecedenceConfig::triAssocLeft(int type) {
	return true;
}

bool CompileTargetPrecedenceConfig::triAssocMiddle(int type) {
	return false;
}

bool CompileTargetPrecedenceConfig::triAssocRight(int type) {
	return true;
}

int CompileTargetPrecedenceConfig::getPrec(int type) {
	return brain[type];
}

bool CompileTargetPrecedenceConfig::hasPrec(int type) {
	return brain.count(type) == 1;
}
