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
#include <string>
#include <vector>
#endif

struct PureTypeArray;
struct ClassVarRef;

typedef struct ClassVarRef ClassVarRef; // C necessity...

typedef struct PureType {
	int type;							// 1 lambda 2 class
	union {
		struct {
			struct PureTypeArray* arguments;// the args nbd
			struct PureType* returntype;	// returns an X
		} lambda;
		struct {
			char* classname;			// Bongo
			char* modulename;			// com.bongo
			struct PureTypeArray* parameters; // For tracking declarations with parameters
		} _class;
		struct {
			char* label;				// T
			struct PureType* upperbound;	// Container{T from List}
			struct PureType* lowerbound;	// Container{T to List}
		} parameterized;
		struct {
			struct PureType* contained;
		} list;
		struct {
			struct PureType* contained;
		} optional;
	} typedata;

#ifdef __cplusplus
	public:
		~PureType();
		PureType() : type(TYPE_MATCHALL) {};
		PureType(int type);
		PureType(const PureType& other);
		PureType& operator=(const PureType& other);

		ClassVarRef createClassVarRef();
		std::string getFQClassname();
		std::string toString();
		std::vector<PureType*> getClassParametersAsVector();

	private:
		void releaseData();
		void deepCopy(const PureType& other);
#endif
} PureType;

#ifdef __cplusplus
extern "C" {
#endif

PureType* makePureType(int type);
PureType* copyPureType(PureType* type);
PureType* makeTypeFromClassVarRef(ClassVarRef* ref);
void freePureType(PureType* t);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<>
	void swap(PureType& lhs, PureType& rhs);

}

#endif

#endif
