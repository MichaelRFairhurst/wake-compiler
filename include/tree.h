#ifndef HEADER_TREE
#define HEADER_TREE

#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "type.h"
#include "y.tab.h"

#define NT_PROGRAM 0

#define NT_EMPTY 1

#define NT_IMPORT 2
#define NT_IMPORTSET 3
#define NT_IMPORTTARGET 4
#define NT_IMPORTPATH 5

#define NT_CLASS 6
#define NT_CLASSSET 7
#define NT_CLASSBODY 8

#define NT_INHERITANCE 9
#define NT_INHERITANCESET 10
#define NT_SUBCLASS 11
#define NT_INTERFACE 12

#define NT_PROVISIONS 13
#define NT_PROVISION 14
#define NT_INJECTION 17
#define NT_INJECTION_ARGS 18
#define NT_INJECTION_ARG 19

#define NT_CTOR 20
#define NT_CTOR_ARGS 21

#define NT_METHOD_DECLARATION 22
#define NT_METHOD_RETURN_TYPE 23
#define NT_ABSTRACT_METHOD 24
#define NT_METHOD_NAME 25
#define NT_METHOD_NAME_SEGMENT 26

#define NT_THIS 27
#define NT_PARENT 28
#define NT_TYPE_ARRAY 29
#define NT_TYPEDATA 30
#define NT_CLASSNAME 31
#define NT_CURRIED 32
#define NT_STRINGLIT 33
#define NT_NUMBERLIT 34
#define NT_TRUTHLIT 35
#define NT_ALIAS 36

#define NT_ARRAY_ACCESS 37
#define NT_MEMBER_ACCESS 38
#define NT_METHOD_INVOCATION 39
#define NT_LAMBDA_INVOCATION 40

#define NT_BLOCK 41
#define NT_EXPRESSIONS 42
#define NT_RETRIEVALS_STATEMENTS 43
#define NT_RETRIEVAL 44
#define NT_ASSIGNMENT 45
#define NT_DECLARATION 74

#define NT_CASE 46
#define NT_DEFAULTCASE 47
#define NT_IF_ELSE 48
#define NT_SWITCH 49
#define NT_WHILE 50
#define NT_FOR 51
#define NT_BREAK 52
#define NT_CONTINUE 53
#define NT_RETURN 54
#define NT_IF_THEN_ELSE 55

#define NT_VALUES 56
#define NT_ARRAY_DECLARATION 57
#define NT_INCREMENT 58
#define NT_DECREMENT 59
#define NT_INVERT 60
#define NT_MULTIPLY 61
#define NT_DIVIDE 62
#define NT_ADD 63
#define NT_SUBTRACT 64
#define NT_LESSTHAN 65
#define NT_GREATERTHAN 66
#define NT_LESSTHANEQUAL 67
#define NT_GREATERTHANEQUAL 68
#define NT_EQUALITY 69
#define NT_INEQUALITY 70
#define NT_AND 71
#define NT_OR 72

#define NT_PROPERTY 73
#define NT_CAST 75

void AddSubNode(Node* parent, Node* child);

void PrependSubNode(Node* parent, Node* child);

Node* MakeEmptyNode(int nodetype);

Node* MakeNodeFromType(Type* thetype);

Node* MakeNodeFromTypeArray(TypeArray* thearray);

Node* MakeNodeFromString(int nodetype, char* mystring);

Node* MakeNodeFromNumber(int nodetype, float number);

Node* MakeTwoBranchNode(int nodetype, Node* a, Node* b);

Node* MakeOneBranchNode(int nodetype, Node* a);

char* getTypeLabel(Type* thetype);

void printtree (Node *n, int level);

void freeNode (Node *n);

#endif
