/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Foreach.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Foreach.h"
#include "TypeError.h"

void wake::ast::Foreach::typeCheck() {
	auto_ptr<Type> list(iterable->typeCheck(false));

	if(list->type == TYPE_OPTIONAL) {
		errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Iterating over optional type. You must first wrap object in an exists { } clause.", node));
		return;
	}

	// no sense checking the contents of the loop if we're missing a variable
	if(list->type == TYPE_MATCHALL) return;

	if(list->type != TYPE_LIST) {
		errors->addError(new SemanticError(TYPE_ERROR, "Calling foreach over something that is not a list", node));
	} else {
		Type lowered(*list->typedata.list.contained);

		if(lowered.alias != NULL) free(lowered.alias);
		Type actualLowered = getIterationType(&lowered);

		scopesymtable->pushScope();
		scopesymtable->add(&actualLowered);
		body->typeCheck();
		scopesymtable->popScope();

		AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, strdup(scopesymtable->getNameForType(&actualLowered).c_str()), node->loc));
	}
}

Type wake::ast::Foreach::getIterationType(Type* iterableType) {
	iterableType->alias = NULL;
	return *iterableType;
}
