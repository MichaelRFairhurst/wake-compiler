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

using namespace wake;

VarDecl<QUALIFIED> ast::ForeachInExplicitType::getIterationVarDecl(PureType<QUALIFIED>* iterableType) {
	if(!analyzer->isASubtypeOfB(&explicitType->typedata, iterableType)) {
		EXPECTED	iterableType->toString()
		ERRONEOUS	explicitType->typedata.toString()
		THROW		("Declaration of item within foreach does not match the item's type");
	}

	VarDecl<QUALIFIED> decl;
	decl.typedata = *iterableType;
	return decl; // preserve shadow and other goodies
}
