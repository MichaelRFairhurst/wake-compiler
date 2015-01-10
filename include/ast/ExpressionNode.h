/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionNode.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_EXPRESSION_NODE
#define HEADER_EXPRESSION_NODE

#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"
#include "Type.h"
#include "node.h"

namespace wake {

	namespace ast {

		class ExpressionNode {

			public:
				ExpressionNode(ErrorTracker* errors, ClassSpaceSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer)
					: errors(errors), objectsymtable(objectsymtable), scopesymtable(scopesymtable), methodanalyzer(methodanalyzer);

				virtual Type* typeCheck(bool forceArrayIdentifier);

			private:
				MethodSignatureParseTreeTraverser* methodanalyzer;
				ErrorTracker* errors;
				ClassSpaceSymbolTable* classestable;
				ScopeSymbolTable* scopesymtable;
				Type* returntype;
		};

	}

}

#endif
