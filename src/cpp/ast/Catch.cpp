/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Catch.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Catch.h"
#include "TypeError.h"
#include "tree.h"
#include "SemanticError.h"
#include "CompilationExceptions.h"

void wake::ast::Catch::typeCheck() {
	try {
		classestable->assertTypeIsValid(&exceptionType->typedata);

		if(!classestable->getAnalyzer()->isException(&exceptionType->typedata)) {
			EXPECTED	"exceptionType subclass"
			ERRONEOUS	exceptionType->typedata.toString()
			THROW		("Only exceptions can be thrown");
		}

		addSubNode(node, makeNodeFromString(NT_COMPILER_HINT, strdup(exceptionType->typedata.typedata._class.classname), node->loc));

		scopesymtable->pushScope();
		scopesymtable->add(exceptionType);

		body->typeCheck();

		scopesymtable->popScope();
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}
}

bool wake::ast::Catch::exhaustiveReturns() {
	return body->exhaustiveReturns();
}
