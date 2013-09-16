#include <stdlib.h>
#include "type.h"

Type* MakeType(int type) {
	Type* thetype = malloc(sizeof(Type));
	thetype->type = type;
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
