#include "TypeParameterizer.h"

#include <string>
#include <string.h>

using namespace std;

void TypeParameterizer::writeInParameterizations(Type** typeaddr, const std::vector<Type*>& parameters) {
	Type* type = *typeaddr;

	if(type->type == TYPE_CLASS) {
		if(type->typedata._class.parameters != NULL) {
			writeInParameterizations(type->typedata._class.parameters, parameters);
		} else {
			for(vector<Type*>::const_iterator it = parameters.begin(); it != parameters.end(); ++it) {
				if(type->typedata._class.classname == string((*it)->typedata.parameterized.label)) {
					*typeaddr = copyType(*it);
					(*typeaddr)->typedata.parameterized.shadow = type->typedata._class.shadow;
					(*typeaddr)->arrayed = type->arrayed;
					(*typeaddr)->optional = type->optional;
					(*typeaddr)->alias = type->alias ? strdup(type->alias) : NULL;
					freeType(type);
					break; // We've found it, move along
				}
			}
		}
	} else if(type->type == TYPE_LAMBDA) {
		if(type->typedata.lambda.returntype != NULL)
			writeInParameterizations(&type->typedata.lambda.returntype, parameters);
		if(type->typedata.lambda.arguments != NULL)
			writeInParameterizations(type->typedata.lambda.arguments, parameters);
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
				*typeaddr = copyType(parameterizations.at(i));
				(*typeaddr)->arrayed += type->arrayed;
				(*typeaddr)->optional += type->optional;
				(*typeaddr)->alias = type->alias;
				// Shadows don't matter because applied parameterizations don't have to do with scopes
				freeType(type);
				break; // We've found it, move along
			}
		}
	} else if(type->type == TYPE_LAMBDA) {
		if(type->typedata.lambda.returntype != NULL)
			applyParameterizations(&type->typedata.lambda.returntype, parameters, parameterizations);
		if(type->typedata.lambda.arguments != NULL)
			applyParameterizations(type->typedata.lambda.arguments, parameters, parameterizations);
	}
}

void TypeParameterizer::applyParameterizations(TypeArray* types, const std::vector<Type*>& parameters, const std::vector<Type*>& parameterizations) {
	int i;
	for(i = 0; i < types->typecount; i++) {
		applyParameterizations(&types->types[i], parameters, parameterizations);
	}
}
