/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * type.c
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <stdlib.h>
#include <string.h>
#include "type.h"

Type* MakeType(int type) {
	Type* thetype = malloc(sizeof(Type));
	thetype->type = type;
	thetype->specialty = NULL;
	thetype->alias = NULL;
	switch(type) {
		case TYPE_CLASS:
			thetype->typedata._class.shadow = 0;
			thetype->typedata._class.parameters = NULL;
			thetype->typedata._class.classname = NULL;
			break;
		case TYPE_LAMBDA:
			thetype->typedata.lambda.returntype = NULL;
			thetype->typedata.lambda.arguments = NULL;//MakeTypeArray();
			break;
		case TYPE_PARAMETERIZED:
			thetype->typedata.parameterized.shadow = 0;
			thetype->typedata.parameterized.upperbound = NULL;
			thetype->typedata.parameterized.lowerbound = NULL;
			break;
		case TYPE_LIST:
			thetype->typedata.list.levels = 0;
			thetype->typedata.list.contained = NULL;
			break;
		case TYPE_OPTIONAL:
			thetype->typedata.optional.levels = 0;
			thetype->typedata.optional.contained = NULL;
			break;
	}
	return thetype;
}

TypeArray* MakeTypeArray() {
	TypeArray* thetype = malloc(sizeof(TypeArray));
	thetype->typecount = 0;
	thetype->types = NULL;

	return thetype;
}

void AddTypeToTypeArray(Type* nexttype, TypeArray* container) {
	container->typecount++;
	container->types = realloc(container->types, container->typecount * sizeof(Type*));
	container->types[container->typecount - 1] = nexttype;
}

void freeType(Type *t) {
	return;
	if(t == NULL) return;

	switch(t->type) {
		case TYPE_LAMBDA:
			freeTypeArray(t->typedata.lambda.arguments);
			freeType(t->typedata.lambda.returntype);
			break;

		case TYPE_CLASS:
			free(t->typedata._class.classname);
			freeTypeArray(t->typedata._class.parameters);
			break;

		case TYPE_PARAMETERIZED:
			free(t->typedata.parameterized.label);
			if(t->typedata.parameterized.upperbound != NULL) free(t->typedata.parameterized.upperbound);
			if(t->typedata.parameterized.lowerbound != NULL) free(t->typedata.parameterized.lowerbound);
			break;

		case TYPE_LIST:
			if(t->typedata.list.contained != NULL) free(t->typedata.list.contained);
			break;

		case TYPE_OPTIONAL:
			if(t->typedata.optional.contained != NULL) free(t->typedata.optional.contained);
			break;
	}

	if(t->alias) free(t->alias);
	if(t->specialty) free(t->specialty);

	free(t);
}

void freeTypeArray(TypeArray *ta) {
	return;
	if(ta == NULL) return;
	int i;
	for(i = 0; i < ta->typecount; i++) freeType(ta->types[i]);
	free(ta->types);
	free(ta);
}

Type* copyType(Type* t) {
	if(t == NULL) return NULL;

	Type* copy = malloc(sizeof(Type));
	copy->type = t->type;
	if(t->type == TYPE_LAMBDA) {
		copy->typedata.lambda.arguments = copyTypeArray(t->typedata.lambda.arguments);
		copy->typedata.lambda.returntype = copyType(t->typedata.lambda.returntype);
		copy->typedata.lambda.body = t->typedata.lambda.body;
	} else if(t->type == TYPE_CLASS) {
		copy->typedata._class.classname = t->typedata._class.classname == NULL ? NULL : strdup(t->typedata._class.classname);
		copy->typedata._class.shadow = t->typedata._class.shadow;					// number of $s
		copy->typedata._class.parameters = copyTypeArray(t->typedata._class.parameters);			// Xxx{T from Yyy}
	} else if(t->type == TYPE_PARAMETERIZED) {
		copy->typedata.parameterized.label = t->typedata.parameterized.label == NULL ? NULL : strdup(t->typedata.parameterized.label);
		copy->typedata.parameterized.shadow = t->typedata.parameterized.shadow;					// number of $s
		copy->typedata.parameterized.lowerbound = copyType(t->typedata.parameterized.lowerbound);
		copy->typedata.parameterized.upperbound = copyType(t->typedata.parameterized.upperbound);
	} else if(t->type == TYPE_LIST) {
		copy->typedata.list.contained = copyType(t->typedata.list.contained);
		copy->typedata.list.levels = t->typedata.list.levels;
	} else if(t->type == TYPE_OPTIONAL) {
		copy->typedata.optional.contained = copyType(t->typedata.optional.contained);
		copy->typedata.optional.levels = t->typedata.optional.levels;
	}
	copy->alias = t->alias == NULL ? NULL : strdup(t->alias);
	copy->specialty = t->specialty == NULL ? NULL : strdup(t->specialty);
	return copy;
}

TypeArray* copyTypeArray(TypeArray* ta) {
	if(ta == NULL) return NULL;
	TypeArray* copy = malloc(sizeof(TypeArray));
	copy->typecount = ta->typecount;
	copy->types = malloc(copy->typecount * sizeof(Type*));
	int i;
	for(i = 0; i < ta->typecount; i++) {
		copy->types[i] = copyType(ta->types[i]);
	}
	return copy;
}
