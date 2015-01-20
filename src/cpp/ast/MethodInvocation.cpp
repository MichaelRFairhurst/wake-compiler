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

Type* wake::ast::MethodInvocation::typeCheck(bool forceArrayIdentifier) {
	Node* methodname = node->node_data.nodes[1];

	if(node->node_data.nodes[0]->node_type == NT_THIS && methodname->subnodes == 2) {
		string name = methodname->node_data.nodes[0]->node_data.string;
		boost::optional<Type*> variable = scopesymtable->find(name);

		if(variable) {
			methodname->node_type = NT_LAMBDA_INVOCATION; // for codegen step
			methodname->node_data.nodes[0]->node_type = NT_ALIAS; // for codegen step
			auto_ptr<ExpressionNode> lambda(astCreator->generateExpressionAst(methodname, false)); // Don't ensure its a usable type -- this node will have such a wrapper already if it matters
			return lambda->typeCheck(false);
		}
	}

	Type subject = *auto_ptr<Type>(subjectExpr->typeCheck(false));

	if(subject.type == TYPE_MATCHALL) {
		return new Type(subject);
	}

	return typeCheckMethodInvocation(subject);
}
