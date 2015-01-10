/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ForeachInAliased.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_FOREACH_IN_ALIASED
#define HEADER_FOREACH_IN_ALIASED

#include "ast/Foreach.h"

namespace wake {

	namespace ast {

		class ForeachInAliased : public Foreach {

			public:
				ForeachInAliased(ExpressionNode* iterable, StatementNode* body, char* alias, Node* node, ScopeSymbolTable* scopesymtable, ErrorTracker* errors)
					: Foreach(iterable, body, node, scopesymtable, errors), alias(alias) {};

				void typeCheck();

				Type getIterationType(Type* iterableType);

			private:
				char* alias;

		};

	}

}

#endif
