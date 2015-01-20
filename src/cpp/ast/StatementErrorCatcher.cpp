/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * StatementErrorCatcher.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/StatementErrorCatcher.h"
#include "TypeError.h"

void wake::ast::StatementErrorCatcher::typeCheck() {
	try {
		wrapped->typeCheck();
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
}
