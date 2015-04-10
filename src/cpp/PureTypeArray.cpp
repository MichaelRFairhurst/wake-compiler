/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PureTypeArray.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "PureTypeArray.h"
#include "PureType.h"
#include <cstddef>
#include <cstdlib>
#include <string.h>

PureTypeArray::PureTypeArray() {
	typecount = 0;
	types = NULL;
}

PureTypeArray::PureTypeArray(const PureTypeArray& other) {
	typecount = 0;
	types = NULL;
	for(int i = 0; i < other.typecount; i++) {
		addPureTypeToPureTypeArray(new PureType(*other.types[i]), this);
	}
	typecount = other.typecount;
}

PureTypeArray& PureTypeArray::operator=(const PureTypeArray& other) {
	for(int i = 0; i < other.typecount; i++) {
		addPureTypeToPureTypeArray(new PureType(*other.types[i]), this);
	}
	typecount = other.typecount;
	return *this;
}

PureTypeArray::~PureTypeArray() {
	for(int i = 0; i < typecount; i++) {
		delete types[i];
	}
	free(types);
}

PureTypeArray* makePureTypeArray() {
	return new PureTypeArray();
}

void freePureTypeArray(PureTypeArray* ta) {
	delete ta;
}

PureTypeArray* copyPureTypeArray(PureTypeArray* ta) {
	return new PureTypeArray(*ta);
}
