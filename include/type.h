/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * type.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TYPE
#define HEADER_TYPE

#define TYPE_LAMBDA 1
#define TYPE_CLASS 2
#define TYPE_PARAMETERIZED 3
#define TYPE_MATCHALL 4
#define TYPE_NOTHING 5
#define TYPE_UNUSABLE 6

struct TypeArray;

typedef struct Type {
	int type;							// 1 lambda 2 class
	union {
		struct {
			struct TypeArray* arguments;// the args nbd
			struct Type* returntype;	// returns an X
			struct Node* body;			// parse tree of body
		} lambda;
		struct {
			char* classname;			// Bongo
			int shadow;					// number of $s
			struct TypeArray* parameters; // For tracking declarations with parameters
		} _class;
		struct {
			char* label;				// T
			int shadow;					// T and $T
			struct Type* upperbound;	// Container{T from List}
			struct Type* lowerbound;	// Container{T to List}
		} parameterized;
	} typedata;
	int arrayed;						// number of []
	char* alias;						// this should be moved....
	char* specialty;					// {Alive}
	int optional;
} Type;

typedef struct TypeArray {
	Type** types;
	int typecount;
} TypeArray;

Type* MakeType(int type);
TypeArray* MakeTypeArray();
void AddTypeToTypeArray(Type* nexttype, TypeArray* container);
void freeType(Type* t);
void freeTypeArray(TypeArray* ta);
Type* copyType(Type* t);
TypeArray* copyTypeArray(TypeArray* ta);

#endif
