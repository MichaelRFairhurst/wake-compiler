/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * UnsafeCast.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/UnsafeCast.h"
#include "CompilationExceptions.h"
#include <boost/ptr_container/ptr_vector.hpp>

using namespace wake;

PureType<QUALIFIED>* ast::UnsafeCast::typeCheck(bool forceArrayIdentifier) {
	try {
		PureType<QUALIFIED> value = *auto_ptr<PureType<QUALIFIED> >(valueExpr->typeCheck(false));
		// type check it, but then just return the casted type.
		classestable->assertTypeIsValid(castedType);
		return new PureType<QUALIFIED>(*castedType);
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
		return new PureType<QUALIFIED>(TYPE_MATCHALL);
	}
}
