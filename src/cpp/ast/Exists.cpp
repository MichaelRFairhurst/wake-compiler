/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Exists.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Exists.h"
#include "CompilationExceptions.h"
#include "tree.h"
#include <memory>

void wake::ast::Exists::typeCheck() {
	try {
		auto_ptr<Type> existableType(existable->typeCheck(false));

		if(node->node_data.nodes[0]->node_type == NT_MEMBER_ACCESS) {
			errors->addError(new SemanticError(TYPE_ERROR, "Calling exists { } on a property is illegal as it is a shared reference and therefore might be unset amid the scope", node));
			return;
		}

		if(existableType->type != TYPE_OPTIONAL) {
			errors->addError(new SemanticError(EXISTS_ON_NONOPTIONAL_TYPE, "exists { } statement uses a nonoptional type", node)); // @todo better error message!
			return;
		}

		Type real(TYPE_MATCHALL);
		real = *existableType->typedata.optional.contained;
		if(existableType->alias != NULL) real.alias = strdup(existableType->alias);

		Type* orig = *scopesymtable->find(existableType.get());
		scopesymtable->addOverwriting(&real);

		block->typeCheck();

		scopesymtable->addOverwriting(orig);

		if(otherwise.get() != NULL) {
			otherwise->typeCheck();
		}
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}
}

bool wake::ast::Exists::exhaustiveReturns() {
	if(otherwise.get() == NULL) {
		return false;
	}

	return block->exhaustiveReturns() && otherwise->exhaustiveReturns();
}
