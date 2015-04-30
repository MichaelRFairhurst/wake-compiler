/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Throw.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Throw.h"
#include "TypeError.h"
#include <memory>

void wake::ast::Throw::typeCheck() {
	auto_ptr<PureType> exceptionType(exception->typeCheck(false));

	if(!analyzer->isException(exceptionType.get())) {
		EXPECTED	"Exception"
		ERRONEOUS	exceptionType->toString()
		THROW		("Can only throw subclasses of exception");
	}
}

bool wake::ast::Throw::exhaustiveReturns() {
	return true;
}
