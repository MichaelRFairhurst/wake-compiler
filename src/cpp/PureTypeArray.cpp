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

template<wake::TypeQualification isQualified>
PureTypeArray<isQualified>::PureTypeArray() {
	typecount = 0;
	types = NULL;
}

template<wake::TypeQualification isQualified>
PureTypeArray<isQualified>::PureTypeArray(const PureTypeArray<isQualified>& other) {
	typecount = 0;
	types = NULL;
	for(int i = 0; i < other.typecount; i++) {
		// Remember: this template is purely for the typesystem, we can cast without borking mem safety
		addPureTypeToPureTypeArray(
			new PureType<wake::UNQUALIFIED>((PureType<wake::UNQUALIFIED>) *other.types[i]),
			(PureTypeArray<wake::UNQUALIFIED>*) this
		);
	}
	typecount = other.typecount;
}

template<wake::TypeQualification isQualified>
PureTypeArray<isQualified>& PureTypeArray<isQualified>::operator=(const PureTypeArray& other) {
	for(int i = 0; i < other.typecount; i++) {
		// Remember: this template is purely for the typesystem, we can cast without borking mem safety
		addPureTypeToPureTypeArray(
			new PureType<wake::UNQUALIFIED>((PureType<wake::UNQUALIFIED>) *other.types[i]),
			(PureTypeArray<wake::UNQUALIFIED>*) this
		);
	}
	typecount = other.typecount;
	return *this;
}

template<wake::TypeQualification isQualified>
PureTypeArray<isQualified>::~PureTypeArray() {
	for(int i = 0; i < typecount; i++) {
		delete types[i];
	}
	free(types);
}

PureTypeArray<wake::UNQUALIFIED>* makePureTypeArray() {
	return new PureTypeArray<wake::UNQUALIFIED>();
}

void freePureTypeArray(PureTypeArray<wake::UNQUALIFIED>* ta) {
	delete ta;
}

PureTypeArray<wake::UNQUALIFIED>* copyPureTypeArray(PureTypeArray<wake::UNQUALIFIED>* ta) {
	return new PureTypeArray<wake::UNQUALIFIED>(*ta);
}
