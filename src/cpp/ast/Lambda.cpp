/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Lambda.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Lambda.h"

Type* wake::ast::Lambda::typeCheck(bool forceArrayIdentifier) {
	scopesymtable->pushScope();
	auto_ptr<Type> lambdaType(new Type(TYPE_LAMBDA));

	for(std::vector<std::pair<boost::optional<std::string>, boost::optional<Type> > >::iterator it = arguments.begin(); it != arguments.end(); ++it) {

		if(lambdaType->typedata.lambda.arguments == NULL) {
			lambdaType->typedata.lambda.arguments = MakeTypeArray();
		}

		boost::optional<Type> type = it->second;
		boost::optional<std::string> alias = it->first;

		if(type) {
			scopesymtable->add(&*type);
			AddTypeToTypeArray(new Type(*type), lambdaType->typedata.lambda.arguments);
		} else {
			// try type inference
		}
	}

	body->typeCheck();

	scopesymtable->popScope();

	return new Type(lambdaType.get());
}
