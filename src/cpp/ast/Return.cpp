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

void wake::ast::Return::typeCheck() {
	if(returntype == NULL) {
		if(value.get() == NULL) return;

		auto_ptr<Type> actualReturn(value->typeCheck(false));

		EXPECTED	"VOID"
		ERRONEOUS	analyzer->getNameForType(actualReturn.get())
		THROW		("Method is not allowed to return anything");
	}

	if(value.get() == NULL) {
		EXPECTED	analyzer->getNameForType(returntype)
		ERRONEOUS	"VOID"
		THROW		("Method is not allowed to return without a value");
	}

	auto_ptr<Type> actualReturn(value->typeCheck(false));

	if(!analyzer->isASubtypeOfB(actualReturn.get(), returntype)) {
		EXPECTED	analyzer->getNameForType(returntype)
		ERRONEOUS	analyzer->getNameForType(actualReturn.get())
		THROW		("Return type is incompatible with method signature");
	}
}
