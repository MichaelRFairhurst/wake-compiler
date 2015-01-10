/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ForeachInExplicitType.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/ForeachInExplicitType.h"
#include "TypeError.h"

Type wake::ast::ForeachInExplicitType::getIterationType(Type* iterableType) {
	iterableType->alias = NULL;

	if(!analyzer->isASubtypeOfB(explicitType, iterableType)) {
		EXPECTED	analyzer->getNameForType(iterableType)
		ERRONEOUS	analyzer->getNameForType(explicitType)
		THROW		("Declaration of item within foreach does not match the item's type");
	}

	return *explicitType; // preserve shadow and other goodies
}
