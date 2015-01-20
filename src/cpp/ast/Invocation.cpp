/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Invocation.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Invocation.h"
#include "TypeError.h"

Type* wake::ast::Invocation::typeCheck(bool forceArrayLiteral) {
	Type lambda = *auto_ptr<Type>(lambdaExpr->typeCheck(false));
	Type actual(TYPE_LAMBDA);
	actual.typedata.lambda.arguments = MakeTypeArray();
	if(lambda.typedata.lambda.returntype != NULL) {
		actual.typedata.lambda.returntype = new Type(*lambda.typedata.lambda.returntype);
	} else {
		return new Type(TYPE_UNUSABLE);
	}

	for(boost::ptr_vector<ExpressionNode>::iterator it = argumentExprs.begin(); it != argumentExprs.end(); ++it) {
		AddTypeToTypeArray(it->typeCheck(false), actual.typedata.lambda.arguments);
	}

	if(!analyzer->isASubtypeOfB(&actual, &lambda)) {
		EXPECTED	analyzer->getNameForType(&lambda)
		ERRONEOUS	analyzer->getNameForType(&actual)
		THROW		("Argument lists not compatible in function invocation");
	}

	return new Type(*lambda.typedata.lambda.returntype);
}
