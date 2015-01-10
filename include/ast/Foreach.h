#ifndef HEADER_AST_FOREACH
#define HEADER_AST_FOREACH

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "tree.h"
#include "ScopedSymbolTable.h"

namespace wake {

	namespace ast {

		class Foreach : public StatementNode {

			public:
				Foreach(ExpressionNode* iterable, StatementNode* body, Node* node, ScopeSymbolTable* scopesymtable)
					: iterable(iterable), body(body), node(node), scopesymtable(scopesymtable);

				virtual typeCheck();

				virtual Type getIterationType(Type* iterableType);

			private:
				ExpressionNode* iterable;
				StatementNode* body;
				Node* node;
				ScopeSymbolTable* scopesymtable;

		}

	}

}

#endif
