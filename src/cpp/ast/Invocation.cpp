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
#include "PureTypeArray.h"
#include <boost/lexical_cast.hpp>

PureType* wake::ast::Invocation::typeCheck(bool forceArrayLiteral) {
	PureType lambda = *auto_ptr<PureType>(lambdaExpr->typeCheck(false));

	int realArgCount = 0;
	if(lambda.typedata.lambda.arguments) {
		realArgCount = lambda.typedata.lambda.arguments->typecount;
	}

	if(argumentExprs.size() != realArgCount) {
		EXPECTED	"function to be called with " + boost::lexical_cast<string>(realArgCount)
		ERRONEOUS	"function was called with " + boost::lexical_cast<string>(argumentExprs.size())
		THROW 		("Wrong number of arguments in lambda invocation");
	}

	int i = 0;
	for(boost::ptr_vector<ExpressionNode>::iterator it = argumentExprs.begin(); it != argumentExprs.end(); ++it, ++i) {
		auto_ptr<PureType> actual(it->typeCheck(false));
		if(!analyzer->isASubtypeOfB(actual.get(), lambda.typedata.lambda.arguments->types[i])) {
			EXPECTED	lambda.typedata.lambda.arguments->types[i]->toString()
			ERRONEOUS	actual->toString()
			THROW		("Argument lists not compatible in function invocation");
		}
	}

	return lambda.typedata.lambda.returntype ? new PureType(*lambda.typedata.lambda.returntype) : new PureType(TYPE_UNUSABLE);
}
