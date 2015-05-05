/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * SpecializablePureType.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_SPECIALIZABLE_PURE_TYPE
#define HEADER_SPECIALIZABLE_PURE_TYPE

#include "PureType.h"

#ifdef __cplusplus
#include <string>
#include <vector>
#endif

typedef struct SpecializablePureType {
	PureType typedata;
	char* specialty;

#ifdef __cplusplus
	public:
		~SpecializablePureType();
		SpecializablePureType() : specialty(NULL) {};
		SpecializablePureType(SpecializablePureType& other);
		SpecializablePureType& operator=(SpecializablePureType& other);

		std::string toProvisionSymbol(std::vector<PureType*> &arguments);

	private:
		void releaseData();
		void deepCopy(const SpecializablePureType& other);
#endif
} SpecializablePureType;

#ifdef __cplusplus
extern "C" {
#endif

SpecializablePureType* makeSpecializablePureType(PureType* inner);
void freeSpecializablePureType(SpecializablePureType* spt);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<>
	void swap(SpecializablePureType& lhs, SpecializablePureType& rhs);
}
#endif

#endif
