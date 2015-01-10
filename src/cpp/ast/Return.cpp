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
		if(value == NULL) break;

		auto_ptr<Type*> = value->typeCheck(false);

		EXPECTED	"VOID"
		ERRONEOUS	analyzer->getNameForType(ret)
		THROW		("Method is not allowed to return anything");
	}

	if(value == NULL) {
		EXPECTED	analyzer->getNameForType(returntype)
		ERRONEOS	"VOID"
		THROW		("Method is not allowed to return without a value");
	}

	auto_ptr<Type*> ret = value->typeCheck(false);

	if(!analyzer->isASubtypeOfB(ret.get(), returntype)) {
		EXPECTED	analyzer->getNameForType(returntype)
		ERRONEOUS	analyzer->getNameForType(ret.get())
		THROW		("Return type is incompatible with method signature");
	}
}
