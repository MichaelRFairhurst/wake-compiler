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
#include "ast/Lambda.h"
#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"
#include "PureType.h"
#include "node.h"
#include "UnifyingType.h"

namespace wake {

	class AstCreator {

		public:
			AstCreator(ErrorTracker* errors, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer, PureType<QUALIFIED>* returntype, PureType<QUALIFIED>* thiscontext, vector<PureType<QUALIFIED>*>& parameterizedtypes)
				: errors(errors), classestable(classestable), scopesymtable(scopesymtable), methodanalyzer(methodanalyzer), returntype(returntype), thiscontext(thiscontext), parameterizedtypes(parameterizedtypes), lambdaReturnType(NULL) {};

			ast::StatementNode* generateStatementAst(Node* node);
			ast::ExpressionNode* generateExpressionAst(Node* node, bool mustBeUsable);

		private:
			ast::Lambda* generateLambda(Node* node);

			MethodSignatureParseTreeTraverser* methodanalyzer;
			ErrorTracker* errors;
			ClassSpaceSymbolTable* classestable;
			ScopeSymbolTable* scopesymtable;
			PureType<QUALIFIED>* returntype;
			PureType<QUALIFIED>* thiscontext;
			vector<PureType<QUALIFIED>*>& parameterizedtypes;
			UnifyingType* lambdaReturnType;

	};

}

#endif
