#ifndef HEADER_AST_FOR
#define HEADER_AST_FOR

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/ScopedSymbolTable.h"
#include "ast/TypeAnalyzer.h"

namespace wake {

	namespace ast {

		class For : public StatementNode {

			public:
				For(StatementNode* init, ExpressionNode* condition, StatementNode* incr, StatementNode* block, ScopedSymbolTable* scopesymtable, TypeAnalyzer* analyzer)
					: init(init), condition(condition), incr(incr), block(block), scopesymtable(scopesymtable), analyzer(analyzer);
				typeCheck(bool forceArrayIdentifier);

			private:
				StatementNode* init;
				ExpressionNode* condition;
				StatementNode* incr;
				StatementNode* block;
				ScopedSymbolTable* scopesymtable;
				TypeAnalyzer* analyzer;
		}

	}

}

#endif
