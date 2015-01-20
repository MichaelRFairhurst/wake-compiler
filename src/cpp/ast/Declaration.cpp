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
		parameterizer.writeInParameterizations(declared, parameterizedtypes);
		classestable->assertTypeIsValid(*declared);
		Type assignment = *auto_ptr<Type>(value->typeCheck(false));
		if(!classestable->getAnalyzer()->isASubtypeOfB(&assignment, *declared)) {
			EXPECTED	classestable->getAnalyzer()->getNameForType(*declared)
			ERRONEOUS	classestable->getAnalyzer()->getNameForType(&assignment)
			THROW		("Invalid value in declaration of variable");
		}
		scopesymtable->add(*declared);
	} catch(SemanticError* e) {
		e->token = node;
		errors->addError(e);
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}
}
