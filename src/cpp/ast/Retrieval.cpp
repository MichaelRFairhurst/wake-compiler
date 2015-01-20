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

Type* wake::ast::Retrieval::typeCheck(bool forceArrayIdentifier) {
	// TypeParameterizer parameterizer;
	// parameterizer.writeInParameterizations(&node->node_data.nodes[0]->node_data.type, parameterizedtypes);
	// @TODO what here can be parameterized? I'm pretty sure only the type parameters, but even then, not totally sure
	try {
		Type provider = *auto_ptr<Type>(providerExp->typeCheck(false));
		classestable->assertTypeIsValid(retrievalType);
		vector<Type*> arguments;
		boost::ptr_vector<Type> arguments_latch;

		for(boost::ptr_vector<wake::ast::ExpressionNode>::iterator it = argumentExprs.begin(); it != argumentExprs.end(); ++it) {
			arguments_latch.push_back((*it).typeCheck(false));
			arguments.push_back(&arguments_latch[arguments_latch.size() - 1]);
		}

		string name = analyzer->getProvisionSymbol(retrievalType, arguments);

		try {
			auto_ptr<ReadOnlyPropertySymbolTable> table(classestable->find(&provider));
			boost::optional<Type*> provision = table->find(name);
			if(!provision) {
				errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Provision " + name + " does not exist on class " + analyzer->getNameForType(&provider), node));
			}
		} catch(SymbolNotFoundException *e) {
			delete e;
		}
		AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, strdup(name.c_str()), node->loc));
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}

	return new Type(*retrievalType);
}
