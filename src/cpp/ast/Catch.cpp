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
		classestable->assertTypeIsValid(exception);

		if(!classestable->getAnalyzer()->isException(exception)) {
			EXPECTED	"exception subclass"
			ERRONEOUS	classestable->getAnalyzer()->getNameForType(exception)
			THROW		("Only exceptions can be thrown");
		}

		AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, strdup(exception->typedata._class.classname), node->loc));

		scopesymtable->pushScope();
		scopesymtable->add(exception);

		body->typeCheck();

		scopesymtable->popScope();
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}
}
