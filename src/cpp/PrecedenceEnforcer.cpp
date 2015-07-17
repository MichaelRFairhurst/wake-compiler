
/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PrecedenceEnforcer.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "PrecedenceEnforcer.h"

void PrecedenceEnforcer::enforceAssociative(Node* tree, int index) {
	enforceExact(tree, index, config.getPrec(tree->node_type));
}

void PrecedenceEnforcer::enforceExact(Node* tree, int index, int outer) {
	Node* child = tree->node_data.nodes[index];
	int type = child->node_type;
	if(config.hasPrec(type)) {
		if(config.getPrec(type) > outer) {
			tree->node_data.nodes[index] = makeOneBranchNode(NT_REQUIRED_PARENS, child, child->loc);
		}

		enforce(child);
	}
}

void PrecedenceEnforcer::enforceNonAssociative(Node* tree, int index) {
	enforceExact(tree, index, config.getPrec(tree->node_type) - 1);
}

void PrecedenceEnforcer::enforce(Node* tree) {
	switch(tree->node_type) {
		case NT_IMPORT:
		case NT_IMPORTSET:
		case NT_IMPORTTARGET:
		case NT_IMPORTPATH:
		case NT_ABSTRACT_METHOD:
		case NT_TYPE_ARRAY:
		case NT_CURRIED:
		case NT_CASE:
		case NT_DEFAULTCASE:
		case NT_INCREMENT:
		case NT_DECREMENT:
		case NT_SWITCH:
			return;

		case NT_ANNOTATED_CLASS:
		case NT_ANNOTATED_METHOD:
		case NT_ANNOTATED_TYPE:
		case NT_PROGRAM:
		case NT_RETRIEVALS_STATEMENTS:
		case NT_BLOCK:
		case NT_VALUES:
		case NT_CLASSSET:
		case NT_EXPRESSIONS:
		case NT_INHERITANCESET:
		case NT_PROVISIONS:
		case NT_CTOR:
		case NT_PARENT:
		case NT_INJECTION:
		case NT_CLASS:
		case NT_TRY:
		case NT_THROW:
			{
				int i;
				for(i = 0; i < tree->subnodes; i++)
					enforce(tree->node_data.nodes[i]);
			}
			break;

		case NT_CATCH:
			enforce(tree->node_data.nodes[1]);
			break;

		case NT_PROVISION:
			if(tree->subnodes != 2 && tree->node_data.nodes[1]->node_type == NT_PROVISION_BEHAVIOR) {
				enforce(tree->node_data.nodes[1]->node_data.nodes[1]);
			}
			break;

		case NT_CLASSBODY:
			{
				int i;
				for(i = 0; i < tree->subnodes; i++)
				if(tree->node_data.nodes[i]->node_type == NT_METHOD_DECLARATION || tree->node_data.nodes[i]->node_type == NT_ANNOTATED_METHOD || tree->node_data.nodes[i]->node_type == NT_PROVISIONS || tree->node_data.nodes[i]->node_type == NT_PROPERTY)
					enforce(tree->node_data.nodes[i]);
			}
			break;

		case NT_METHOD_DECLARATION:
			{
				if(tree->subnodes > 0) enforce(tree->node_data.nodes[0]);
				if(tree->subnodes > 1) enforce(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) enforce(tree->node_data.nodes[2]);
				if(tree->subnodes > 3) enforce(tree->node_data.nodes[3]);
			}

			break;

		case NT_LAMBDA_DECLARATION:
			{
				enforce(tree->node_data.nodes[1]);
			}
			break;

		//case NT_METHOD_RETURN_TYPE:
		//case NT_METHOD_NAME:
			//break;

		case NT_METHOD_INVOCATION:
		case NT_EARLYBAILOUT_METHOD_INVOCATION:
			{
				enforce(tree->node_data.nodes[0]);
				int i;
				for(i = 1; i < tree->node_data.nodes[1]->subnodes; i += 2) {
					for(int b = 0; b < tree->node_data.nodes[1]->node_data.nodes[i]->subnodes; b++) {
						enforce(tree->node_data.nodes[1]->node_data.nodes[i]->node_data.nodes[b]);
					}
				}
			}
			break;

		case NT_EARLYBAILOUT_MEMBER_ACCESS:
		case NT_MEMBER_ACCESS:
			{
				enforce(tree->node_data.nodes[0]);
			}
			break;

		case NT_RETRIEVAL:
			{
				enforceExact(tree, 2, config.getPrec(NT_METHOD_INVOCATION));
				for(int i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
					enforce(tree->node_data.nodes[1]->node_data.nodes[i]);
				}
			}
			break;

		case NT_PROPERTY:
			{
				enforce(tree->node_data.nodes[0]->node_data.nodes[1]);
			}
			break;

		case NT_TYPEINFER_DECLARATION:
		case NT_DECLARATION:
		case NT_ASSIGNMENT:
		case NT_VALUED_ASSIGNMENT:
		case NT_ADD_ASSIGNMENT:
		case NT_SUB_ASSIGNMENT:
		case NT_MULT_ASSIGNMENT:
		case NT_DIV_ASSIGNMENT:
			{
				enforce(tree->node_data.nodes[1]);
				break;
			}

		case NT_ARRAY_DECLARATION:
			{
				int i;
				for(i = 0; i < tree->subnodes; i++) {
					enforce(tree->node_data.nodes[i]);
				}

			}
			break;

		case NT_EXISTS:
			enforce(tree->node_data.nodes[0]);
			enforce(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) {
				enforce(tree->node_data.nodes[2]);
			}
			break;

		case NT_IF_ELSE:
			enforce(tree->node_data.nodes[0]);
			enforce(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) {
				enforce(tree->node_data.nodes[2]);
			}
			break;

		case NT_WHILE:
			enforce(tree->node_data.nodes[0]);
			enforce(tree->node_data.nodes[1]);
			break;

		case NT_FOR:
			enforce(tree->node_data.nodes[0]);
			enforce(tree->node_data.nodes[1]);
			enforce(tree->node_data.nodes[2]);
			enforce(tree->node_data.nodes[3]);
			break;

		case NT_FOREACH:
		case NT_FOREACHIN:
		case NT_FOREACHAT:
		case NT_FOREACHINAT:
			{
				int index;
				if(tree->node_type == NT_FOREACHIN || tree->node_type == NT_FOREACHINAT) {
					index = 1;
				} else {
					index = 0;
				}

				enforceExact(tree, index, config.getPrec(NT_ARRAY_ACCESS) < config.getPrec(NT_MEMBER_ACCESS) ? config.getPrec(NT_ARRAY_ACCESS) : config.getPrec(NT_MEMBER_ACCESS)); // code generator will generate (iterationVal).length and also (iterationVal)[i]
			}
			break;

		case NT_LAMBDA_INVOCATION:
			enforceAssociative(tree, 0);
			if(tree->subnodes == 2) {
				for(int i = 0; i < tree->node_data.nodes[1]->subnodes; ++i) {
					enforce(tree->node_data.nodes[1]->node_data.nodes[i]);
				}
			}
			break;

		case NT_IF_THEN_ELSE:
			if(config.triAssocLeft(NT_IF_THEN_ELSE)) {
				enforceAssociative(tree, 0);
			} else {
				enforceNonAssociative(tree, 0);
			}

			if(config.triAssocMiddle(NT_IF_THEN_ELSE)) {
				enforceAssociative(tree, 1);
			} else {
				enforceNonAssociative(tree, 1);
			}

			if(config.triAssocRight(NT_IF_THEN_ELSE)) {
				enforceAssociative(tree, 2);
			} else {
				enforceNonAssociative(tree, 2);
			}
			break;

		case NT_RETURN:
			if(tree->subnodes) enforce(tree->node_data.nodes[0]);
			break;

		// unary ops with child at 1
		case NT_CAST:
			enforceAssociative(tree, 1);
			break;

		// unary ops with child at 0
		case NT_AUTOBOX:
		case NT_INVERT:
		case NT_BITNOT:
			enforceAssociative(tree, 0);
			break;

		case NT_DIVIDE:
		case NT_MOD:
		case NT_MODNATIVE:
		case NT_MODALT:
		case NT_SUBTRACT:
		case NT_BITSHIFTLEFT:
		case NT_BITSHIFTRIGHT:
		case NT_ARRAY_ACCESS_LVAL:
		case NT_TYPESAFE_ARRAY_ACCESS:
		case NT_ARRAY_ACCESS:
		case NT_MULTIPLY:
		case NT_ADD:
		case NT_LESSTHAN:
		case NT_GREATERTHAN:
		case NT_LESSTHANEQUAL:
		case NT_GREATERTHANEQUAL:
		case NT_EQUALITY:
		case NT_INEQUALITY:
		case NT_BITAND:
		case NT_BITXOR:
		case NT_BITOR:
		case NT_AND:
		case NT_OR:
			enforceAssociative(tree, 0);
			if(config.binAssoc(tree->node_type)) {
				//enforceAssociative(tree, 0);
				enforceAssociative(tree, 1);
			} else {
				//enforceNonAssociative(tree, 0);
				enforceNonAssociative(tree, 1);
			}
			break;
	}
}
