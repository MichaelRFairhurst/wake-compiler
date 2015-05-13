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

using namespace wake;

template<TypeQualification isQualified>
PureType<isQualified>::~PureType() {
	releaseData();
}

template<TypeQualification isQualified>
void PureType<isQualified>::releaseData() {
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

template<TypeQualification isQualified>
PureType<isQualified>::PureType(int type) {
	this->type = type;
	switch(type) {
		case TYPE_CLASS:
			typedata._class.parameters = NULL;
			typedata._class.classname = NULL;
			typedata._class.modulename = NULL;
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

template<TypeQualification isQualified>
ClassVarRef PureType<isQualified>::createClassVarRef() {
	switch(type) {
		case TYPE_CLASS:
			{
				return ClassVarRef(strdup(typedata._class.classname), 0, 0);
			}

		case TYPE_PARAMETERIZED:
		case TYPE_PARAMETERIZED_ARG:
			{
				return ClassVarRef(strdup(typedata.parameterized.label), 0, 0);
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

template<TypeQualification isQualified>
std::vector<PureType<isQualified>*> PureType<isQualified>::getClassParametersAsVector() {
	if(type != TYPE_CLASS) {
		throw std::runtime_error(std::string("Only class types, parameterized types, and lists/optionals around those types can be class references. All others require an alias"));
	}

	std::vector<PureType<isQualified>*> parameters;
	if(typedata._class.parameters != NULL) {
		for (int i = 0; i < typedata._class.parameters->typecount; i++) {
			parameters.push_back(typedata._class.parameters->types[i]);
		}
	}

	return parameters;
}

template<TypeQualification isQualified>
PureType<isQualified>::PureType(const PureType<isQualified>& other) {
	deepCopy(other);
}

template<TypeQualification isQualified>
void PureType<isQualified>::deepCopy(const PureType<isQualified>& other) {
	type = other.type;
	switch(type) {
		case TYPE_CLASS:
			typedata._class.classname = other.typedata._class.classname ? strdup(other.typedata._class.classname) : NULL;
			if(other.typedata._class.modulename != NULL) {
				typedata._class.modulename = strdup(other.typedata._class.modulename);
			} else {
				typedata._class.modulename = NULL;
			}
			if(other.typedata._class.parameters != NULL) {
				typedata._class.parameters = new PureTypeArray<isQualified>(*other.typedata._class.parameters);
			} else {
				typedata._class.parameters = NULL;
			}
			break;
		case TYPE_LAMBDA:
			if(other.typedata.lambda.returntype != NULL) {
				typedata.lambda.returntype = new PureType<isQualified>(*other.typedata.lambda.returntype);
			} else {
				typedata.lambda.returntype = NULL;
			}
			if(other.typedata.lambda.arguments != NULL) {
				typedata.lambda.arguments = new PureTypeArray<isQualified>(*other.typedata.lambda.arguments);
			} else {
				typedata.lambda.arguments = NULL;
			}
			break;
		case TYPE_PARAMETERIZED:
		case TYPE_PARAMETERIZED_ARG:
			if(other.typedata.parameterized.upperbound != NULL) {
				typedata.parameterized.upperbound = new PureType<isQualified>(*other.typedata.parameterized.upperbound);
			} else {
				typedata.parameterized.upperbound = NULL;
			}
			if(other.typedata.parameterized.lowerbound != NULL) {
				typedata.parameterized.lowerbound = new PureType<isQualified>(*other.typedata.parameterized.lowerbound);
			} else {
				typedata.parameterized.lowerbound = NULL;
			}
			typedata.parameterized.label = strdup(other.typedata.parameterized.label);
			break;
		case TYPE_LIST:
			if(other.typedata.list.contained != NULL) {
				typedata.list.contained = new PureType<isQualified>(*other.typedata.list.contained);
			} else {
				typedata.list.contained = NULL;
			}
			break;
		case TYPE_OPTIONAL:
			if(other.typedata.optional.contained != NULL) {
				typedata.optional.contained = new PureType<isQualified>(*other.typedata.optional.contained);
			} else {
				typedata.optional.contained = NULL;
			}
			break;
	}
}

template<TypeQualification isQualified>
PureType<isQualified>& PureType<isQualified>::operator=(const PureType<isQualified>& other) {
	releaseData();
	deepCopy(other);
}

template<TypeQualification isQualified>
std::string PureType<isQualified>::toString() {
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
		name = typedata.lambda.returntype != NULL ? typedata.lambda.returntype->toString() : "";
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

template<TypeQualification isQualified>
std::string PureType<isQualified>::getFQClassname() {
	if(type != TYPE_CLASS) {
		return "";
	}

	std::string fqname = typedata._class.modulename != NULL ? typedata._class.modulename : "";

	if(fqname.size()) {
		fqname += ".";
	}
	fqname += typedata._class.classname;

	return fqname;
}

PureType<UNQUALIFIED>* makePureType(int type) {
	return new PureType<UNQUALIFIED>(type);
}

PureType<UNQUALIFIED>* makeTypeFromClassVarRef(ClassVarRef* ref) {
	if(ref->arrayed) {
		PureType<UNQUALIFIED>* outer = new PureType<UNQUALIFIED>(TYPE_LIST);
		ref->arrayed--;
		outer->typedata.list.contained = makeTypeFromClassVarRef(ref);
		ref->arrayed++;
		return outer;
	}

	PureType<UNQUALIFIED>* type = new PureType<UNQUALIFIED>(TYPE_CLASS);
	type->typedata._class.classname = strdup(ref->classname);
	return type;
}

void freePureType(PureType<UNQUALIFIED>* t) {
	delete t;
}

PureType<UNQUALIFIED>* copyPureType(PureType<UNQUALIFIED>* t) {
	return new PureType<UNQUALIFIED>(*t);
}

namespace std {
	template<TypeQualification isQualified>
	void swap(PureType<isQualified>& lhs, PureType<isQualified>& rhs) {
		void* ptr1;
		void* ptr2;
		void* ptr3;

		if(rhs.type == TYPE_LAMBDA) {
			ptr1 = rhs.typedata.lambda.arguments;
			ptr2 = rhs.typedata.lambda.returntype;
		} else if(rhs.type == TYPE_CLASS) {
			ptr1 = rhs.typedata._class.classname;
			ptr2 = rhs.typedata._class.modulename;
			ptr3 = rhs.typedata._class.parameters;
		} else if(rhs.type == TYPE_PARAMETERIZED || rhs.type == TYPE_PARAMETERIZED_ARG) {
			ptr1 = rhs.typedata.parameterized.label;
			ptr2 = rhs.typedata.parameterized.upperbound;
			ptr3 = rhs.typedata.parameterized.lowerbound;
		} else if(rhs.type == TYPE_LIST) {
			ptr1 = rhs.typedata.list.contained;
		} else if(rhs.type == TYPE_OPTIONAL) {
			ptr1 = rhs.typedata.optional.contained;
		}

		if(lhs.type == TYPE_LAMBDA) {
			rhs.typedata.lambda.arguments = lhs.typedata.lambda.arguments;
			rhs.typedata.lambda.returntype = lhs.typedata.lambda.returntype;
		} else if(lhs.type == TYPE_CLASS) {
			rhs.typedata._class.classname = lhs.typedata._class.classname;
			rhs.typedata._class.modulename = lhs.typedata._class.modulename;
			rhs.typedata._class.parameters = lhs.typedata._class.parameters;
		} else if(lhs.type == TYPE_PARAMETERIZED || lhs.type == TYPE_PARAMETERIZED_ARG) {
			rhs.typedata.parameterized.label = lhs.typedata.parameterized.label;
			rhs.typedata.parameterized.upperbound = lhs.typedata.parameterized.upperbound;
			rhs.typedata.parameterized.lowerbound = lhs.typedata.parameterized.lowerbound;
		} else if(lhs.type == TYPE_LIST) {
			rhs.typedata.list.contained = lhs.typedata.list.contained;
		} else if(lhs.type == TYPE_OPTIONAL) {
			rhs.typedata.optional.contained = lhs.typedata.optional.contained;
		}

		if(rhs.type == TYPE_LAMBDA) {
			lhs.typedata.lambda.arguments = (PureTypeArray<isQualified>*) ptr1;
			lhs.typedata.lambda.returntype = (PureType<isQualified>*) ptr2;
		} else if(rhs.type == TYPE_CLASS) {
			lhs.typedata._class.classname = (char*) ptr1;
			lhs.typedata._class.modulename = (char*) ptr2;
			lhs.typedata._class.parameters = (PureTypeArray<isQualified>*) ptr3;
		} else if(rhs.type == TYPE_PARAMETERIZED || rhs.type == TYPE_PARAMETERIZED_ARG) {
			lhs.typedata.parameterized.label = (char*) ptr1;
			lhs.typedata.parameterized.upperbound = (PureType<isQualified>*) ptr2;
			lhs.typedata.parameterized.lowerbound = (PureType<isQualified>*) ptr3;
		} else if(rhs.type == TYPE_LIST) {
			lhs.typedata.list.contained = (PureType<isQualified>*) ptr1;
		} else if(rhs.type == TYPE_OPTIONAL) {
			lhs.typedata.optional.contained = (PureType<isQualified>*) ptr1;
		}

		std::swap(lhs.type, rhs.type);
	}
}

template class PureType<QUALIFIED>;
template class PureType<UNQUALIFIED>;
template void std::swap(PureType<QUALIFIED>& lhs, PureType<QUALIFIED>& rhs);
template void std::swap(PureType<UNQUALIFIED>& lhs, PureType<UNQUALIFIED>& rhs);
