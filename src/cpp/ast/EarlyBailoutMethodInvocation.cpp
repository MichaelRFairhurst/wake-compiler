
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

Type* wake::ast::EarlyBailoutMethodInvocation::typeCheck(bool forceArrayIdentifier) {
	Type subject = *auto_ptr<Type>(subjectExpr->typeCheck(false));
	if(subject.type == TYPE_MATCHALL) {
		return new Type(subject);
	} else if(subject.type != TYPE_OPTIONAL) {
		errors->addError(new SemanticError(OPTIONAL_USE_OF_NONOPTIONAL_TYPE, "using .? on a nonoptional", node));
		return new Type(TYPE_MATCHALL);
	} else {
		Type* ret = new Type(TYPE_OPTIONAL);
		Type* nonoptional = subject.typedata.optional.contained;

		while(nonoptional->type == TYPE_OPTIONAL) {
			nonoptional = nonoptional->typedata.optional.contained;
		}

		ret->typedata.optional.contained = typeCheckMethodInvocation(*nonoptional);

		return ret;
	}
}
