/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Foreach.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Foreach.h"
#include "TypeError.h"

void wake::ast::Foreach::typeCheck() {
	auto_ptr<PureType> list(iterable->typeCheck(false));

	if(list->type == TYPE_OPTIONAL) {
		errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Iterating over optional type. You must first wrap object in an exists { } clause.", node));
		return;
	}

	// no sense checking the contents of the loop if we're missing a variable
	if(list->type == TYPE_MATCHALL) return;

	if(list->type != TYPE_LIST) {
		errors->addError(new SemanticError(TYPE_ERROR, "Calling foreach over something that is not a list", node));
	} else {
		PureType lowered(*list->typedata.list.contained);

		VarDecl actualLowered = getIterationVarDecl(&lowered);

		if(node->node_data.nodes[0]->node_type == NT_VAR_REF && node->node_data.nodes[0]->node_data.var_ref->alias == NULL) {
			actualLowered.shadow = node->node_data.nodes[0]->node_data.var_ref->_class->shadow;
		}

		scopesymtable->pushScope();
		scopesymtable->add(&actualLowered);
		body->typeCheck();
		scopesymtable->popScope();

		addSubNode(node, makeNodeFromString(NT_COMPILER_HINT, strdup(actualLowered.createVarRef().toString().c_str()), node->loc));
	}
}

VarDecl wake::ast::Foreach::getIterationVarDecl(PureType* iterableType) {
	VarDecl decl;
	decl.typedata = *iterableType;
	return decl;
}

bool wake::ast::Foreach::exhaustiveReturns() {
	return false; // might be 0 items
}

