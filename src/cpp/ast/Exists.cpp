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
#include <memory>

void wake::ast::Exists::typeCheck() {
	try {
		auto_ptr<Type> existableType(existable->typeCheck(false));

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

		if(otherwise != NULL) {
			otherwise->typeCheck();
		}
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}
}
