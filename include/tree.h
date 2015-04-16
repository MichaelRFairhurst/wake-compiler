/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * tree.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By: Nathan Fairhurst
 *
 **************************************************/

#ifndef HEADER_TREE
#define HEADER_TREE

#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "wake.tab.h"

#define NT_PROGRAM 0

#define NT_EMPTY 1

#define NT_IMPORT 2
#define NT_IMPORTSET 3
#define NT_IMPORTTARGET 4
#define NT_IMPORTPATH 5
#define NT_MODULE 120

#define NT_CLASS 6
#define NT_CLASS_EXTERN 108
#define NT_CLASSSET 7
#define NT_CLASSBODY 8

#define NT_INHERITANCE 9
#define NT_INHERITANCESET 10
#define NT_SUBCLASS 11
#define NT_INTERFACE 12

#define NT_PROVISIONS 13
#define NT_PROVISION 14
#define NT_INJECTION 17
#define NT_SUBINJECTIONS 18
#define NT_SUBINJECTION 19
#define NT_INJECTION_ARG 93
#define NT_PROVISION_BEHAVIOR 94

#define NT_CTOR 20
#define NT_CTOR_ARGS 21
#define NT_CTOR_ARG 86

#define NT_METHOD_DECLARATION 22
#define NT_METHOD_RETURN_TYPE 23
#define NT_ABSTRACT_METHOD 24
#define NT_METHOD_NAME 25
#define NT_METHOD_NAME_SEGMENT 26

#define NT_THIS 27
#define NT_NOTHING 28
#define NT_PARENT 29
#define NT_TYPE_ARRAY 30
#define NT_TYPEDATA 31
#define NT_SPECIALIZABLE_TYPEDATA 121
#define NT_CLASSNAME 32
#define NT_CURRIED 33
#define NT_STRINGLIT 34
#define NT_NUMBERLIT 35
#define NT_BOOLLIT 36
#define NT_ALIAS 37

#define NT_ARRAY_ACCESS 38
#define NT_ARRAY_ACCESS_LVAL 92
#define NT_MEMBER_ACCESS 39
#define NT_METHOD_INVOCATION 40
#define NT_LAMBDA_INVOCATION 41
#define NT_INFERENCEABLE_TYPES 106
#define NT_LAMBDA_DECLARATION 107
#define NT_INVOCATION_PARTS_TEMP 105
#define NT_TYPESAFE_ARRAY_ACCESS 102
#define NT_EARLYBAILOUT_MEMBER_ACCESS 103
#define NT_EARLYBAILOUT_METHOD_INVOCATION 104

#define NT_BLOCK 42
#define NT_EXPRESSIONS 43
#define NT_RETRIEVALS_STATEMENTS 44
#define NT_RETRIEVAL 45
#define NT_ASSIGNMENT 46
#define NT_VALUED_ASSIGNMENT 81
#define NT_ADD_ASSIGNMENT 82
#define NT_SUB_ASSIGNMENT 83
#define NT_MULT_ASSIGNMENT 84
#define NT_DIV_ASSIGNMENT 85
#define NT_DECLARATION 75

#define NT_CASE 47
#define NT_DEFAULTCASE 48
#define NT_IF_ELSE 49
#define NT_SWITCH 50
#define NT_WHILE 51
#define NT_FOR 52
#define NT_FOREACH 87
#define NT_FOREACHIN 88
#define NT_BREAK 53
#define NT_THROW 89
#define NT_TRY 90
#define NT_CATCH 91
#define NT_CONTINUE 54
#define NT_RETURN 55
#define NT_IF_THEN_ELSE 56
#define NT_EXISTS 57

#define NT_VALUES 58
#define NT_ARRAY_DECLARATION 59
#define NT_INCREMENT 60
#define NT_DECREMENT 61
#define NT_INVERT 62
#define NT_MULTIPLY 63
#define NT_DIVIDE 64
#define NT_ADD 65
#define NT_SUBTRACT 66
#define NT_LESSTHAN 67
#define NT_GREATERTHAN 68
#define NT_LESSTHANEQUAL 69
#define NT_GREATERTHANEQUAL 70
#define NT_EQUALITY 71
#define NT_INEQUALITY 72
#define NT_AND 73
#define NT_OR 74

#define NT_PROPERTY 76
#define NT_CAST 77
#define NT_COMPILER_HINT 78
#define NT_AUTOBOX 86

#define NT_PRIVATE 79
#define NT_PUBLIC 80

#define NT_ANNOTATION 95
#define NT_ANNOTATIONS 100
#define NT_ANNOTATED_TYPE 96
#define NT_ANNOTATED_CLASS 97
#define NT_ANNOTATED_METHOD 98
#define NT_ANNOTATION_NAME 99
#define NT_ANNOTATION_VALS 101
#define NT_ANNOTATED_PROPERTY 109
#define NT_ANNOTATED_PROVISION 110

#define NT_BITSHIFTLEFT 111
#define NT_BITSHIFTRIGHT 112
#define NT_BITAND 113
#define NT_BITXOR 114
#define NT_BITOR 115
#define NT_BITNOT 116

#define NT_MOD 117
#define NT_MODNATIVE 118
#define NT_MODALT 119

void addSubNode(Node* parent, Node* child);

void prependSubNode(Node* parent, Node* child);

Node* makeEmptyNode(int nodetype, YYLTYPE loc);

Node* makeNodeFromPureType(PureType* thetype, YYLTYPE loc);

Node* makeNodeFromPureTypeArray(PureTypeArray* thearray, YYLTYPE loc);

Node* makeNodeFromString(int nodetype, char* mystring, YYLTYPE loc);

Node* makeNodeFromNumber(int nodetype, double number, YYLTYPE loc);

Node* makeTwoBranchNode(int nodetype, Node* a, Node* b, YYLTYPE loc);

Node* makeOneBranchNode(int nodetype, Node* a, YYLTYPE loc);

//char* getTypeLabel(Type* thetype);

void printtree (Node *n, int level);

void freeNode (Node *n);

#endif
