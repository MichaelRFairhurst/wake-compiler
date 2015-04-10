/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PureType.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "PureType.h"
#include "PureTypeArray.h"
#include "ClassVarRef.h"
#include <cstddef>
#include <cstdlib>
#include <string.h>
#include <string>
#include <stdexcept>

PureType::~PureType() {
	releaseData();
}

void PureType::releaseData() {
	if(type == TYPE_LAMBDA) {
		delete typedata.lambda.arguments;
		delete typedata.lambda.returntype;
	} else if(type == TYPE_CLASS) {
		free(typedata._class.classname);
		delete typedata._class.parameters;
	} else if(type == TYPE_PARAMETERIZED || type == TYPE_PARAMETERIZED_ARG) {
		free(typedata.parameterized.label);
		delete typedata.parameterized.upperbound;
		delete typedata.parameterized.lowerbound;
	} else if(type == TYPE_LIST) {
		delete typedata.list.contained;
	} else if(type == TYPE_OPTIONAL) {
		delete typedata.optional.contained;
	}
}

PureType::PureType(int type) {
	this->type = type;
	switch(type) {
		case TYPE_CLASS:
			typedata._class.parameters = NULL;
			typedata._class.classname = NULL;
			break;
		case TYPE_LAMBDA:
			typedata.lambda.returntype = NULL;
			typedata.lambda.arguments = NULL;
			break;
		case TYPE_PARAMETERIZED:
		case TYPE_PARAMETERIZED_ARG:
			typedata.parameterized.upperbound = NULL;
			typedata.parameterized.lowerbound = NULL;
			break;
		case TYPE_LIST:
			typedata.list.contained = NULL;
			break;
		case TYPE_OPTIONAL:
			typedata.optional.contained = NULL;
			break;
	}
}

ClassVarRef PureType::createClassVarRef() {
	switch(type) {
		case TYPE_CLASS:
			{
				return ClassVarRef(typedata._class.classname, 0, 0);
			}

		case TYPE_PARAMETERIZED:
		case TYPE_PARAMETERIZED_ARG:
			{
				return ClassVarRef(typedata.parameterized.label, 0, 0);
			}

		case TYPE_OPTIONAL:
			{
				return typedata.optional.contained->createClassVarRef();
			}

		case TYPE_LIST:
			{
				ClassVarRef ref(typedata.list.contained->createClassVarRef());
				ref.arrayed++;
				return ref;
			}

		default:
			throw std::runtime_error(std::string("Only class types, parameterized types, and lists/optionals around those types can be class references. All others require an alias"));

	}
}

PureType::PureType(const PureType& other) {
	deepCopy(other);
}

void PureType::deepCopy(const PureType& other) {
	type = other.type;
	switch(type) {
		case TYPE_CLASS:
			typedata._class.classname = other.typedata._class.classname ? strdup(other.typedata._class.classname) : NULL;
			if(other.typedata._class.parameters != NULL) {
				typedata._class.parameters = new PureTypeArray(*other.typedata._class.parameters);
			} else {
				typedata._class.parameters = NULL;
			}
			break;
		case TYPE_LAMBDA:
			if(other.typedata.lambda.returntype != NULL) {
				typedata.lambda.returntype = new PureType(*other.typedata.lambda.returntype);
			} else {
				typedata.lambda.returntype = NULL;
			}
			if(other.typedata.lambda.arguments != NULL) {
				typedata.lambda.arguments = new PureTypeArray(*other.typedata.lambda.arguments);
			} else {
				typedata.lambda.arguments = NULL;
			}
			break;
		case TYPE_PARAMETERIZED:
		case TYPE_PARAMETERIZED_ARG:
			if(other.typedata.parameterized.upperbound != NULL) {
				typedata.parameterized.upperbound = new PureType(*other.typedata.parameterized.upperbound);
			} else {
				typedata.parameterized.upperbound = NULL;
			}
			if(other.typedata.parameterized.lowerbound != NULL) {
				typedata.parameterized.lowerbound = new PureType(*other.typedata.parameterized.lowerbound);
			} else {
				typedata.parameterized.lowerbound = NULL;
			}
			typedata.parameterized.label = strdup(other.typedata.parameterized.label);
			break;
		case TYPE_LIST:
			if(other.typedata.list.contained != NULL) {
				typedata.list.contained = new PureType(*other.typedata.list.contained);
			} else {
				typedata.list.contained = NULL;
			}
			break;
		case TYPE_OPTIONAL:
			if(other.typedata.optional.contained != NULL) {
				typedata.optional.contained = new PureType(*other.typedata.optional.contained);
			} else {
				typedata.optional.contained = NULL;
			}
			break;
	}
}

PureType& PureType::operator=(const PureType& other) {
	releaseData();
	deepCopy(other);
}

std::string PureType::toString() {
	std::string name;

	if(type == TYPE_UNUSABLE) {
		return "{no type}";
	}

	if(type == TYPE_PARAMETERIZED || type == TYPE_PARAMETERIZED_ARG) {
		name = typedata.parameterized.label;
	}

	if(type == TYPE_MATCHALL) {
		return "{inferencing failed}";
	}

	if(type == TYPE_CLASS) {
		name = typedata._class.classname;
		if(typedata._class.parameters != NULL) {
			name += "{";
			int i;
			for(i = 0; i < typedata._class.parameters->typecount; i++) {
				if(i) name += ",";
				name += typedata._class.parameters->types[i]->toString();
			}
			name += "}";
		}
	} else if(type == TYPE_LAMBDA) {
		name = typedata.lambda.returntype->toString();
		name += "--(";

		if(typedata.lambda.arguments != NULL) {
			int i;
			for(i = 0; i < typedata.lambda.arguments->typecount; i++) {
				if(i) name += ",";
				name += typedata.lambda.arguments->types[i]->toString();
			}
		}
		name += ")";
	}

	if(type == TYPE_LIST) {
		name = typedata.list.contained->toString();
		name += "[]";
	}

	if(type == TYPE_OPTIONAL) {
		name = typedata.optional.contained->toString();
		name += "?";
	}

	return name;
}


std::string PureType::getFQClassname() {
	std::string fqname = typedata._class.modulename;

	if(fqname.size()) {
		fqname += ".";
	}
	fqname += typedata._class.classname;
}

PureType* makeType(int type) {
	return new PureType(type);
}

void freeType(PureType* t) {
	delete t;
}

PureType* copyType(PureType* t) {
	return new PureType(*t);
}
