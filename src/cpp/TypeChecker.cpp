/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeChecker.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By: Coleridge Williamson
 *
 **************************************************/

#include "TypeChecker.h"
#include "CompilationExceptions.h"
#include "TypeParameterizer.h"
#include "AstCreator.h"
#include "ast/StatementNode.h"

#include <memory>
#include <boost/ptr_container/ptr_vector.hpp>

TypeChecker::TypeChecker(ErrorTracker* errors, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer) {
	this->errors = errors;
	this->classestable = classestable;
	this->scopesymtable = scopesymtable;
	this->methodanalyzer = methodanalyzer;
	thiscontext = NULL;
}

void TypeChecker::check(Node* tree) {
	wake::AstCreator astCreator(errors, classestable, scopesymtable, methodanalyzer, returntype, thiscontext, parameterizedtypes);
	auto_ptr<wake::ast::StatementNode> ast(astCreator.generateStatementAst(tree));
	ast->typeCheck();

	flowAnalysis(tree, false, false, false);
	if(returntype != NULL && !exhaustiveReturns(tree))
		errors->addError(new SemanticError(INEXHAUSTIVE_RETURNS, "", tree));
}

bool TypeChecker::exhaustiveReturns(Node* tree) {
	switch(tree->node_type) {
		case NT_SWITCH:
			//not yet supported
			return false;

		case NT_FOR:
		case NT_WHILE:
			// Loops are always conditional and therefore never exhaustive
			return false;

		case NT_EXISTS:
		case NT_IF_ELSE:
			// If no else, then not exhaustive
			if(tree->subnodes == 2) return false;
			// Returns are exhaustive if both the if and else statements are exhaustive
			return exhaustiveReturns(tree->node_data.nodes[1]) && exhaustiveReturns(tree->node_data.nodes[2]);

		case NT_TRY:
			// If no breok, then not exhaustive
			if(tree->subnodes == 1) return false;
			// Returns are exhaustive if both the try and catch statements are exhaustive
			return exhaustiveReturns(tree->node_data.nodes[0]) && exhaustiveReturns(tree->node_data.nodes[1]);

		case NT_RETURN:
		case NT_THROW:
			return true;

		case NT_CATCH:
		case NT_BLOCK:
		case NT_RETRIEVALS_STATEMENTS:
			{
				int i;
				// Statement lists which contain a return are exhaustive
				for(i = 0; i < tree->subnodes; i++)
					if(exhaustiveReturns(tree->node_data.nodes[i])) return true;
				return false;
			}

		case NT_ABSTRACT_METHOD:
			return true;

		default:
			return false;
	}
}

void TypeChecker::flowAnalysis(Node* tree, bool breakable, bool caseable, bool continuable) {
	int i;
	switch(tree->node_type) {
		// These can contain switches, loops, switches, and breaks
		case NT_SWITCH:
			for(i = 0; i < tree->subnodes; i++)
				flowAnalysis(tree->node_data.nodes[i], true, true, continuable);
			break;
		case NT_FOR:
		case NT_WHILE:
			for(i = 0; i < tree->subnodes; i++)
				flowAnalysis(tree->node_data.nodes[i], true, caseable, true);
			break;
		case NT_BLOCK:
		case NT_IF_ELSE:
		case NT_RETRIEVALS_STATEMENTS:
			for(i = 0; i < tree->subnodes; i++)
				flowAnalysis(tree->node_data.nodes[i], breakable, caseable, continuable);
			break;

		// These can't! don't waste time recursing on them or anything else
		case NT_CASE:
		case NT_DEFAULTCASE:
			if(!caseable)
				errors->addError(new SemanticError(ILLEGAL_CASE, "", tree));
			break;
		case NT_BREAK:
			if(!breakable)
				errors->addError(new SemanticError(ILLEGAL_BREAK, "", tree));
			break;
		case NT_CONTINUE:
			if(!continuable)
				errors->addError(new SemanticError(ILLEGAL_CONTINUE, "", tree));
			break;
	}
}

void TypeChecker::setReturnType(Type* returntype) {
	this->returntype = returntype;
}

void TypeChecker::setThisContext(Type* context) {
	thiscontext = context;
}

void TypeChecker::setParameterizedTypes(const vector<Type*>& types) {
	parameterizedtypes = types;
}
