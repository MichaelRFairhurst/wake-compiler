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

#ifdef __cplusplus
#define TEMPL_UNQUALIFIED <wake::UNQUALIFIED>
#define TEMPL_RECURSE <isQualified>
template<wake::TypeQualification isQualified>
#else
#define TEMPL_UNQUALIFIED
#define TEMPL_RECURSE
typedef
#endif
struct SpecializablePureType {
	PureType TEMPL_RECURSE typedata;
	char* specialty;

#ifdef __cplusplus
	public:
		~SpecializablePureType();
		SpecializablePureType() : specialty(NULL) {};
		SpecializablePureType(SpecializablePureType<isQualified>& other);
		SpecializablePureType& operator=(SpecializablePureType<isQualified>& other);

		std::string toProvisionSymbol(std::vector<PureType<wake::QUALIFIED>*> &arguments);

	private:
		void releaseData();
		void deepCopy(const SpecializablePureType<isQualified>& other);
};
#else
} SpecializablePureType;
#endif

#ifdef __cplusplus
extern "C" {
#endif

SpecializablePureType TEMPL_UNQUALIFIED* makeSpecializablePureType(PureType TEMPL_UNQUALIFIED* inner);
void freeSpecializablePureType(SpecializablePureType TEMPL_UNQUALIFIED* spt);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<wake::TypeQualification isQualified>
	void swap(SpecializablePureType<isQualified>& lhs, SpecializablePureType<isQualified>& rhs);
}
#endif

#endif
