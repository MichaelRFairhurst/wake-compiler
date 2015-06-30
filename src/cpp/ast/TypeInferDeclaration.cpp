/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeInferDeclaration.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/TypeInferDeclaration.h"
#include "TypeError.h"
#include "CompilationExceptions.h"
#include <string.h>

using namespace wake;

void ast::TypeInferDeclaration::typeCheck() {
	try {
		PureType<QUALIFIED> assignment = *auto_ptr<PureType<QUALIFIED> >(value->typeCheck(false));
		VarDecl<QUALIFIED> decl;
		decl.typedata = assignment;
		decl.alias = strdup(declared);
		scopesymtable->add(new VarDecl<QUALIFIED>(decl));
	} catch(SemanticError* e) {
		e->token = node;
		errors->addError(e);
	}
}

bool ast::TypeInferDeclaration::exhaustiveReturns() {
	return false;
}
