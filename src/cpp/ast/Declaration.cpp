/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Declaration.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Declaration.h"
#include "TypeParameterizer.h"
#include "TypeError.h"
#include "CompilationExceptions.h"

void wake::ast::Declaration::typeCheck() {
	try {
		TypeParameterizer parameterizer;
		parameterizer.rewriteClasstypesToParameterizedtypeByLabel(&declared->typedata, parameterizedtypes);
		classestable->assertTypeIsValid(&declared->typedata);
		PureType assignment = *auto_ptr<PureType>(value->typeCheck(false));
		if(!classestable->getAnalyzer()->isASubtypeOfB(&assignment, &declared->typedata)) {
			EXPECTED	declared->typedata.toString()
			ERRONEOUS	assignment.toString()
			THROW		("Invalid value in declaration of variable");
		}
		scopesymtable->add(declared);
	} catch(SemanticError* e) {
		e->token = node;
		errors->addError(e);
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}
}

bool wake::ast::Declaration::exhaustiveReturns() {
	return false;
}
