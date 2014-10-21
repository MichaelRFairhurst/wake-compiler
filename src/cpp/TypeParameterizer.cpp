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

void TypeParameterizer::writeInParameterizations(Type** typeaddr, const std::vector<Type*>& parameters) {
	Type* type = *typeaddr;

	if(type->type == TYPE_CLASS) {
		if(type->typedata._class.parameters != NULL) {
			writeInParameterizations(type->typedata._class.parameters, parameters);
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
			writeInParameterizations(&type->typedata.lambda.returntype, parameters);
		if(type->typedata.lambda.arguments != NULL)
			writeInParameterizations(type->typedata.lambda.arguments, parameters);
	} else if(type->type == TYPE_LIST) {
		writeInParameterizations(&type->typedata.list.contained, parameters);
		if(type->typedata.list.contained->type == TYPE_LIST) {
			auto_ptr<Type> doublewrapper(type->typedata.list.contained);
			type->typedata.list.levels += doublewrapper->typedata.list.levels;
			type->typedata.list.contained = doublewrapper->typedata.list.contained;
			doublewrapper->typedata.list.contained = NULL;
		}
	} else if(type->type == TYPE_OPTIONAL) {
		writeInParameterizations(&type->typedata.optional.contained, parameters);
		if(type->typedata.optional.contained->type == TYPE_OPTIONAL) {
			auto_ptr<Type> doublewrapper(type->typedata.optional.contained);
			type->typedata.optional.levels += doublewrapper->typedata.optional.levels;
			type->typedata.optional.contained = doublewrapper->typedata.optional.contained;
			doublewrapper->typedata.optional.contained = NULL;
		}
	}
}

void TypeParameterizer::writeInParameterizations(TypeArray* types, const std::vector<Type*>& parameters) {
	int i;
	for(i = 0; i < types->typecount; i++) {
		writeInParameterizations(&types->types[i], parameters);
	}
}

void TypeParameterizer::applyParameterizations(Type** typeaddr, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations) {
	Type* type = *typeaddr;

	if(type->type == TYPE_CLASS) {
		if(type->typedata._class.parameters != NULL) {
			applyParameterizations(type->typedata._class.parameters, parameters, parameterizations);
		}
	} else if(type->type == TYPE_PARAMETERIZED) {
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
		if(type->typedata.list.contained->type == TYPE_LIST) {
			auto_ptr<Type> doublewrapper(type->typedata.list.contained);
			type->typedata.list.levels += doublewrapper->typedata.list.levels;
			type->typedata.list.contained = doublewrapper->typedata.list.contained;
			doublewrapper->typedata.list.contained = NULL;
		}
	} else if(type->type == TYPE_OPTIONAL) {
		applyParameterizations(&type->typedata.optional.contained, parameters, parameterizations);
		if(type->typedata.optional.contained->type == TYPE_OPTIONAL) {
			auto_ptr<Type> doublewrapper(type->typedata.optional.contained);
			type->typedata.optional.levels += doublewrapper->typedata.optional.levels;
			type->typedata.optional.contained = doublewrapper->typedata.optional.contained;
			doublewrapper->typedata.optional.contained = NULL;
		}
	}
}

void TypeParameterizer::applyParameterizations(TypeArray* types, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations) {
	int i;
	for(i = 0; i < types->typecount; i++) {
		applyParameterizations(&types->types[i], parameters, parameterizations);
	}
}
