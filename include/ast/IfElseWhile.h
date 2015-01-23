/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * IfElseWhile.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_IF_ELSE_WHILE
#define HEADER_AST_IF_ELSE_WHILE

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "TypeAnalyzer.h"
#include <memory>

namespace wake {

	namespace ast {

		class IfElseWhile : public StatementNode {

			public:
				IfElseWhile(ExpressionNode* condition, StatementNode* block, StatementNode* otherwise, TypeAnalyzer* analyzer) : condition(condition), block(block), otherwise(otherwise), analyzer(analyzer) {};

				void typeCheck();

				bool exhaustiveReturns();

				~IfElseWhile(){};

			private:
				std::auto_ptr<ExpressionNode> condition;
				std::auto_ptr<StatementNode> block;
				std::auto_ptr<StatementNode> otherwise;
				TypeAnalyzer* analyzer;

		};

	}

}

#endif
