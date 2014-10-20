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

void AddTypeToTypeArray(Type* nexttype, TypeArray* container) {
	container->typecount++;
	container->types = realloc(container->types, container->typecount * sizeof(Type*));
	container->types[container->typecount - 1] = nexttype;
}
