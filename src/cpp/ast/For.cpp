/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * For.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/For.h"
#include "TypeError.h"
#include <memory>

void wake::ast::For::typeCheck() {
	scopesymtable->pushScope();

	init->typeCheck();
	std::auto_ptr<Type> conditionType(condition->typeCheck(false));
	incr->typeCheck();
	block->typeCheck();

	scopesymtable->popScope();

	if(!analyzer->isPrimitiveTypeBool(conditionType.get())) {
		EXPECTED	"Bool"
		ERRONEOUS	analyzer->getNameForType(conditionType.get())
		THROW		("For conditions must be Bool");
	}
}
