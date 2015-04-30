/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionErrorCatcher.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/ExpressionErrorCatcher.h"
#include "TypeError.h"

PureType* wake::ast::ExpressionErrorCatcher::typeCheck(bool forceArrayIdentifier) {
	try {
		return child->typeCheck(forceArrayIdentifier);
	} catch(TypeError* error) {
		string errormsg = error->reason;
		errormsg += "; expected '";
		errormsg += error->expected;
		errormsg += "'; actual was '";
		errormsg += error->erroneous;
		errormsg += "'";

		errors->addError(new SemanticError(TYPE_ERROR, errormsg, node));
		delete error;
	}

	return new PureType(TYPE_MATCHALL);
}
