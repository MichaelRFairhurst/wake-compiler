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
	auto_ptr<PureType> conditionType(condition->typeCheck(false));
	block->typeCheck();

	if(otherwise.get() != NULL) {
		otherwise->typeCheck();
	}

	if(!analyzer->isPrimitiveTypeNum(conditionType.get()) && !analyzer->isPrimitiveTypeBool(conditionType.get())) {
		EXPECTED	"Bool"
		ERRONEOUS	conditionType->toString()
		THROW		("If/While conditions must be Bool or Num");
	}
}

bool wake::ast::IfElseWhile::exhaustiveReturns() {
	if(otherwise.get() == NULL) {
		return false;
	}

	return block->exhaustiveReturns() && otherwise->exhaustiveReturns();
}
