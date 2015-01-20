/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * OtherStatement.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "type.h"
#include "TypeAnalyzer.h"
#include "TypeParameterizer.h"
#include "ClassSpaceSymbolTable.h"
#include "ast/StatementNode.h"
#include "ast/OtherStatement.h"

void wake::ast::OtherStatement::typeCheck() {
	switch(tree->node_type) {
		// Ignoring these for now
		case NT_SWITCH:
		case NT_CURRIED:
		case NT_INCREMENT:
		case NT_DECREMENT:
		// These require a common-ancestor function
		case NT_TYPE_ARRAY:
		case NT_VALUES:
			throw string("Not supported yet");

		// These have tests so can't throw, but won't compile anyway as long as switch throws
		case NT_DEFAULTCASE:
		case NT_CASE:

		// these don't have types
		case NT_BLOCK:
		case NT_TRY:
			scopesymtable->pushScope();
			// FALL THROUGH!
		case NT_RETRIEVALS_STATEMENTS:
		case NT_EXPRESSIONS:
		case NT_BREAK:
		case NT_CONTINUE:
			try {
				for(boost::ptr_vector<StatementNode>::iterator it = children.begin(); it != children.end(); ++it) {
					it->typeCheck();
				}
				if(tree->node_type == NT_BLOCK || tree->node_type == NT_TRY) scopesymtable->popScope();
			} catch(SemanticError *e) {
				if(tree->node_type == NT_BLOCK || tree->node_type == NT_TRY) scopesymtable->popScope();
				throw e;
			}
	}
}
