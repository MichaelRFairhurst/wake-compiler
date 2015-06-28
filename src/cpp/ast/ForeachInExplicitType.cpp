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
	if(!explicitType->canRepresentType(*iterableType)) {
		EXPECTED	iterableType->toString()
		ERRONEOUS	explicitType->toString()
		THROW		("Declaration of item within foreach does not match the item's type");
	}

	VarDecl<QUALIFIED> decl;
	decl.typedata = *iterableType;
	decl.shadow = explicitType->shadow;
	return decl; // preserve shadow and other goodies
}
