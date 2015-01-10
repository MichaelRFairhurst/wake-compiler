/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionUsableCatcher.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_EXPRESSION_USABLE_CATCHER
#define HEADER_AST_EXPRESSION_USABLE_CATCHER

#include "ast/ExpressionNode.h"
#include "ErrorTracker.h"

namespace wake {

	namespace ast {

		class ExpressionUsableCatcher : public ExpressionNode {

			public:
				ExpressionUsableCatcher(ExpressionNode* child, ErrorTracker* errors, Node* node) : child(child), errors(errors), node(node);
				Type* typeCheck(bool forceArrayIdentifier);


			private:
				Node* node;
				ExpressionNode* child;
				ErrorTracker* errors;

		};

	}

}

#endif
