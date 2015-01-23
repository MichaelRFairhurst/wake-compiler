/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionStatement.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_EXPRESSION_STATEMENT
#define HEADER_AST_EXPRESSION_STATEMENT

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include <memory>

namespace wake {

	namespace ast {

		class ExpressionStatement : public StatementNode {

			public:
				ExpressionStatement(ExpressionNode* expression) : expression(expression) {};

				void typeCheck();

				bool exhaustiveReturns();

				~ExpressionStatement(){};

			private:
				std::auto_ptr<ExpressionNode> expression;

		};

	}

}

#endif
