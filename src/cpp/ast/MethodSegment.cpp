/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MethodSegment.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/MethodSegment.h"
#include "ast/MethodSegment.h"

wake::ast::MethodSegment::~MethodSegment() {
	for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator it = arguments.begin(); it != arguments.end(); ++it)
	if(it->first) {
		delete it->first;
	} else {
		delete it->second;
	}
}
