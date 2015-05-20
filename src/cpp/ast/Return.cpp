/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Return.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Return.h"
#include "TypeError.h"
#include <memory>

using namespace wake;

void ast::Return::typeCheck() {
	if(returntype == NULL) {
		if(value.get() == NULL) return;

		auto_ptr<PureType<QUALIFIED> > actualReturn(value->typeCheck(false));

		EXPECTED	"VOID"
		ERRONEOUS	actualReturn->toString()
		THROW		("Method is not allowed to return anything");
	}

	if(value.get() == NULL) {
		EXPECTED	returntype->toString()
		ERRONEOUS	"VOID"
		THROW		("Method is not allowed to return without a value");
	}

	auto_ptr<PureType<QUALIFIED> > actualReturn(value->typeCheck(false));

	if(!analyzer->isASubtypeOfB(actualReturn.get(), returntype)) {
		EXPECTED	returntype->toString()
		ERRONEOUS	actualReturn->toString()
		THROW		("Return type is incompatible with method signature");
	}
}

bool ast::Return::exhaustiveReturns() {
	return true;
}
