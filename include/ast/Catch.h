/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Catch.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_CATCH
#define HEADER_AST_CATCH

#include "ast/StatementNode.h"
#include "type.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "ErrorTracker.h"
#include "node.h"
#include <memory>

namespace wake {

	namespace ast {

		class Catch : public StatementNode {

			public:
				Catch(Type* exceptionType, StatementNode* body, Node* node, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, ErrorTracker* errors)
					: exceptionType(exceptionType), body(body), classestable(classestable), scopesymtable(scopesymtable), errors(errors), node(node) {};

				void typeCheck();

				~Catch(){};

			private:
				Type* exceptionType;
				std::auto_ptr<StatementNode> body;
				Node* node;
				ClassSpaceSymbolTable* classestable;
				ScopeSymbolTable* scopesymtable;
				ErrorTracker* errors;

		};

	}

}

#endif
