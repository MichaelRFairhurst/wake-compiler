#ifndef HEADER_FOREACH_IN_EXPLICIT_TYPE
#define HEADER_FOREACH_IN_EXPLICIT_TYPE

#include "ast/Foreach.h"
#include "TypeAnalyzer.h"

namespace wake {

	namespace ast {

		class ForeachInExplicitType : public Foreach {

			public:
				ForeachInExplicitType(ExpressionNode* iterable, StatementNode* body, Type* explicitType, Node* node, ScopeSymbolTable* scopesymtable, TypeAnalyzer* analyzer)
					: Foreach(iterable, body, node, scopesymtable), analyzer(analyzer), explicitType(explicitType);

				typeCheck();

				Type getIterationType(Type* iterableType);

			private:
				Type* explicitType;
				TypeAnalyzer* analyzer;

		}

	}

}

#endif
