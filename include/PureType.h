/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PureType.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_PURE_TYPE
#define HEADER_PURE_TYPE

#define TYPE_LAMBDA 1
#define TYPE_CLASS 2
#define TYPE_PARAMETERIZED 3
#define TYPE_PARAMETERIZED_ARG 9
#define TYPE_MATCHALL 4
#define TYPE_UNUSABLE 6
#define TYPE_LIST 7
#define TYPE_OPTIONAL 8

#ifdef __cplusplus
#include "TypeQualification.h"
#include <string>
#include <vector>

template<wake::TypeQualification isQualified>
#endif
struct PureTypeArray;
struct ClassVarRef;

typedef struct ClassVarRef ClassVarRef; // C necessity...

// Make PureType<IsQualified> so we have compiletime safety
// in mixing up unqualified vs fully qualified types

#ifdef __cplusplus
#define TEMPL_RECURSE <isQualified>
#define TEMPL_UNQUALIFIED <wake::UNQUALIFIED>
template<wake::TypeQualification isQualified = wake::UNQUALIFIED>
#else
#define TEMPL_RECURSE
typedef
#endif
struct PureType {
	int type;							// 1 lambda 2 class
	union {
		struct {
			struct PureTypeArray TEMPL_RECURSE* arguments;// the args nbd
			struct PureType TEMPL_RECURSE* returntype;	// returns an X
		} lambda;
		struct {
			char* classname;			// Bongo
			char* modulename;			// com.bongo
			struct PureTypeArray TEMPL_RECURSE* parameters; // For tracking declarations with parameters
		} _class;
		struct {
			char* label;				// T
			struct PureType TEMPL_RECURSE* upperbound;	// Container{T from List}
			struct PureType TEMPL_RECURSE* lowerbound;	// Container{T to List}
		} parameterized;
		struct {
			struct PureType TEMPL_RECURSE* contained;
		} list;
		struct {
			struct PureType TEMPL_RECURSE* contained;
		} optional;
	} typedata;

#ifdef __cplusplus
	public:
		~PureType();
		PureType() : type(TYPE_MATCHALL) {};
		PureType(int type);
		PureType(const PureType<isQualified>& other);
		PureType& operator=(const PureType<isQualified>& other);

		ClassVarRef createClassVarRef();
		std::string getFQClassname();
		std::string toString();
		std::vector<PureType<isQualified>*> getClassParametersAsVector();

	private:
		void releaseData();
		void deepCopy(const PureType<isQualified>& other);
#endif
}
#ifndef __cplusplus
PureType;
#else
;

extern "C" {
#endif

PureType TEMPL_UNQUALIFIED* makePureType(int type);
PureType TEMPL_UNQUALIFIED* copyPureType(PureType TEMPL_UNQUALIFIED* type);
PureType TEMPL_UNQUALIFIED* makeTypeFromClassVarRef(ClassVarRef* ref);
void freePureType(PureType TEMPL_UNQUALIFIED* t);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<wake::TypeQualification isQualified>
	void swap(PureType<isQualified>& lhs, PureType<isQualified>& rhs);

}

#endif

#endif
