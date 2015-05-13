/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * LambdaReturn.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/LambdaReturn.h"
#include "TypeError.h"
#include <memory>

using namespace wake;

void ast::LambdaReturn::typeCheck() {
	if(value.get() == NULL) {
		auto_ptr<PureType<QUALIFIED> > unusable(new PureType<QUALIFIED>(TYPE_UNUSABLE));
		returntype->unifyWith(unusable.get());
	} else {
		auto_ptr<PureType<QUALIFIED> > returnType(value->typeCheck(false));
		returntype->unifyWith(returnType.get());
	}
}

bool ast::LambdaReturn::exhaustiveReturns() {
	return true;
}
