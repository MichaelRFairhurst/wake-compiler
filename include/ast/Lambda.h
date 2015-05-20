/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Lambda.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_LAMBDA
#define HEADER_AST_LAMBDA

#include "ast/StatementNode.h"
#include "ast/ExpectedTypeExpression.h"
#include "UnifyingType.h"
#include "ErrorTracker.h"
#include "TypeAnalyzer.h"
#include "ScopeSymbolTable.h"
#include <memory>

namespace wake {

	namespace ast {

		class Lambda : public ExpectedTypeExpression {
			public:
				Lambda(std::vector<std::pair<boost::optional<std::string>, boost::optional<VarDecl<QUALIFIED> > > > arguments, StatementNode* body, UnifyingType* returntype, ScopeSymbolTable* scopesymtable, ErrorTracker* errors, Node* node, TypeAnalyzer* analyzer)
				: analyzer(analyzer), arguments(arguments), body(body), scopesymtable(scopesymtable), errors(errors), returntype(returntype), node(node) {};

				PureType<QUALIFIED>* typeCheck(bool forceArrayIdentifier);
				PureType<QUALIFIED>* typeCheckExpecting(PureType<QUALIFIED>* hint);
				PureType<QUALIFIED>* typeCheckCommon(PureType<QUALIFIED>* hint);

			private:
				std::auto_ptr<UnifyingType> returntype;
				std::vector<std::pair<boost::optional<std::string>, boost::optional<VarDecl<QUALIFIED> > > > arguments;
				std::auto_ptr<StatementNode> body;
				ScopeSymbolTable* scopesymtable;
				ErrorTracker* errors;
				Node* node;
				TypeAnalyzer* analyzer;

		};

	}

}

#endif
