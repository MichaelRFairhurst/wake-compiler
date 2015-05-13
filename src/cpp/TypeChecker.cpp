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

using namespace wake;

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
	if(returntype != NULL && !ast->exhaustiveReturns())
		errors->addError(new SemanticError(INEXHAUSTIVE_RETURNS, "", tree));
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

void TypeChecker::setReturnType(PureType<QUALIFIED>* returntype) {
	this->returntype = returntype;
}

void TypeChecker::setThisContext(PureType<QUALIFIED>* context) {
	thiscontext = context;
}

void TypeChecker::setParameterizedTypes(const vector<PureType<QUALIFIED>*>& types) {
	parameterizedtypes = types;
}
