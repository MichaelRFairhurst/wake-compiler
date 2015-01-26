/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MethodInvocationBase.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/MethodInvocationBase.h"
#include "CompilationExceptions.h"
#include "TypeError.h"
#include <boost/lexical_cast.hpp>

Type* wake::ast::MethodInvocationBase::typeCheckMethodInvocation(Type& subject) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	Type* boxedtype;

	if(subject.type == TYPE_OPTIONAL) {
		errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Calling method on optional type " + analyzer->getNameForType(&subject) + ". You must first wrap object in an exists { } clause.", node));
		return new Type(TYPE_MATCHALL);
	}

	if(analyzer->isAutoboxedType(&subject, &boxedtype)) {
		Node* autobox = node->node_data.nodes[0];
		node->node_data.nodes[0] = MakeTwoBranchNode(NT_AUTOBOX, autobox, MakeNodeFromString(NT_COMPILER_HINT, strdup(boxedtype->typedata._class.classname), node->loc), node->loc);
		subject = *boxedtype;
		delete boxedtype;
	}

	auto_ptr<ReadOnlyPropertySymbolTable> methodtable;

	try {
		methodtable.reset(classestable->find(&subject));
	} catch (SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, string("Class by name of ") + subject.typedata._class.classname + " returned by another expression has not been imported and cannot be resolved", node));
		return MakeType(TYPE_MATCHALL);
	}

	vector<pair<string, TypeArray*> > methodSegmentTypes;
	boost::ptr_vector<TypeArray> methodSegmentsLatch;

	string casing;
	string name;
	for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it) {
		casing += it->name + "(";
		for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt) {
			casing += "#";
		}
		casing += ")";
	}

	boost::optional<ObjectProperty*> lambdaprop = methodtable->findByCasing(casing);
	boost::optional<Type*> lambdatype;

	int i = 0;
	if(lambdaprop) {
		lambdatype = (*lambdaprop)->type;
		name = (*lambdaprop)->address;
		for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it, i++)
		for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt)
		if(argExprIt->first) {
			auto_ptr<Type> argType(argExprIt->first->typeCheck(false));
			if(argType->type != TYPE_MATCHALL && !analyzer->isASubtypeOfB(argType.get(), (*lambdatype)->typedata.lambda.arguments->types[i])) {
				errors->addError(new SemanticError(TYPE_ERROR, "Argument number "
					+ boost::lexical_cast<std::string>(i + 1)
					+ " has invalid type. Expected "
					+ analyzer->getNameForType((*lambdatype)->typedata.lambda.arguments->types[i])
					+ ", actual was "
					+ analyzer->getNameForType(argType.get())
				));
			}
		} else {
			argExprIt->second->typeCheckExpecting((*lambdatype)->typedata.lambda.arguments->types[i]);
		}
	} else {
		for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it) {
			TypeArray* args = MakeTypeArray();
			methodSegmentsLatch.push_back(args);
			methodSegmentTypes.push_back(pair<string, TypeArray*>(it->name, args));

			for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt)
			if(argExprIt->first) {
				AddTypeToTypeArray(argExprIt->first->typeCheck(false), args);
			} else {
				AddTypeToTypeArray(argExprIt->second->typeCheck(false), args);
			}
		}

		name = methodtable->getSymbolNameOf(&methodSegmentTypes);
		lambdatype = methodtable->find(name);
	}

	if(lambdatype) {
		AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, strdup(subject.typedata._class.classname), node->loc));
		AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, strdup(methodtable->getAddress(name).c_str()), node->loc));

		if((*lambdatype)->typedata.lambda.returntype == NULL) {
			return new Type(TYPE_UNUSABLE);
		} else {
			return new Type(*(*lambdatype)->typedata.lambda.returntype);
		}
	}

	errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Couldn't find property " + methodtable->getSymbolNameOf(&methodSegmentTypes) + " on class" + subject.typedata._class.classname, node));
	return MakeType(TYPE_MATCHALL);
}
