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

#ifdef __cplusplus
template<wake::TypeQualification isQualified>
#else
typedef
#endif
struct PureTypeArray {
	PureType TEMPL_RECURSE** types;
	int typecount;
#ifdef __cplusplus
	public:
		PureTypeArray();
		~PureTypeArray();
		PureTypeArray(const PureTypeArray<isQualified>& other);
		PureTypeArray& operator=(const PureTypeArray<isQualified>& other);
		void addType(PureType<isQualified>* other);
};
#else
} PureTypeArray;
#endif

#ifdef __cplusplus
extern "C" {
#endif

PureTypeArray TEMPL_UNQUALIFIED* makePureTypeArray();
void addPureTypeToPureTypeArray(PureType TEMPL_UNQUALIFIED* nexttype, PureTypeArray TEMPL_UNQUALIFIED* container);
void freePureTypeArray(PureTypeArray TEMPL_UNQUALIFIED* ta);

#ifdef __cplusplus
}
#endif

#endif
