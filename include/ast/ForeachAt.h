/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ForeachAt.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_FOREACH_AT
#define HEADER_AST_FOREACH_AT

#include "ast/Foreach.h"
#include "ast/ExpressionNode.h"
#include "ScopeSymbolTable.h"
#include "ErrorTracker.h"
#include "VarRef.h"
#include <memory>

#include "tree.h"

namespace wake {

	namespace ast {

		class ForeachAt : public StatementNode {

			public:
				ForeachAt(Foreach* base, VarRef var_ref, Node* node, ScopeSymbolTable* scopesymtable, ErrorTracker* errors)
					: base(base), var_ref(var_ref), node(node), scopesymtable(scopesymtable), errors(errors) {};

				void typeCheck();

				bool exhaustiveReturns();

			private:
				std::auto_ptr<Foreach> base;
				VarRef var_ref;
				Node* node;
				ScopeSymbolTable* scopesymtable;
				ErrorTracker* errors;

		};

	}

}

#endif
