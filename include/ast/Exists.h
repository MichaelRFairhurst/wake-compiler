/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Exists.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_EXISTS
#define HEADER_AST_EXISTS

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ScopedSymbolTable.h"
#include "ErrorTracker.h"

namespace wake {

	namespace ast {

		class Exists : public StatementNode {

			public:
				Exists(ExpressionNode* existable, StatementNode* block, StatementNode* otherwise, ScopeSymbolTable* scopesymtable, ErrorTracker* errors)
					: existable(existable), block(block), otherwise(otherwise), scopesymtable(scopesymtable), errors(errors);
				typeCheck();

			private:
				ExpressionNode* existable;
				StatementNode* block;
				StatementNode* otherwise;
				ScopeSymbolTable* scopesymtable;
				ErrorTracker* errors;

		};

	}

}

#endif
