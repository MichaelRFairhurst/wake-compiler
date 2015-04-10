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
#endif

struct PureTypeArray;
struct ClassVarRef;

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
		PureType();
		PureType(int type);
		PureType(const PureType& other);
		PureType& operator=(const PureType& other);

		ClassVarRef createClassVarRef();
		std::string getFQClassname();
		std::string toString();

	private:
		void releaseData();
		void deepCopy(const PureType& other);
#endif
} PureType;

#ifdef __cplusplus
extern "C" {
#endif

PureType* makePureType(int type);
void freePureType(PureType* t);

#ifdef __cplusplus
}

#include <algorithm>

namespace std
{
	template<>
	void swap(PureType& lhs, PureType& rhs)
	{
		void* ptr1;
		void* ptr2;
		void* ptr3;

		if(rhs.type == TYPE_LAMBDA) {
			ptr1 = rhs.typedata.lambda.arguments;
			ptr2 = rhs.typedata.lambda.returntype;
		} else if(rhs.type == TYPE_CLASS) {
			ptr1 = rhs.typedata._class.classname;
			ptr2 = rhs.typedata._class.modulename;
			ptr3 = rhs.typedata._class.parameters;
		} else if(rhs.type == TYPE_PARAMETERIZED || rhs.type == TYPE_PARAMETERIZED_ARG) {
			ptr1 = rhs.typedata.parameterized.label;
			ptr2 = rhs.typedata.parameterized.upperbound;
			ptr3 = rhs.typedata.parameterized.lowerbound;
		} else if(rhs.type == TYPE_LIST) {
			ptr1 = rhs.typedata.list.contained;
		} else if(rhs.type == TYPE_OPTIONAL) {
			ptr1 = rhs.typedata.optional.contained;
		}

		if(lhs.type == TYPE_LAMBDA) {
			rhs.typedata.lambda.arguments = lhs.typedata.lambda.arguments;
			rhs.typedata.lambda.returntype = lhs.typedata.lambda.returntype;
		} else if(lhs.type == TYPE_CLASS) {
			rhs.typedata._class.classname = lhs.typedata._class.classname;
			rhs.typedata._class.modulename = lhs.typedata._class.modulename;
			rhs.typedata._class.parameters = lhs.typedata._class.parameters;
		} else if(lhs.type == TYPE_PARAMETERIZED || lhs.type == TYPE_PARAMETERIZED_ARG) {
			rhs.typedata.parameterized.label = lhs.typedata.parameterized.label;
			rhs.typedata.parameterized.upperbound = lhs.typedata.parameterized.upperbound;
			rhs.typedata.parameterized.lowerbound = lhs.typedata.parameterized.lowerbound;
		} else if(lhs.type == TYPE_LIST) {
			rhs.typedata.list.contained = lhs.typedata.list.contained;
		} else if(lhs.type == TYPE_OPTIONAL) {
			rhs.typedata.optional.contained = lhs.typedata.optional.contained;
		}

		if(rhs.type == TYPE_LAMBDA) {
			lhs.typedata.lambda.arguments = (PureTypeArray*) ptr1;
			lhs.typedata.lambda.returntype = (PureType*) ptr2;
		} else if(rhs.type == TYPE_CLASS) {
			lhs.typedata._class.classname = (char*) ptr1;
			lhs.typedata._class.modulename = (char*) ptr2;
			lhs.typedata._class.parameters = (PureTypeArray*) ptr3;
		} else if(rhs.type == TYPE_PARAMETERIZED || rhs.type == TYPE_PARAMETERIZED_ARG) {
			lhs.typedata.parameterized.label = (char*) ptr1;
			lhs.typedata.parameterized.upperbound = (PureType*) ptr2;
			lhs.typedata.parameterized.lowerbound = (PureType*) ptr3;
		} else if(rhs.type == TYPE_LIST) {
			lhs.typedata.list.contained = (PureType*) ptr1;
		} else if(rhs.type == TYPE_OPTIONAL) {
			lhs.typedata.optional.contained = (PureType*) ptr1;
		}

		std::swap(lhs.type, rhs.type);
	}
}

#endif

#endif
