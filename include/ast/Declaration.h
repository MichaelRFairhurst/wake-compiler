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

namespace wake {

	namespace ast {

		class Declaration : public StatementNode {

			public:
				Declaration(Type* declared, ExpressionNode* value, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, ErrorTracker* errors, const vector<Type*>& parameterizedtypes)
					: parameterizedtypes(parameterizedtypes);

				void typeCheck();

			private:
				Type* declared;
				ExpressionNode* value;
				ClassSpaceSymbolTable* classestable;
				ScopeSymbolTable* scopesymtable;
				ErrorTracker* errors;
				const vector<Type*>& parameterizedtypes;

		};

	}

}

#endif
