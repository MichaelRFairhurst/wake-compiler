#include "type.h"
#include <cstddef>
#include <cstdlib>
#include <string.h>

Type::~Type() {
	releaseData();
}

void Type::releaseData() {
	if(type == TYPE_LAMBDA) {
		delete typedata.lambda.arguments;
		delete typedata.lambda.returntype;
	} else if(type == TYPE_CLASS) {
		free(typedata._class.classname);
		delete typedata._class.parameters;
	} else if(type == TYPE_PARAMETERIZED) {
		free(typedata.parameterized.label);
		delete typedata.parameterized.upperbound;
		delete typedata.parameterized.lowerbound;
	} else if(type == TYPE_LIST) {
		delete typedata.list.contained;
	} else if(type == TYPE_OPTIONAL) {
		delete typedata.optional.contained;
	}
	free(alias);
	free(specialty);
}

Type::Type(int type) {
	this->type = type;
	specialty = NULL;
	alias = NULL;
	switch(type) {
		case TYPE_CLASS:
			typedata._class.shadow = 0;
			typedata._class.parameters = NULL;
			typedata._class.classname = NULL;
			break;
		case TYPE_LAMBDA:
			typedata.lambda.returntype = NULL;
			typedata.lambda.arguments = NULL;//MakeTypeArray();
			break;
		case TYPE_PARAMETERIZED:
			typedata.parameterized.shadow = 0;
			typedata.parameterized.upperbound = NULL;
			typedata.parameterized.lowerbound = NULL;
			break;
		case TYPE_LIST:
			typedata.list.levels = 0;
			typedata.list.contained = NULL;
			break;
		case TYPE_OPTIONAL:
			typedata.optional.levels = 0;
			typedata.optional.contained = NULL;
			break;
	}
}

Type::Type(const Type& other) {
	deepCopy(other);
}

void Type::deepCopy(const Type& other) {
	type = other.type;
	switch(type) {
		case TYPE_CLASS:
			typedata._class.shadow = other.typedata._class.shadow;
			typedata._class.classname = strdup(other.typedata._class.classname);
			if(other.typedata._class.parameters != NULL) {
				typedata._class.parameters = new TypeArray(*other.typedata._class.parameters);
			} else {
				typedata._class.parameters = NULL;
			}
			break;
		case TYPE_LAMBDA:
			if(other.typedata.lambda.returntype != NULL) {
				typedata.lambda.returntype = new Type(*other.typedata.lambda.returntype);
			} else {
				typedata.lambda.returntype = NULL;
			}
			if(other.typedata.lambda.arguments != NULL) {
				typedata.lambda.arguments = new TypeArray(*other.typedata.lambda.arguments);
			} else {
				typedata.lambda.arguments = NULL;
			}
			break;
		case TYPE_PARAMETERIZED:
			typedata.parameterized.shadow = other.typedata.parameterized.shadow;
			if(other.typedata.parameterized.upperbound != NULL) {
				typedata.parameterized.upperbound = new Type(*other.typedata.parameterized.upperbound);
			} else {
				typedata.parameterized.upperbound = NULL;
			}
			if(other.typedata.parameterized.lowerbound != NULL) {
				typedata.parameterized.lowerbound = new Type(*other.typedata.parameterized.lowerbound);
			} else {
				typedata.parameterized.lowerbound = NULL;
			}
			typedata.parameterized.label = strdup(other.typedata.parameterized.label);
			break;
		case TYPE_LIST:
			typedata.list.levels = other.typedata.list.levels;
			if(other.typedata.list.contained != NULL) {
				typedata.list.contained = new Type(*other.typedata.list.contained);
			} else {
				typedata.list.contained = NULL;
			}
			break;
		case TYPE_OPTIONAL:
			typedata.optional.levels = other.typedata.optional.levels;
			if(other.typedata.optional.contained != NULL) {
				typedata.optional.contained = new Type(*other.typedata.optional.contained);
			} else {
				typedata.optional.contained = NULL;
			}
			break;
	}
	if(other.alias != NULL) {
		alias = strdup(other.alias);
	} else {
		alias = NULL;
	}

	if(other.specialty != NULL) {
		specialty = strdup(other.specialty);
	} else {
		specialty = NULL;
	}
}

Type& Type::operator=(const Type& other) {
	releaseData();
	deepCopy(other);
}

TypeArray::TypeArray() {
	typecount = 0;
	types = NULL;
}

TypeArray::TypeArray(const TypeArray& other) {
	typecount = 0;
	types = NULL;
	for(int i = 0; i < other.typecount; i++) {
		AddTypeToTypeArray(new Type(*other.types[i]), this);
	}
	typecount = other.typecount;
}

TypeArray& TypeArray::operator=(const TypeArray& other) {
	for(int i = 0; i < other.typecount; i++) {
		AddTypeToTypeArray(new Type(*other.types[i]), this);
	}
	typecount = other.typecount;
	return *this;





































































































































































































}

TypeArray::~TypeArray() {
	for(int i = 0; i < typecount; i++) {
		delete types[i];
	}
	free(types);
}

Type* MakeType(int type) {
	return new Type(type);
}

TypeArray* MakeTypeArray() {
	return new TypeArray();
}

//void AddTypeToTypeArray(Type* nexttype, TypeArray* container);
void freeType(Type* t) {
	delete t;
}

void freeTypeArray(TypeArray* ta) {
	delete ta;
}

Type* copyType(Type* t) {
	return new Type(*t);
}

TypeArray* copyTypeArray(TypeArray* ta) {
	return new TypeArray(*ta);
}
