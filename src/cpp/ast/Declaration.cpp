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

/*
wake::ast::Declaration(Type* declared, ExpressionNode* value, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, ErrorTracker* errors, const vector<Type*>& parameterizedtypes) {
	this->declared = declared;
	this->value = value;
	this->classes = classestable;
	this->scopesymtable = scopesymtable;
	this->errors = errors;
	this->parameterizedtypes = parameterizedtypes;
}*/

void wake::ast::Declaration::typeCheck(bool forceArrayIdentifier) {
	try {
		TypeParameterizer parameterizer;
		parameterizer.writeInParameterizations(&declared, parameterizedtypes);
		classestable->assertTypeIsValid(declared);
		Type assignment = *auto_ptr<Type>(value->check(false));
		if(!classestable->getAnalyzer()->isASubtypeOfB(&assignment, declared)) {
			EXPECTED	classestable->getAnalyzer->getNameForType(declared)
			ERRONEOUS	classestable->getAnalyzer->getNameForType(&assignment)
			THROW		("Invalid value in declaration of variable");
		}
		scopesymtable->add(declared);
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
		delete e;
	}
}
