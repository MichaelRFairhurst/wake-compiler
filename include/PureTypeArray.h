/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PureTypeArray.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_PURE_TYPE_ARRAY
#define HEADER_PURE_TYPE_ARRAY

#include "PureType.h"

typedef struct PureTypeArray {
	PureType** types;
	int typecount;
#ifdef __cplusplus
	public:
		PureTypeArray();
		~PureTypeArray();
		PureTypeArray(const PureTypeArray& other);
		PureTypeArray& operator=(const PureTypeArray& other);
#endif
} PureTypeArray;

#ifdef __cplusplus
extern "C" {
#endif

PureTypeArray* makePureTypeArray();
void addPureTypeToPureTypeArray(PureType* nexttype, PureTypeArray* container);
void freePureTypeArray(PureTypeArray* ta);

#ifdef __cplusplus
}
#endif

#endif
