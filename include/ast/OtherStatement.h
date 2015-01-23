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

#include "ast/StatementNode.h"
#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"
#include "type.h"
#include "node.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>

namespace wake {

	namespace ast {

		class OtherStatement : public StatementNode {

			public:
				OtherStatement(Node* tree, std::vector<StatementNode*> children, ScopeSymbolTable* scopesymtable)
					: tree(tree), scopesymtable(scopesymtable) {
						for(std::vector<StatementNode*>::iterator it = children.begin(); it != children.end(); ++it) this->children.push_back(*it);
					};

				void typeCheck();

				bool exhaustiveReturns();

				~OtherStatement(){};

			private:
				Node* tree;
				boost::ptr_vector<StatementNode> children;
				ScopeSymbolTable* scopesymtable;

		};

	}

}

#endif
