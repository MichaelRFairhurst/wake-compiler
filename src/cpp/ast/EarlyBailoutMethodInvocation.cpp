
/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * EarlyBailoutMethodInvecation.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/EarlyBailoutMethodInvocation.h"
#include "TypeError.h"

PureType* wake::ast::EarlyBailoutMethodInvocation::typeCheck(bool forceArrayIdentifier) {
	PureType subject = *auto_ptr<PureType>(subjectExpr->typeCheck(false));
	if(subject.type == TYPE_MATCHALL) {
		return new PureType(subject);
	} else if(subject.type != TYPE_OPTIONAL) {
		errors->addError(new SemanticError(OPTIONAL_USE_OF_NONOPTIONAL_TYPE, "using .? on a nonoptional", node));
		return new PureType(TYPE_MATCHALL);
	} else {
		PureType* ret = new PureType(TYPE_OPTIONAL);
		PureType* nonoptional = subject.typedata.optional.contained;

		while(nonoptional->type == TYPE_OPTIONAL) {
			nonoptional = nonoptional->typedata.optional.contained;
		}

		ret->typedata.optional.contained = typeCheckMethodInvocation(*nonoptional);

		return ret;
	}
}
