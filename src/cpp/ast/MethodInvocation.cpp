/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MemberInvocation.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/MethodInvocation.h"
#include "ast/Invocation.h"
#include "AstCreator.h"

PureType* wake::ast::MethodInvocation::typeCheck(bool forceArrayIdentifier) {
	Node* methodname = node->node_data.nodes[1];

	if(node->node_data.nodes[0]->node_type == NT_THIS && methodname->subnodes < 3) {
		string name = methodname->node_data.nodes[0]->node_data.string;
		boost::optional<PureType*> variable = scopesymtable->find(name);

		if(variable) {
			node->node_type = NT_LAMBDA_INVOCATION; // for codegen step
			node->node_data.nodes[0] = methodname->node_data.nodes[0];
			if(methodname->subnodes == 2) {
				node->node_data.nodes[1] = methodname->node_data.nodes[1];
			} else {
				node->subnodes = 1;
			}
			methodname->subnodes = 0;
			freeNode(methodname);

			node->node_data.nodes[0]->node_type = NT_ALIAS; // for codegen step
			auto_ptr<ExpressionNode> lambda(astCreator->generateExpressionAst(node, false)); // Don't ensure its a usable type -- this node will have such a wrapper already if it matters
			return lambda->typeCheck(false);
		}
	}

	PureType subject = *auto_ptr<PureType>(subjectExpr->typeCheck(false));

	if(subject.type == TYPE_MATCHALL) {
		return new PureType(subject);
	}

	return typeCheckMethodInvocation(subject);
}
