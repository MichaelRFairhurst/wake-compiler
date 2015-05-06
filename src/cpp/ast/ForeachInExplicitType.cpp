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

VarDecl wake::ast::ForeachInExplicitType::getIterationVarDecl(PureType* iterableType) {
	if(!analyzer->isASubtypeOfB(&explicitType->typedata, iterableType)) {
		EXPECTED	iterableType->toString()
		ERRONEOUS	explicitType->typedata.toString()
		THROW		("Declaration of item within foreach does not match the item's type");
	}

	VarDecl decl;
	decl.typedata = *iterableType;
	return decl; // preserve shadow and other goodies
}
