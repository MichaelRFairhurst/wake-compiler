#ifndef HEADER_AST_IF_ELSE_WHILE
#define HEADER_AST_IF_ELSE_WHILE

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/TypeAnalyzer.h"

namespace wake {

	namespace ast {

		class IfElseWhile : public StatementNode {

			public:
				IfElseWhile(ExpressionNode* condition, StatementNode* block, StatementNode* otherwise, TypeAnalyzer* analyzer) : condition(condition, block(block), otherwise(otherwise), analyzer(analyzer);
				void typeCheck();

			private:
				ExpressionNode* condition;
				StatementNode* block;
				StatementNode* otherwise;
				TypeAnalyzer* analyzer;

		}

	}

}

#endif
