#include "TypeParameterizer.h"

#include <string>

using namespace std;

void TypeParameterizer::applyParameterizations(Type** typeaddr, const std::vector<Type*>& parameters) {
	Type* type = *typeaddr;

	if(type->type == TYPE_CLASS) {
		if(type->typedata._class.parameters != NULL) {
			applyParameterizations(type->typedata._class.parameters, parameters);
		} else {
			for(vector<Type*>::const_iterator it = parameters.begin(); it != parameters.end(); ++it) {
				if(type->typedata._class.classname == string((*it)->typedata.parameterized.label)) {
					*typeaddr = copyType(*it);
					(*typeaddr)->arrayed = type->arrayed;
					(*typeaddr)->optional = type->optional;
					(*typeaddr)->alias = type->alias;
					freeType(type);
					break; // We've found it, move along
				}
			}
		}
	} else if(type->type == TYPE_LAMBDA) {
		if(type->typedata.lambda.returntype != NULL)
			applyParameterizations(&type->typedata.lambda.returntype, parameters);
		if(type->typedata.lambda.arguments != NULL)
			applyParameterizations(type->typedata.lambda.arguments, parameters);
	}
}

void TypeParameterizer::applyParameterizations(TypeArray* types, const std::vector<Type*>& parameters) {
	int i;
	for(i = 0; i < types->typecount; i++) {
		applyParameterizations(&types->types[i], parameters);
	}
}
