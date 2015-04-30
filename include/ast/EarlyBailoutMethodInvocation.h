/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * EarlyBailoutMethodInvecation.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_EARLYBAILOUT_METHOD_INVOCATION
#define HEADER_AST_EARLYBAILOUT_METHOD_INVOCATION

#include "ast/MethodInvocationBase.h"
#include "ast/ExpressionNode.h"

namespace wake {

	namespace ast {

		class EarlyBailoutMethodInvocation : public MethodInvocationBase {

			public:
				EarlyBailoutMethodInvocation(ExpressionNode* subjectExpr, vector<MethodSegment*> methodSegments, Node* node, ClassSpaceSymbolTable* classestable, ErrorTracker* errors)
				: subjectExpr(subjectExpr), MethodInvocationBase(methodSegments, node, classestable, errors) {};

				PureType* typeCheck(bool forceArrayIdentifier);

			private:
				auto_ptr<ExpressionNode> subjectExpr;

		};

	}

}

#endif
