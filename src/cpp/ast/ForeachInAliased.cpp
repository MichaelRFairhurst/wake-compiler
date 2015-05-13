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

using namespace wake;

VarDecl<QUALIFIED> ast::ForeachInAliased::getIterationVarDecl(PureType<QUALIFIED>* iterableType) {
	VarDecl<QUALIFIED> decl;
	decl.typedata = *iterableType;
	decl.alias = strdup(alias);
	return decl;
}
