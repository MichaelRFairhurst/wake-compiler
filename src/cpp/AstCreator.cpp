#include "tree.h"
#include <vector>
#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/OtherStatement.h"
#include "ast/OtherExpression.h"
#include "ast/Declaration.h"
#include "ast/Catch.h"
#include "ast/ExpressionErrorCatcher.h"
#include "ast/ExpressionUsabilityCatcher.h"
#include "AstCreator.h"

StatementNode* wake::AstCreator::generateStatementAst(Node* node) {
	switch(node->node_type) {
		case NT_STRINGLIT:
		case NT_NUMBERLIT:
		case NT_BOOLLIT:
		case NT_ALIAS:
		case NT_TYPEDATA:
		case NT_THIS:
		case NT_NOTHING:
		case NT_PARENT:
		case NT_ADD_ASSIGNMENT:
		case NT_ADD:
		case NT_MULT_ASSIGNMENT:
		case NT_DIV_ASSIGNMENT:
		case NT_SUB_ASSIGNMENT:
		case NT_LESSTHAN:
		case NT_LESSTHANEQUAL:
		case NT_GREATERTHAN:
		case NT_GREATERTHANEQUAL:
		case NT_EQUALITY:
		case NT_INEQUALITY:
		case NT_AND:
		case NT_OR:
		case NT_INVERT:
		case NT_TYPESAFE_ARRAY_ACCESS:
		case NT_ARRAY_ACCESS:
		case NT_ARRAY_ACCESS_LVAL:
		case NT_VALUED_ASSIGNMENT:
		case NT_ASSIGNMENT:
		case NT_LAMBDA_INVOCATION:
		case NT_EARLYBAILOUT_METHOD_INVOCATION:
		case NT_METHOD_INVOCATION:
		case NT_CAST:
		case NT_RETRIEVAL:
		case NT_ARRAY_DECLARATION:
		case NT_EARLYBAILOUT_MEMBER_ACCESS:
		case NT_MEMBER_ACCESS:
		case NT_IF_THEN_ELSE:
		case NT_INCREMENT:
		case NT_DECREMENT:
		case NT_CURRIED:
			return new ExpressionStatement(generateExpressionAst(node, false));

		case NT_DECLARATION:
			return new Declaration(node->node_data.nodes[0]->node_data.type, generateExpression(node->node_data.nodes[1], true), classestable, scopesymtable, errors, paramaterizedtypes);

		case NT_CATCH:
			return new Catch(node->node_data.nodes[0]->node_data.type, generateStatement(node->node_data.nodes[1]), classestable, scopesymtable, errors);

		case NT_THROW:
			return new Throw(generateExpression(node->node_data.nodes[0], true), classestable->getAnalyzer(), errors);

		case NT_IF_ELSE:
		case NT_WHILE:
			return new IfElseWhile(generateExpression(node->node_data.nodes[0], true), generateStatement(node->node_data.nodes[1]), node->subnodes == 3 ? generateStatement(node->node_data.nodes[2]) : NULL, classestable->getAnalyzer());

		case NT_FOR:
			return new For(generateStatement(node->node_data.nodes[0]), generateExpression(node->node_data.nodes[1], true), generateStatement(node->node_data.nodes[2]), generateStatement(node->node_data.nodes[3]), scopesymtable, classestable->getAnalyzer());

		case NT_RETURN:
			return new Return(generateExpression(node->node_data.nodes[0], true), returntype, classestable->getAnalyzer());

		case NT_EXISTS:
		case NT_FOREACH:
		case NT_FOREACHIN:
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
			vector<StatementNode*> subnodes = new vector<StatementNode*>();

			for(int i = 0; i < node->subnodes) {
				subnodes.push_back(generateStatementAst(node->node_data.nodes[0]));
			}

			return new OtherStatement(node, subnodes, errors, scopesymtable, objectysmtable, methodanalyzer, thiscontext, returntype, parameterizedtypes);
	}
}

ExpressionNode* wake::AstCreator::generateExpressionAst(Node* node, bool mustBeUsable) {
	vector<ExpressionNode*> subnodes = new vector<ExpressionNode*>();

	for(int i = 0; i < node->subnodes) {
		subnodes.push_back(generateExpressionAst(node->node_data.nodes[0], true));
	}

	if(mustBeUsable) {
		return new ExpressionUsabilityCatcher(new ExpressionErrorCatcher(new OtherExpression(node, subnodes, errors, scopesymtable, objectysmtable, methodanalyzer, thiscontext, returntype, parameterizedtypes)));
	} else {
		return new ExpressionErrorCatcher(new OtherExpression(node, subnodes, errors, scopesymtable, objectysmtable, methodanalyzer, thiscontext, returntype, parameterizedtypes));
	}

}
