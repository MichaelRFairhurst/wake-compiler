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
using namespace wake;

template<TypeQualification isQualified>
void TypeParameterizer::rewriteClasstypesToParameterizedtypeByLabel(PureType<isQualified>* pType, const std::vector<PureType<isQualified>*>& parameters) {
	PureType<isQualified> typeOrig = *pType;

	if(pType->type == TYPE_CLASS) {
		if(pType->typedata._class.parameters != NULL) {
			rewriteClasstypesToParameterizedtypeByLabel(pType->typedata._class.parameters, parameters);
		} else {
			for(typename vector<PureType<isQualified>*>::const_iterator it = parameters.begin(); it != parameters.end(); ++it) {
				if(pType->typedata._class.classname == string((*it)->typedata.parameterized.label)) {
					*pType = PureType<isQualified>(**it);
					break; // We've found it, move along
				}
			}
		}
	} else if(pType->type == TYPE_LAMBDA) {
		if(pType->typedata.lambda.returntype != NULL)
			rewriteClasstypesToParameterizedtypeByLabel(pType->typedata.lambda.returntype, parameters);
		if(pType->typedata.lambda.arguments != NULL)
			rewriteClasstypesToParameterizedtypeByLabel(pType->typedata.lambda.arguments, parameters);
	} else if(pType->type == TYPE_LIST) {
		rewriteClasstypesToParameterizedtypeByLabel(pType->typedata.list.contained, parameters);
	} else if(pType->type == TYPE_OPTIONAL) {
		rewriteClasstypesToParameterizedtypeByLabel(pType->typedata.optional.contained, parameters);
	}
}

template void TypeParameterizer::rewriteClasstypesToParameterizedtypeByLabel(PureType<QUALIFIED>* pType, const std::vector<PureType<QUALIFIED>*>& parameters);
template void TypeParameterizer::rewriteClasstypesToParameterizedtypeByLabel(PureType<UNQUALIFIED>* pType, const std::vector<PureType<UNQUALIFIED>*>& parameters);

template<TypeQualification isQualified>
void TypeParameterizer::rewriteClasstypesToParameterizedtypeByLabel(PureTypeArray<isQualified>* types, const std::vector<PureType<isQualified>*>& parameters) {
	int i;
	for(i = 0; i < types->typecount; i++) {
		rewriteClasstypesToParameterizedtypeByLabel(types->types[i], parameters);
	}
}

void TypeParameterizer::applyParameterizations(PureType<QUALIFIED>* pType, const std::vector<PureType<QUALIFIED>*>& parameters, const std::vector<PureType<QUALIFIED>*>& parameterizations) {
	PureType<QUALIFIED> typeOrig = *pType;

	if(pType->type == TYPE_CLASS) {
		if(pType->typedata._class.parameters != NULL) {
			applyParameterizations(pType->typedata._class.parameters, parameters, parameterizations);
		}
	} else if(pType->type == TYPE_PARAMETERIZED || pType->type == TYPE_PARAMETERIZED_ARG) {
		for(int i = 0; i < parameters.size(); i++) {
			if(pType->typedata.parameterized.label == string(parameters.at(i)->typedata.parameterized.label)) {
				*pType = PureType<QUALIFIED>(*parameterizations.at(i));
				// Shadows don't matter because applied parameterizations don't have to do with scopes
				break; // We've found it, move along
			}
		}
	} else if(pType->type == TYPE_LAMBDA) {
		if(pType->typedata.lambda.returntype != NULL)
			applyParameterizations(pType->typedata.lambda.returntype, parameters, parameterizations);
		if(pType->typedata.lambda.arguments != NULL)
			applyParameterizations(pType->typedata.lambda.arguments, parameters, parameterizations);
	} else if(pType->type == TYPE_LIST) {
		applyParameterizations(pType->typedata.list.contained, parameters, parameterizations);
	} else if(pType->type == TYPE_OPTIONAL) {
		applyParameterizations(pType->typedata.optional.contained, parameters, parameterizations);
	}
}

void TypeParameterizer::applyParameterizations(PureTypeArray<QUALIFIED>* types, const std::vector<PureType<QUALIFIED>*>& parameters, const std::vector<PureType<QUALIFIED>*>& parameterizations) {
	int i;
	for(i = 0; i < types->typecount; i++) {
		applyParameterizations(types->types[i], parameters, parameterizations);
	}
}

bool TypeParameterizer::captureArgumentParameterizations(PureType<QUALIFIED>* actual, PureType<QUALIFIED>* argument, std::map<std::string, PureType<QUALIFIED>*>& parameters, TypeAnalyzer* analyzer) {
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

bool TypeParameterizer::captureArgumentParameterizations(PureTypeArray<QUALIFIED>* actual, PureTypeArray<QUALIFIED>* argument, std::map<std::string, PureType<QUALIFIED>*>& parameters, TypeAnalyzer* analyzer) {
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
