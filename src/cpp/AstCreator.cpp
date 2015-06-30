/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * AstCreator.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "AstCreator.h"
#include "ast/OtherStatement.h"
#include "ast/OtherExpression.h"
#include "ast/Declaration.h"
#include "ast/TypeInferDeclaration.h"
#include "ast/ExpressionStatement.h"
#include "ast/EmptyStatement.h"
#include "ast/Catch.h"
#include "ast/Return.h"
#include "ast/Throw.h"
#include "ast/ExpressionErrorCatcher.h"
#include "ast/ExpressionUsabilityCatcher.h"
#include "ast/For.h"
#include "ast/IfElseWhile.h"
#include "ast/Exists.h"
#include "ast/ForeachInAliased.h"
#include "ast/ForeachInExplicitType.h"
#include "ast/ForeachAt.h"
#include "ast/Retrieval.h"
#include "ast/Invocation.h"
#include "ast/MethodInvocation.h"
#include "ast/EarlyBailoutMethodInvocation.h"
#include "ast/StatementErrorCatcher.h"
#include "ast/Lambda.h"
#include "ast/LambdaReturn.h"
#include "ast/UnsafeCast.h"
#include <vector>

wake::ast::StatementNode* wake::AstCreator::generateStatementAst(Node* node) {
	wake::ast::StatementNode* created;

	switch(node->node_type) {
		case NT_DECLARATION:
			created = new wake::ast::Declaration(
				classestable->setModulesOnType(node->node_data.nodes[0]->node_data.var_decl),
				generateExpressionAst(node->node_data.nodes[1], true),
				node, classestable, scopesymtable, errors, parameterizedtypes);
			break;

		case NT_TYPEINFER_DECLARATION:
			created = new wake::ast::TypeInferDeclaration(
				node->node_data.nodes[0]->node_data.string,
				generateExpressionAst(node->node_data.nodes[1], true),
				node, scopesymtable, errors);
			break;

		case NT_CATCH:
			created = new wake::ast::Catch(
				classestable->setModulesOnType(node->node_data.nodes[0]->node_data.var_decl),
				generateStatementAst(node->node_data.nodes[1]),
				node, classestable, scopesymtable, errors);
			break;

		case NT_THROW:
			created = new wake::ast::Throw(generateExpressionAst(node->node_data.nodes[0], true), classestable->getAnalyzer(), errors);
			break;

		case NT_IF_ELSE:
		case NT_WHILE:
			created = new wake::ast::IfElseWhile(
				generateExpressionAst(node->node_data.nodes[0], true),
				generateStatementAst(node->node_data.nodes[1]),
				node->subnodes == 3 ? generateStatementAst(node->node_data.nodes[2]) : NULL,
				classestable->getAnalyzer());
			break;

		case NT_FOR:
			created = new wake::ast::For(
				generateStatementAst(node->node_data.nodes[0]),
				generateExpressionAst(node->node_data.nodes[1], true),
				generateStatementAst(node->node_data.nodes[2]),
				generateStatementAst(node->node_data.nodes[3]),
				scopesymtable, classestable->getAnalyzer());
			break;

		case NT_RETURN:
			if(node->subnodes) {
				if(lambdaReturnType) {
					created = new wake::ast::LambdaReturn(generateExpressionAst(node->node_data.nodes[0], true), lambdaReturnType);
				} else {
					created = new wake::ast::Return(generateExpressionAst(node->node_data.nodes[0], true), returntype, classestable->getAnalyzer());
				}
			} else {
				if(lambdaReturnType) {
					created = new wake::ast::LambdaReturn(NULL, lambdaReturnType);
				} else {
					created = new wake::ast::Return(NULL, returntype, classestable->getAnalyzer());
				}
			}
			break;

		case NT_EXISTS:
			created = new wake::ast::Exists(
				generateExpressionAst(node->node_data.nodes[0], true),
				generateStatementAst(node->node_data.nodes[1]),
				node->subnodes == 3 ? generateStatementAst(node->node_data.nodes[2]) : NULL,
				node, scopesymtable, errors);
			break;

		case NT_FOREACH:
			created = new wake::ast::Foreach(
				generateExpressionAst(node->node_data.nodes[0], true),
				generateStatementAst(node->node_data.nodes[1]),
				node, scopesymtable, errors);
			break;

		case NT_FOREACHIN:
			if(node->node_data.nodes[0]->node_data.var_ref->alias != NULL) {
				created = new wake::ast::ForeachInAliased(
					generateExpressionAst(node->node_data.nodes[1], true),
					generateStatementAst(node->node_data.nodes[2]),
					node->node_data.nodes[0]->node_data.var_ref->alias,
					node, scopesymtable, errors);
				break;
			} else {
				created = new wake::ast::ForeachInExplicitType(
					generateExpressionAst(node->node_data.nodes[1], true),
					generateStatementAst(node->node_data.nodes[2]),
					node->node_data.nodes[0]->node_data.var_ref->_class,
					node, scopesymtable, errors, classestable->getAnalyzer());
				break;
			}

		case NT_FOREACHAT:
			{
				wake::ast::Foreach* foreach = new wake::ast::Foreach(
					generateExpressionAst(node->node_data.nodes[0], true),
					generateStatementAst(node->node_data.nodes[2]),
					node, scopesymtable, errors);

				created = new wake::ast::ForeachAt(
					foreach,
					*node->node_data.nodes[1]->node_data.var_ref,
					node, scopesymtable, errors);
			}

			break;

		case NT_FOREACHINAT:
			{
				wake::ast::Foreach* foreach;
				if(node->node_data.nodes[0]->node_data.var_ref->alias != NULL) {
					foreach = new wake::ast::ForeachInAliased(
						generateExpressionAst(node->node_data.nodes[1], true),
						generateStatementAst(node->node_data.nodes[3]),
						node->node_data.nodes[0]->node_data.var_ref->alias,
						node, scopesymtable, errors);
				} else {
					foreach = new wake::ast::ForeachInExplicitType(
						generateExpressionAst(node->node_data.nodes[1], true),
						generateStatementAst(node->node_data.nodes[3]),
						node->node_data.nodes[0]->node_data.var_ref->_class,
						node, scopesymtable, errors, classestable->getAnalyzer());
				}

				created = new wake::ast::ForeachAt(
					foreach,
					*node->node_data.nodes[2]->node_data.var_ref,
					node, scopesymtable, errors);
			}

			break;


		case NT_SWITCH:
		case NT_VALUES:
		case NT_DEFAULTCASE:
		case NT_CASE:
		case NT_BLOCK:
		case NT_TRY:
		case NT_EXPRESSIONS:
		case NT_RETRIEVALS_STATEMENTS:
		case NT_BREAK:
		case NT_CONTINUE:
		case NT_ABSTRACT_METHOD:
			{
				std::vector<wake::ast::StatementNode*> subnodes;

				for(int i = 0; i < node->subnodes; i++) {
					subnodes.push_back(generateStatementAst(node->node_data.nodes[i]));
				}

				created = new wake::ast::OtherStatement(node, subnodes, scopesymtable);
				break;
			}

		default:
			return new wake::ast::ExpressionStatement(generateExpressionAst(node, false));

	}

	return new wake::ast::StatementErrorCatcher(created, node, errors);
}

