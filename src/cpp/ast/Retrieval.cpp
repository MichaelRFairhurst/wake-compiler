/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Retrieval.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Retrieval.h"
#include "CompilationExceptions.h"
#include <boost/ptr_container/ptr_vector.hpp>

using namespace wake;

PureType<QUALIFIED>* wake::ast::Retrieval::typeCheck(bool forceArrayIdentifier) {
	// TypeParameterizer parameterizer;
	// parameterizer.writeInParameterizations(&node->node_data.nodes[0]->node_data.type, parameterizedtypes);
	// @TODO what here can be parameterized? I'm pretty sure only the type parameters, but even then, not totally sure
	try {
		PureType<QUALIFIED> provider = *auto_ptr<PureType<QUALIFIED> >(providerExp->typeCheck(false));
		classestable->assertTypeIsValid(&retrievalType->typedata);

		if(provider.type == TYPE_MATCHALL) {
			return new PureType<QUALIFIED>(retrievalType->typedata);
		}

		vector<PureType<QUALIFIED>*> arguments;
		boost::ptr_vector<PureType<QUALIFIED> > arguments_latch;

		for(boost::ptr_vector<wake::ast::ExpressionNode>::iterator it = argumentExprs.begin(); it != argumentExprs.end(); ++it) {
			arguments_latch.push_back((*it).typeCheck(false));
			arguments.push_back(&arguments_latch[arguments_latch.size() - 1]);
		}

		string name = retrievalType->toProvisionSymbol(arguments);

		try {
			if(provider.type != TYPE_CLASS) {
				errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Provision " + name + " cannot be called on non class type " + provider.toString(), node));
			}
			auto_ptr<ReadOnlyPropertySymbolTable> table(classestable->findFullyQualified(provider.getFQClassname()));
			boost::optional<PureType<QUALIFIED>*> provision = table->find(name);
			if(!provision) {
				errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Provision " + name + " does not exist on class " + provider.toString(), node));
			}
		} catch(SymbolNotFoundException *e) {
			delete e;
		}
		addSubNode(node, makeNodeFromString(NT_COMPILER_HINT, strdup(name.c_str()), node->loc));
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}

	return new PureType<QUALIFIED>(retrievalType->typedata);
}
