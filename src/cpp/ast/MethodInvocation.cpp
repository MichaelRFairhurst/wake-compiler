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

Type* wake::ast::MethodInvocation::typeCheck(bool forceArrayIdentifier) {
	Node* methodname = node->node_data.nodes[1];

	if(node->node_data.nodes[0]->node_type == NT_THIS && methodname->subnodes == 2) {
		string name = methodname->node_data.nodes[0]->node_data.string;
		boost::optional<Type*> variable = scopesymtable->find(name);

		if(variable) {
			methodname->node_type = NT_LAMBDA_INVOCATION;
			methodname->node_data.nodes[0]->node_type = NT_ALIAS;
			return MakeType(TYPE_MATCHALL);//children[1].typeCheck(forceArrayIdentifier); // ??? is this a bug introduced in splitting up the AST ???
		}
	}

	Type subject = *auto_ptr<Type>(subjectExpr->typeCheck(false));

	if(subject.type == TYPE_MATCHALL) {
		return new Type(subject);
	}

	return typeCheckMethodInvocation(subject);
}
