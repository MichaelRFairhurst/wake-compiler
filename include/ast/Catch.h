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
#include "Type.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopedSymbolTable.h"
#include "ErrorTracker.h"
#include "node.h"

namespace wake {

	namespace ast {

		class Catch : public StatementNode {

			public:
				Catch(Type* exception, StatementNode* body, Node* node, ClassSpaceSymbolTable* classsymtable, ScopedSymbolTable* scopesymtable, ErrorTracker* errors)
					: exception(exception), body(body), classsymtable(classsymtable), scopesymtable(scopesymtable), errors(errors);

				typeCheck(bool forceArrayIdentifier);

			private:
				Type* exception;
				Node* node;
				ClassSpaceSymbolTable* classsymtable;
				ScopedSymbolTable* scopesymtable;
				ErrorTracker* errors;

		};

	}

}

#endif
