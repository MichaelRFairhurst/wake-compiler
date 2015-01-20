/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * For.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_FOR
#define HEADER_AST_FOR

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ScopeSymbolTable.h"
#include "TypeAnalyzer.h"
#include <memory>

namespace wake {

	namespace ast {

		class For : public StatementNode {

			public:
				For(StatementNode* init, ExpressionNode* condition, StatementNode* incr, StatementNode* block, ScopeSymbolTable* scopesymtable, TypeAnalyzer* analyzer)
					: init(init), condition(condition), incr(incr), block(block), scopesymtable(scopesymtable), analyzer(analyzer) {};

				void typeCheck();

				~For(){};

			private:
				std::auto_ptr<StatementNode> init;
				std::auto_ptr<ExpressionNode> condition;
				std::auto_ptr<StatementNode> incr;
				std::auto_ptr<StatementNode> block;
				ScopeSymbolTable* scopesymtable;
				TypeAnalyzer* analyzer;
		};

	}

}

#endif
