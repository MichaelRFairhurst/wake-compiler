/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * OtherStatement.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef AST_OTHER_STATEMENT
#define AST_OTHER_STATEMENT

#include <vector>
#include "ast/StatementNode.h"
#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"
#include "type.h"
#include "node.h"

namespace wake {

	namespace ast {

		class OtherStatement : public StatementNode {

			public:
				OtherStatement(Node* tree, std::vector<StatementNode*> children, ScopeSymbolTable* scopesymtable)
					: tree(tree), children(children), scopesymtable(scopesymtable) {};
				void typeCheck();

			private:
				Node* tree;
				std::vector<StatementNode*> children;
				ScopeSymbolTable* scopesymtable;

		};

	}

}

#endif
