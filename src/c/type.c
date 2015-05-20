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
#include "PureType.h"
#include "PureTypeArray.h"

void addPureTypeToPureTypeArray(PureType* nexttype, PureTypeArray* container) {
	container->typecount++;
	container->types = realloc(container->types, container->typecount * sizeof(PureType*));
	container->types[container->typecount - 1] = nexttype;
}