wake::ast::ExpressionNode* wake::AstCreator::generateExpressionAst(Node* node, bool mustBeUsable) {
	wake::ast::ExpressionNode* created;
	if(node->node_type == NT_RETRIEVAL) {
		std::vector<wake::ast::ExpressionNode*> arguments;
		if(node->node_data.nodes[1]->node_type != NT_EMPTY) {
			for(int i = 0; i < node->node_data.nodes[1]->subnodes; i++) {
				arguments.push_back(generateExpressionAst(node->node_data.nodes[1]->node_data.nodes[i], true));
			}
		}
		wake::ast::ExpressionNode* provider = generateExpressionAst(node->node_data.nodes[2], true);
		created = new wake::ast::Retrieval(
			provider,
			classestable->setModulesOnType(node->node_data.nodes[0]->node_data.specializable_pure_type),
			arguments, node, classestable, classestable->getAnalyzer(), errors);
	} else if(node->node_type == NT_UNSAFE_CAST) {
		created = new wake::ast::UnsafeCast(
			generateExpressionAst(node->node_data.nodes[1], true),
			classestable->setModulesOnType(node->node_data.nodes[0]->node_data.pure_type),
			node, classestable, errors);
	} else if(node->node_type == NT_LAMBDA_INVOCATION) {
		std::vector<wake::ast::ExpressionNode*> arguments;
		if(node->subnodes == 2) {
			int i;
			for(i = 0; i < node->node_data.nodes[1]->subnodes; i++) {
				arguments.push_back(generateExpressionAst(node->node_data.nodes[1]->node_data.nodes[i], true));
			}
		}
		wake::ast::ExpressionNode* lambda = generateExpressionAst(node->node_data.nodes[0], true);
		created = new wake::ast::Invocation(lambda, arguments, node, classestable->getAnalyzer(), errors);
	} else if(node->node_type == NT_METHOD_INVOCATION || node->node_type == NT_EARLYBAILOUT_METHOD_INVOCATION) {

		Node* methodname = node->node_data.nodes[1];
		vector<wake::ast::MethodSegment*> methodSegments;
		int i = 0;
		while(i < methodname->subnodes) {
			string name = methodname->node_data.nodes[i]->node_data.string;
			std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> > argExprs;
			i++;

			if(methodname->subnodes > i)
			for(int a = 0; a < methodname->node_data.nodes[i]->subnodes; a++)
			if(methodname->node_data.nodes[i]->node_data.nodes[a]->node_type == NT_LAMBDA_DECLARATION) {
				argExprs.push_back(std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*>(NULL, generateLambda(methodname->node_data.nodes[i]->node_data.nodes[a])));
			} else {
				argExprs.push_back(std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*>(generateExpressionAst(methodname->node_data.nodes[i]->node_data.nodes[a], true), NULL));
			}

			methodSegments.push_back(new wake::ast::MethodSegment(name, argExprs));
			i++;
		}

		if(node->node_type == NT_METHOD_INVOCATION) {
			created = new wake::ast::MethodInvocation(generateExpressionAst(node->node_data.nodes[0], true), methodSegments, node, this, scopesymtable, classestable, errors);
		} else {
			created = new wake::ast::EarlyBailoutMethodInvocation(generateExpressionAst(node->node_data.nodes[0], true), methodSegments, node, classestable, errors);
		}

	} else if(node->node_type == NT_LAMBDA_DECLARATION) {
		created = generateLambda(node);
	} else {

		std::vector<wake::ast::ExpressionNode*> subnodes;

		for(int i = 0; i < node->subnodes; i++) {
			subnodes.push_back(generateExpressionAst(node->node_data.nodes[i], true));
		}

		created = new wake::ast::OtherExpression(node, subnodes, errors, classestable, scopesymtable, methodanalyzer, thiscontext, returntype, parameterizedtypes);
	}

	if(mustBeUsable) {
		return new wake::ast::ExpressionUsabilityCatcher(new wake::ast::ExpressionErrorCatcher(created, node, errors), errors, node);
	} else {
		return new wake::ast::ExpressionErrorCatcher(created, node, errors);
	}

}

