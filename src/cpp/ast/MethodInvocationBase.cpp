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

PureType* wake::ast::MethodInvocationBase::typeCheckMethodInvocation(PureType& subject) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	PureType* boxedtype;

	if(subject.type == TYPE_OPTIONAL) {
		errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Calling method on optional type " + subject.toString() + ". You must first wrap object in an exists { } clause.", node));
		return new PureType(TYPE_MATCHALL);
	}

	if(analyzer->isAutoboxedType(&subject, &boxedtype)) {
		Node* autobox = node->node_data.nodes[0];
		node->node_data.nodes[0] = makeTwoBranchNode(NT_AUTOBOX, autobox, makeNodeFromString(NT_COMPILER_HINT, strdup(boxedtype->typedata._class.classname), node->loc), node->loc);
		subject = *boxedtype;
		delete boxedtype;
	}

	auto_ptr<ReadOnlyPropertySymbolTable> methodtable;

	try {
		methodtable.reset(classestable->findFullyQualified(subject.getFQClassname(), subject.getClassParametersAsVector()));
	} catch (SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, string("Class by name of ") + subject.typedata._class.classname + " returned by another expression has not been imported and cannot be resolved", node));
		return new PureType(TYPE_MATCHALL);
	}

	vector<pair<string, PureTypeArray*> > methodSegmentTypes;
	boost::ptr_vector<PureTypeArray> methodSegmentsLatch;

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
	std::auto_ptr<PureType> lambdatype;

	if(lambdaprop) {
		int i = 0;
		map<string, PureType*> methodAppliedTypes;
		lambdatype.reset(new PureType((*lambdaprop)->decl.typedata));
		address = (*lambdaprop)->address;

		if(!((*lambdaprop)->flags & PROPERTY_PUBLIC) && node->node_data.nodes[0]->node_type != NT_THIS) {
			errors->addError(new SemanticError(PRIVATE_ACCESS, "Method " + address + " is private", node));
		}

		TypeParameterizer parameterizer;

		for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it)
		for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt, ++i)
		if(argExprIt->first) {
			auto_ptr<PureType> argType(argExprIt->first->typeCheck(false));
			bool compatible = true;

			if(analyzer->hasArgParameterization(lambdatype->typedata.lambda.arguments->types[i])) {
				map<string, PureType*> capturedParameterizations;
				compatible = parameterizer.captureArgumentParameterizations(argType.get(), lambdatype->typedata.lambda.arguments->types[i], capturedParameterizations, analyzer);
				if(compatible) {
					vector<PureType*> newParameters;
					vector<PureType*> newParameterizations;
					boost::ptr_vector<PureType> latch;

					for(map<string, PureType*>::iterator it = capturedParameterizations.begin(); it != capturedParameterizations.end(); ++it) {
						newParameterizations.push_back(it->second);
						latch.push_back(new PureType(TYPE_PARAMETERIZED));
						newParameters.push_back(&latch[latch.size() - 1]);
						latch[latch.size() - 1].typedata.parameterized.label = strdup(it->first.c_str());
					}

					PureType* tempLambda = new PureType(*lambdatype.get());
					parameterizer.applyParameterizations(tempLambda, newParameters, newParameterizations);
					lambdatype.reset(tempLambda);
				}
			}

			if(!compatible || (argType->type != TYPE_MATCHALL && !analyzer->isASubtypeOfB(argType.get(), lambdatype->typedata.lambda.arguments->types[i]))) {
				errors->addError(new SemanticError(TYPE_ERROR, "Argument number "
					+ boost::lexical_cast<std::string>(i + 1)
					+ " has invalid type. Expected "
					+ lambdatype->typedata.lambda.arguments->types[i]->toString()
					+ ", actual was "
					+ argType->toString(),
					node
				));
			}
		}

		i = 0;
		for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it)
		for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt, ++i)
		if(argExprIt->second) {
			auto_ptr<PureType> argType;
			if(lambdatype->typedata.lambda.arguments->types[i]->type == TYPE_LAMBDA && analyzer->hasArgParameterization(lambdatype->typedata.lambda.arguments->types[i]->typedata.lambda.arguments)) {
				argType.reset(argExprIt->second->typeCheck(false));
			} else {
				argType.reset(argExprIt->second->typeCheckExpecting(lambdatype->typedata.lambda.arguments->types[i]));
			}

			bool compatible = true;

			if(analyzer->hasArgParameterization(lambdatype->typedata.lambda.arguments->types[i])) {
				map<string, PureType*> capturedParameterizations;
				compatible = parameterizer.captureArgumentParameterizations(argType.get(), lambdatype->typedata.lambda.arguments->types[i], capturedParameterizations, analyzer);
				if(compatible) {
					vector<PureType*> newParameters;
					vector<PureType*> newParameterizations;
					boost::ptr_vector<PureType> latch;

					for(map<string, PureType*>::iterator it = capturedParameterizations.begin(); it != capturedParameterizations.end(); ++it) {
						newParameterizations.push_back(it->second);
						latch.push_back(new PureType(TYPE_PARAMETERIZED));
						newParameters.push_back(&latch[latch.size() - 1]);
						latch[latch.size() - 1].typedata.parameterized.label = strdup(it->first.c_str());
					}

					PureType* tempLambda = new PureType(*lambdatype.get());
					parameterizer.applyParameterizations(tempLambda, newParameters, newParameterizations);
					lambdatype.reset(tempLambda);
				}
			}

			if(!compatible || (argType->type != TYPE_MATCHALL && !analyzer->isASubtypeOfB(argType.get(), lambdatype->typedata.lambda.arguments->types[i]))) {
				errors->addError(new SemanticError(TYPE_ERROR, "Argument number "
					+ boost::lexical_cast<std::string>(i + 1)
					+ " has invalid type. Expected "
					+ lambdatype->typedata.lambda.arguments->types[i]->toString()
					+ ", actual was "
					+ argType->toString(),
					node
				));
			}
		}
	} else {
		for(boost::ptr_vector<MethodSegment>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it) {
			PureTypeArray* args = new PureTypeArray();
			methodSegmentsLatch.push_back(args);
			methodSegmentTypes.push_back(pair<string, PureTypeArray*>(it->name, args));

			for(std::vector<std::pair<wake::ast::ExpressionNode*, wake::ast::ExpectedTypeExpression*> >::iterator argExprIt = it->arguments.begin(); argExprIt != it->arguments.end(); ++argExprIt)
			if(argExprIt->first) {
				addPureTypeToPureTypeArray(argExprIt->first->typeCheck(false), args);
			} else {
				addPureTypeToPureTypeArray(argExprIt->second->typeCheck(false), args);
			}
		}

		string name = methodtable->getSymbolNameOf(&methodSegmentTypes);
		boost::optional<PureType*> optlambdatype(methodtable->find(name));
		if(optlambdatype) {
			lambdatype.reset(new PureType(**optlambdatype));
			address = methodtable->getAddress(name);
			int flags = methodtable->getFlags(name);
			if(!(flags & PROPERTY_PUBLIC) && node->node_data.nodes[0]->node_type != NT_THIS) {
				errors->addError(new SemanticError(PRIVATE_ACCESS, "Method " + address + " is private", node));
			}
		}
	}

	if(lambdatype.get()) {
		addSubNode(node, makeNodeFromString(NT_COMPILER_HINT, strdup(subject.typedata._class.classname), node->loc));
		addSubNode(node, makeNodeFromString(NT_COMPILER_HINT, strdup(address.c_str()), node->loc));

		if(lambdatype->typedata.lambda.returntype == NULL) {
			return new PureType(TYPE_UNUSABLE);
		} else {
			return new PureType(*lambdatype->typedata.lambda.returntype);
		}
	}

	errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Couldn't find property " + methodtable->getSymbolNameOf(&methodSegmentTypes) + " on class " + subject.typedata._class.classname, node));
	return new PureType(TYPE_MATCHALL);
}
