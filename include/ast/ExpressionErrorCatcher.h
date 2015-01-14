/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionErrorCatcher.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_EXPRESSION_ERROR_CATCHER
#define HEADER_AST_EXPRESSION_ERROR_CATCHER

#include "ast/ExpressionNode.h"

namespace wake {

	namespace ast {

		class ExpressionErrorCatcher : public ExpressionNode {

			public:
				ExpressionErrorCatcher(ExpressionNode* child, Node* node) : child(child), node(node) {};
				Type* typeCheck(bool forceArrayIdentifier);


			private:
				Node* node;
				ExpressionNode* child;

		};

	}

}

#endif
