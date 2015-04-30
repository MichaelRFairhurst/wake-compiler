/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ForeachInAliased.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/ForeachInAliased.h"

VarDecl wake::ast::ForeachInAliased::getIterationVarDecl(VarDecl* iterableType) {
	iterableType->alias = strdup(alias);
	return *iterableType;
}
