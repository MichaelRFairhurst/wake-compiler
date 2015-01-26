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
#include <boost/ptr_container/ptr_vector.hpp>

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
	bool checkBody = true;

	int i = 0;
	for(std::vector<std::pair<boost::optional<std::string>, boost::optional<Type> > >::iterator it = arguments.begin(); it != arguments.end(); ++it, i++) {

		if(lambdaType->typedata.lambda.arguments == NULL) {
			lambdaType->typedata.lambda.arguments = MakeTypeArray();
		}

		if(it->second) {
			Type* type = &*it->second;
			AddTypeToTypeArray(new Type(type), lambdaType->typedata.lambda.arguments);
			scopesymtable->add(lambdaType->typedata.lambda.arguments->types[lambdaType->typedata.lambda.arguments->typecount - 1]);
		} else {
			std::string alias = *it->first;
			// try type inference
			if(hint == NULL) {
				checkBody = false;
				AddTypeToTypeArray(new Type(TYPE_MATCHALL), lambdaType->typedata.lambda.arguments);
				errors->addError(new SemanticError(TYPE_INFERENCE_FAILURE,
					"Cannot infer argument of name '"
					+ alias
					+ "': Lambda must be declared as an argument to a non-overloaded method in order to have a usable context.",
					node));
			} else if(hint->type != TYPE_LAMBDA || hint->typedata.lambda.arguments == NULL || hint->typedata.lambda.arguments->typecount <= i) {
				checkBody = false;
				AddTypeToTypeArray(new Type(TYPE_MATCHALL), lambdaType->typedata.lambda.arguments);
				errors->addError(new SemanticError(TYPE_INFERENCE_FAILURE,
					"Cannot infer argument of name '"
					+ alias
					+ "': The type hint is either not a lambda, or the type hinted lambda has fewer arguments",
					node));
			} else {
				AddTypeToTypeArray(new Type(hint->typedata.lambda.arguments->types[i]), lambdaType->typedata.lambda.arguments);
				lambdaType->typedata.lambda.arguments->types[lambdaType->typedata.lambda.arguments->typecount - 1]->alias = strdup(alias.c_str());
				scopesymtable->add(lambdaType->typedata.lambda.arguments->types[lambdaType->typedata.lambda.arguments->typecount - 1]);
			}
		}

	}

	if(checkBody) {
		body->typeCheck();
	}

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
