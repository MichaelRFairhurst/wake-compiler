/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Declaration.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_DECLARATION
#define HEADER_AST_DECLARATION

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

		class Declaration : public StatementNode {

			public:
				Declaration(VarDecl** declared, ExpressionNode* value, Node* node, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, ErrorTracker* errors, const vector<PureType*>& parameterizedtypes)
					: declared(declared), value(value), node(node), classestable(classestable), scopesymtable(scopesymtable), errors(errors), parameterizedtypes(parameterizedtypes) {};

				void typeCheck();

				bool exhaustiveReturns();

				~Declaration(){};

			private:
				VarDecl** declared;
				std::auto_ptr<ExpressionNode> value;
				Node* node;
				ClassSpaceSymbolTable* classestable;
				ScopeSymbolTable* scopesymtable;
				ErrorTracker* errors;
				const vector<PureType*>& parameterizedtypes;

		};

	}

}

#endif