wake::ast::Lambda* wake::AstCreator::generateLambda(Node* node) {
	std::vector<std::pair<boost::optional<std::string>, boost::optional<VarDecl<QUALIFIED> > > > arguments;

	for(int i = 0; i < node->node_data.nodes[0]->subnodes; i++)
	if(node->node_data.nodes[0]->node_data.nodes[i]->node_type == NT_ALIAS) {
		boost::optional<std::string> alias(node->node_data.nodes[0]->node_data.nodes[i]->node_data.string);
		boost::optional<VarDecl<QUALIFIED> > notype;

		arguments.push_back(std::pair<boost::optional<std::string>, boost::optional<VarDecl<QUALIFIED> > >(alias, notype));
	} else {
		boost::optional<std::string> noalias;
		boost::optional<VarDecl<QUALIFIED> > type(VarDecl<QUALIFIED>(*classestable->setModulesOnType(node->node_data.nodes[0]->node_data.nodes[i]->node_data.var_decl)));

		arguments.push_back(std::pair<boost::optional<std::string>, boost::optional<VarDecl<QUALIFIED> > >(noalias, type));
	}

	UnifyingType* lastLambdaReturn = lambdaReturnType;
	lambdaReturnType = new UnifyingType(classestable->getAnalyzer());
	wake::ast::Lambda* toReturn = new wake::ast::Lambda(arguments, generateStatementAst(node->node_data.nodes[1]), lambdaReturnType, scopesymtable, errors, node, classestable->getAnalyzer());
	lambdaReturnType = lastLambdaReturn;

	return toReturn;
}
