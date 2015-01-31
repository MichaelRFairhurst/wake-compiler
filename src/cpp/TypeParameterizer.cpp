/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeParameterizer.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "TypeParameterizer.h"

#include <string>
#include <string.h>
#include <memory>

using namespace std;

void TypeParameterizer::rewriteClasstypesToParameterizedtypeByLabel(Type** typeaddr, const std::vector<Type*>& parameters) {
	Type* type = *typeaddr;

	if(type->type == TYPE_CLASS) {
		if(type->typedata._class.parameters != NULL) {
			rewriteClasstypesToParameterizedtypeByLabel(type->typedata._class.parameters, parameters);
		} else {
			for(vector<Type*>::const_iterator it = parameters.begin(); it != parameters.end(); ++it) {
				if(type->typedata._class.classname == string((*it)->typedata.parameterized.label)) {
					*typeaddr = new Type(**it);
					(*typeaddr)->typedata.parameterized.shadow = type->typedata._class.shadow;
					(*typeaddr)->alias = type->alias ? strdup(type->alias) : NULL;
					delete type;
					break; // We've found it, move along
				}
			}
		}
	} else if(type->type == TYPE_LAMBDA) {
		if(type->typedata.lambda.returntype != NULL)
			rewriteClasstypesToParameterizedtypeByLabel(&type->typedata.lambda.returntype, parameters);
		if(type->typedata.lambda.arguments != NULL)
			rewriteClasstypesToParameterizedtypeByLabel(type->typedata.lambda.arguments, parameters);
	} else if(type->type == TYPE_LIST) {
		rewriteClasstypesToParameterizedtypeByLabel(&type->typedata.list.contained, parameters);
	} else if(type->type == TYPE_OPTIONAL) {
		rewriteClasstypesToParameterizedtypeByLabel(&type->typedata.optional.contained, parameters);
	}
}

void TypeParameterizer::rewriteClasstypesToParameterizedtypeByLabel(TypeArray* types, const std::vector<Type*>& parameters) {
	int i;
	for(i = 0; i < types->typecount; i++) {
		rewriteClasstypesToParameterizedtypeByLabel(&types->types[i], parameters);
	}
}

void TypeParameterizer::applyParameterizations(Type** typeaddr, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations) {
	Type* type = *typeaddr;

	if(type->type == TYPE_CLASS) {
		if(type->typedata._class.parameters != NULL) {
			applyParameterizations(type->typedata._class.parameters, parameters, parameterizations);
		}
	} else if(type->type == TYPE_PARAMETERIZED || type->type == TYPE_PARAMETERIZED_ARG) {
		for(int i = 0; i < parameters.size(); i++) {
			if(type->typedata.parameterized.label == string(parameters.at(i)->typedata.parameterized.label)) {
				*typeaddr = new Type(*parameterizations.at(i));
				(*typeaddr)->alias = type->alias ? strdup(type->alias) : NULL;
				// Shadows don't matter because applied parameterizations don't have to do with scopes
				delete type;
				break; // We've found it, move along
			}
		}
	} else if(type->type == TYPE_LAMBDA) {
		if(type->typedata.lambda.returntype != NULL)
			applyParameterizations(&type->typedata.lambda.returntype, parameters, parameterizations);
		if(type->typedata.lambda.arguments != NULL)
			applyParameterizations(type->typedata.lambda.arguments, parameters, parameterizations);
	} else if(type->type == TYPE_LIST) {
		applyParameterizations(&type->typedata.list.contained, parameters, parameterizations);
	} else if(type->type == TYPE_OPTIONAL) {
		applyParameterizations(&type->typedata.optional.contained, parameters, parameterizations);
	}
}

void TypeParameterizer::applyParameterizations(TypeArray* types, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations) {
	int i;
	for(i = 0; i < types->typecount; i++) {
		applyParameterizations(&types->types[i], parameters, parameterizations);
	}
}

bool TypeParameterizer::captureArgumentParameterizations(Type* actual, Type* argument, std::map<std::string, Type*>& parameters, TypeAnalyzer* analyzer) {
	if((actual == NULL) != (argument == NULL)) {
		return false;
	}

	if(actual == NULL && argument == NULL) {
		return true;
	}

	// Don't bind T to matchall in, for instance, passing [] into T[]
	if(actual->type == TYPE_MATCHALL) {
		return true;
	}

	if(argument->type == TYPE_PARAMETERIZED_ARG) {
		string label = argument->typedata.parameterized.label;
		if(parameters.find(label) != parameters.end()) {
			return analyzer->isAExactlyB(parameters[label], actual);
		}

		parameters[label] = actual;
		return true;
	}

	if(actual->type != argument->type) {
		return false;
	}

	if(actual->type == TYPE_CLASS) {
		return captureArgumentParameterizations(actual->typedata._class.parameters, argument->typedata._class.parameters, parameters, analyzer);
	} else if(actual->type == TYPE_LAMBDA) {
		return captureArgumentParameterizations(actual->typedata.lambda.arguments, argument->typedata.lambda.arguments, parameters, analyzer)
			&& captureArgumentParameterizations(actual->typedata.lambda.returntype, argument->typedata.lambda.returntype, parameters, analyzer);
	} else if(actual->type == TYPE_LIST) {
		return captureArgumentParameterizations(actual->typedata.list.contained, argument->typedata.list.contained, parameters, analyzer);
	} else if(actual->type == TYPE_OPTIONAL) {
		return captureArgumentParameterizations(actual->typedata.optional.contained, argument->typedata.optional.contained, parameters, analyzer);
	}

	return false; // maybe this should be vacuously true, but I don't think so...
}

bool TypeParameterizer::captureArgumentParameterizations(TypeArray* actual, TypeArray* argument, std::map<std::string, Type*>& parameters, TypeAnalyzer* analyzer) {
	int i;
	int actualCount = actual == NULL ? 0 : actual->typecount;
	int argumentCount = argument == NULL ? 0 : argument->typecount;

	if(actualCount != argumentCount) return false;

	for(i = 0; i < actualCount; i++)
	if(!captureArgumentParameterizations(actual->types[i], argument->types[i], parameters, analyzer)) {
		return false;
	}

	return true;
}
