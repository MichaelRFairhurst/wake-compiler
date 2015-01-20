/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * IfElseWhile.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/IfElseWhile.h"
#include "TypeError.h"
#include <memory>

void wake::ast::IfElseWhile::typeCheck() {
	auto_ptr<Type> conditionType(condition->typeCheck(false));
	block->typeCheck();

	if(otherwise.get() != NULL) {
		otherwise->typeCheck();
	}

	if(!analyzer->isPrimitiveTypeNum(conditionType.get()) && !analyzer->isPrimitiveTypeBool(conditionType.get())) {
		EXPECTED	"Bool"
		ERRONEOUS	analyzer->getNameForType(conditionType.get())
		THROW		("If/While conditions must be Bool or Num");
	}
}
