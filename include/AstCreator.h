/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * AstCreator.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_CREATOR
#define HEADER_AST_CREATOR

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"
#include "type.h"
#include "node.h"

namespace wake {

	class AstCreator {

		public:
			AstCreator(ErrorTracker* errors, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer, Type* returntype, Type* thiscontext, vector<Type*>& parameterizedtypes)
				: errors(errors), classestable(classestable), scopesymtable(scopesymtable), methodanalyzer(methodanalyzer), returntype(returntype), thiscontext(thiscontext), parameterizedtypes(parameterizedtypes) {};

			ast::StatementNode* generateStatementAst(Node* node);
			ast::ExpressionNode* generateExpressionAst(Node* node, bool mustBeUsable);

		private:
			MethodSignatureParseTreeTraverser* methodanalyzer;
			ErrorTracker* errors;
			ClassSpaceSymbolTable* classestable;
			ScopeSymbolTable* scopesymtable;
			Type* returntype;
			Type* thiscontext;
			vector<Type*>& parameterizedtypes;

	};

}

#endif
