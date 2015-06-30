/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeInferDeclaration.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_TYPE_INFER_DECLARATION
#define HEADER_AST_TYPE_INFER_DECLARATION

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "PureType.h"
#include <vector>
#include <memory>

namespace wake {

	namespace ast {

		class TypeInferDeclaration : public StatementNode {

			public:
				TypeInferDeclaration(char* declared, ExpressionNode* value, Node* node, ScopeSymbolTable* scopesymtable, ErrorTracker* errors)
					: declared(declared), value(value), node(node), scopesymtable(scopesymtable), errors(errors) {};

				void typeCheck();

				bool exhaustiveReturns();

				~TypeInferDeclaration(){};

			private:
				char* declared;
				std::auto_ptr<ExpressionNode> value;
				Node* node;
				ScopeSymbolTable* scopesymtable;
				ErrorTracker* errors;

		};

	}

}

#endif
