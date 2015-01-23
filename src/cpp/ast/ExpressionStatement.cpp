/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionStatement.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/ExpressionStatement.h"

void wake::ast::ExpressionStatement::typeCheck() {
	delete expression->typeCheck(false);
}

bool wake::ast::ExpressionStatement::exhaustiveReturns() {
	return false;
}
