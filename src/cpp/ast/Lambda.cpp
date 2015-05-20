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
#include "PureTypeArray.h"
#include "SemanticError.h"
#include <boost/ptr_container/ptr_vector.hpp>

using namespace wake;

PureType<QUALIFIED>* ast::Lambda::typeCheck(bool forceArrayIdentifier) {
	return typeCheckCommon(NULL);
}

PureType<QUALIFIED>* ast::Lambda::typeCheckExpecting(PureType<QUALIFIED>* hint) {
	return typeCheckCommon(hint);
}

PureType<QUALIFIED>* ast::Lambda::typeCheckCommon(PureType<QUALIFIED>* hint) {
	scopesymtable->pushScope();
	auto_ptr<PureType<QUALIFIED> > lambdaType(new PureType<QUALIFIED>(TYPE_LAMBDA));
	bool unknownSignature = false;

	int i = 0;
	for(std::vector<std::pair<boost::optional<std::string>, boost::optional<VarDecl<QUALIFIED> > > >::iterator it = arguments.begin(); it != arguments.end(); ++it, i++) {

		if(lambdaType->typedata.lambda.arguments == NULL) {
			lambdaType->typedata.lambda.arguments = new PureTypeArray<QUALIFIED>();
		}

		if(it->second) {
			VarDecl<QUALIFIED>* decl = &*it->second;
			lambdaType->typedata.lambda.arguments->addType(new PureType<QUALIFIED>(decl->typedata));
			scopesymtable->add(decl);
		} else {
			std::string alias = *it->first;
			// try type inference
			if(hint == NULL) {
				unknownSignature = true;
				lambdaType->typedata.lambda.arguments->addType(new PureType<QUALIFIED>(TYPE_MATCHALL));
				errors->addError(new SemanticError(TYPE_INFERENCE_FAILURE,
					"Cannot infer argument of name '"
					+ alias
					+ "': Lambda must be declared as an argument to a non-overloaded method in order to have a usable context.",
					node));
			} else if(hint->type != TYPE_LAMBDA || hint->typedata.lambda.arguments == NULL || hint->typedata.lambda.arguments->typecount <= i) {
				unknownSignature = true;
				lambdaType->typedata.lambda.arguments->addType(new PureType<QUALIFIED>(TYPE_MATCHALL));
				errors->addError(new SemanticError(TYPE_INFERENCE_FAILURE,
					"Cannot infer argument of name '"
					+ alias
					+ "': The type hint is either not a lambda, or the type hinted lambda has fewer arguments",
					node));
			} else {
				lambdaType->typedata.lambda.arguments->addType(new PureType<QUALIFIED>(*hint->typedata.lambda.arguments->types[i]));
				VarDecl<QUALIFIED> decl;
				decl.alias = strdup(alias.c_str());
				decl.typedata = *lambdaType->typedata.lambda.arguments->types[lambdaType->typedata.lambda.arguments->typecount - 1];
				scopesymtable->add(new VarDecl<QUALIFIED>(decl));
			}
		}

	}

	if(!unknownSignature) {
		body->typeCheck();
	}

	scopesymtable->popScope();

	if(returntype->getUnificationFailure1() != NULL) {
		errors->addError(new SemanticError(TYPE_INFERENCE_FAILURE,
			"Unification failed for lambda returned types: Partially inferred type "
			+ returntype->getUnificationFailure1()->toString()
			+ " and next type "
			+ returntype->getUnificationFailure2()->toString(),
			node));

		lambdaType->typedata.lambda.returntype = new PureType<QUALIFIED>(TYPE_MATCHALL);

	} else if(returntype->getCurrentUnification() && returntype->getCurrentUnification() != NULL && returntype->getCurrentUnification()->type != TYPE_UNUSABLE) {
		lambdaType->typedata.lambda.returntype = new PureType<QUALIFIED>(*returntype->getCurrentUnification());

		if(!body->exhaustiveReturns()) {
			errors->addError(new SemanticError(INEXHAUSTIVE_RETURNS, "Lambda declaration has an inferred return type, but not every execution path returns a value", node));
		}
	}

	if(unknownSignature) {
		return new PureType<QUALIFIED>(TYPE_MATCHALL);
	}

	return new PureType<QUALIFIED>(*lambdaType.get());
}
