/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Lambda.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Lambda.h"
#include "TypeError.h"
#include "SemanticError.h"

Type* wake::ast::Lambda::typeCheck(bool forceArrayIdentifier) {
	return typeCheckCommon(NULL);
}

bool wake::ast::Lambda::typeCheckExpecting(Type* hint) {
	auto_ptr<Type> discovered(typeCheckCommon(hint));
	return analyzer->isASubtypeOfB(discovered.get(), hint);
}

Type* wake::ast::Lambda::typeCheckCommon(Type* hint) {
	scopesymtable->pushScope();
	auto_ptr<Type> lambdaType(new Type(TYPE_LAMBDA));

	for(std::vector<std::pair<boost::optional<std::string>, boost::optional<Type> > >::iterator it = arguments.begin(); it != arguments.end(); ++it) {

		if(lambdaType->typedata.lambda.arguments == NULL) {
			lambdaType->typedata.lambda.arguments = MakeTypeArray();
		}

		if(it->second) {
			boost::optional<Type> type(*it->second);
			scopesymtable->add(new Type(*type));
			AddTypeToTypeArray(new Type(*type), lambdaType->typedata.lambda.arguments);
		} else {
			boost::optional<std::string> alias(*it->first);
			// try type inference
		}
	}

	body->typeCheck();

	scopesymtable->popScope();

	if(returntype->getUnificationFailure1() != NULL) {
		errors->addError(new SemanticError(TYPE_INFERENCE_FAILURE,
			"Unification failed for lambda returned types: Partially inferred type "
			+ analyzer->getNameForType(returntype->getUnificationFailure1())
			+ " and next type "
			+ analyzer->getNameForType(returntype->getUnificationFailure2()),
			node));

		lambdaType->typedata.lambda.returntype = new Type(TYPE_MATCHALL);

	} else if(returntype->getCurrentUnification() && returntype->getCurrentUnification() != NULL && returntype->getCurrentUnification()->type != TYPE_UNUSABLE) {
		lambdaType->typedata.lambda.returntype = new Type(returntype->getCurrentUnification());

		if(!body->exhaustiveReturns()) {
			errors->addError(new SemanticError(INEXHAUSTIVE_RETURNS, "Lambda declaration has an inferred return type, but not every execution path returns a value", node));
		}
	}

	return new Type(lambdaType.get());
}
