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
#include "TypeParameterizer.h"
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
	string address;
	for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it) {
		casing += it->name + "(";
		for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt) {
			if(argExprIt != it->arguments.begin()) {
				casing += ",";
			}
			casing += "#";
		}
		casing += ")";
	}

	boost::optional<ObjectProperty*> lambdaprop = methodtable->findByCasing(casing);
	std::auto_ptr<Type> lambdatype;

	if(lambdaprop) {
		int i = 0;
		map<string, Type*> methodAppliedTypes;
		lambdatype.reset(new Type((*lambdaprop)->type));
		address = (*lambdaprop)->address;

		TypeParameterizer parameterizer;

		for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it)
		for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt, ++i)
		if(argExprIt->first) {
			auto_ptr<Type> argType(argExprIt->first->typeCheck(false));
			bool compatible = true;

			if(analyzer->hasArgParameterization(lambdatype->typedata.lambda.arguments->types[i])) {
				map<string, Type*> capturedParameterizations;
				compatible = parameterizer.captureArgumentParameterizations(argType.get(), lambdatype->typedata.lambda.arguments->types[i], capturedParameterizations, analyzer);
				if(compatible) {
					vector<Type*> newParameters;
					vector<Type*> newParameterizations;
					boost::ptr_vector<Type> latch;

					for(map<string, Type*>::iterator it = capturedParameterizations.begin(); it != capturedParameterizations.end(); ++it) {
						newParameterizations.push_back(it->second);
						latch.push_back(new Type(TYPE_PARAMETERIZED));
						newParameters.push_back(&latch[latch.size() - 1]);
						latch[latch.size() - 1].typedata.parameterized.label = strdup(it->first.c_str());
					}

					Type* tempLambda = new Type(lambdatype.get());
					parameterizer.applyParameterizations(&tempLambda, newParameters, newParameterizations);
					lambdatype.reset(tempLambda);
				}
			}

			if(!compatible || (argType->type != TYPE_MATCHALL && !analyzer->isASubtypeOfB(argType.get(), lambdatype->typedata.lambda.arguments->types[i]))) {
				errors->addError(new SemanticError(TYPE_ERROR, "Argument number "
					+ boost::lexical_cast<std::string>(i + 1)
					+ " has invalid type. Expected "
					+ analyzer->getNameForType(lambdatype->typedata.lambda.arguments->types[i])
					+ ", actual was "
					+ analyzer->getNameForType(argType.get()),
					node
				));
			}
		}

		i = 0;
		for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it)
		for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt, ++i)
		if(argExprIt->second) {
			auto_ptr<Type> argType;
			if(lambdatype->typedata.lambda.arguments->types[i]->type == TYPE_LAMBDA && analyzer->hasArgParameterization(lambdatype->typedata.lambda.arguments->types[i]->typedata.lambda.arguments)) {
				argType.reset(argExprIt->second->typeCheck(false));
			} else {
				argType.reset(argExprIt->second->typeCheckExpecting(lambdatype->typedata.lambda.arguments->types[i]));
			}

			bool compatible = true;

			if(analyzer->hasArgParameterization(lambdatype->typedata.lambda.arguments->types[i])) {
				map<string, Type*> capturedParameterizations;
				compatible = parameterizer.captureArgumentParameterizations(argType.get(), lambdatype->typedata.lambda.arguments->types[i], capturedParameterizations, analyzer);
				if(compatible) {
					vector<Type*> newParameters;
					vector<Type*> newParameterizations;
					boost::ptr_vector<Type> latch;

					for(map<string, Type*>::iterator it = capturedParameterizations.begin(); it != capturedParameterizations.end(); ++it) {
						newParameterizations.push_back(it->second);
						latch.push_back(new Type(TYPE_PARAMETERIZED));
						newParameters.push_back(&latch[latch.size() - 1]);
						latch[latch.size() - 1].typedata.parameterized.label = strdup(it->first.c_str());
					}

					Type* tempLambda = new Type(lambdatype.get());
					parameterizer.applyParameterizations(&tempLambda, newParameters, newParameterizations);
					lambdatype.reset(tempLambda);
				}
			}

			if(!compatible || (argType->type != TYPE_MATCHALL && !analyzer->isASubtypeOfB(argType.get(), lambdatype->typedata.lambda.arguments->types[i]))) {
				errors->addError(new SemanticError(TYPE_ERROR, "Argument number "
					+ boost::lexical_cast<std::string>(i + 1)
					+ " has invalid type. Expected "
					+ analyzer->getNameForType(lambdatype->typedata.lambda.arguments->types[i])
					+ ", actual was "
					+ analyzer->getNameForType(argType.get()),
					node
				));
			}
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

		string name = methodtable->getSymbolNameOf(&methodSegmentTypes);
		boost::optional<Type*> optlambdatype(methodtable->find(name));
		if(optlambdatype) {
			lambdatype.reset(new Type(*optlambdatype));
			address = methodtable->getAddress(name);
		}
	}

	if(lambdatype.get()) {
		AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, strdup(subject.typedata._class.classname), node->loc));
		AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, strdup(address.c_str()), node->loc));

		if(lambdatype->typedata.lambda.returntype == NULL) {
			return new Type(TYPE_UNUSABLE);
		} else {
			return new Type(lambdatype->typedata.lambda.returntype);
		}
	}

	errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Couldn't find property " + methodtable->getSymbolNameOf(&methodSegmentTypes) + " on class " + subject.typedata._class.classname, node));
	return MakeType(TYPE_MATCHALL);
}
