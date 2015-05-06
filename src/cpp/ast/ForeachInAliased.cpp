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

VarDecl wake::ast::ForeachInAliased::getIterationVarDecl(PureType* iterableType) {
	VarDecl decl;
	decl.typedata = *iterableType;
	decl.alias = strdup(alias);
	return decl;
}
