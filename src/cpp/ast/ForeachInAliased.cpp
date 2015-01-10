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

Type* wake::ast::ForeachInAliased::getIterationType(Type* iterableType) {
	iterableType.alias = strdup(alias);
	return *iterableType;
}
