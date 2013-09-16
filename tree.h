#ifndef HEADER_TREE
#define HEADER_TREE

#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "type.h"
#include "y.tab.h"

#define NT_NUMBER 1
#define NT_STRING 2
#define NT_CLASS 3
#define NT_CLASSSET 4
#define NT_IMPORT 5
#define NT_IMPORTSET 6
#define NT_INHERITANCE 7
#define NT_INHERITANCESET 8
#define NT_PROGRAM 9
#define NT_CLASSBODY 10
#define NT_PROVISIONS 11
#define NT_PROVISION 12
#define NT_PROVIDED 13
#define NT_PROVIDED_SPECIALIZED 14
#define NT_PROVIDING 15
#define NT_INJECTED_CTOR 16
#define NT_INJECTED_CTOR_ARGS 17
#define NT_INJECTED_CTOR_ARG 18
#define NT_CTOR 19
#define NT_ARGUMENTS 20
#define NT_ARGUMENT 21
#define NT_SHADOW 22
#define NT_EMPTY 23
#define NT_INTERFACE 24
#define NT_SUBCLASS 25
#define NT_IMPORTTARGET 26
#define NT_BLOCK 27
#define NT_STRINGLIT 28
#define NT_NUMBERLIT 29
#define NT_CLASSNAME 30
#define NT_CURRIED 31
#define NT_IMPORTPATH 32
#define NT_CTOR_ARGS 33
#define NT_METHOD_DECLARATION 33
#define NT_METHOD_RETURN_TYPE 34
#define NT_ABSTRACT_METHOD 35
#define NT_METHOD_NAME 36
#define NT_METHOD_NAME_SEGMENT 37
#define NT_TYPEDATA 38
#define NT_VALUES 39
#define NT_ALIAS 40
#define NT_ARRAY_ACCESS 41
#define NT_MEMBER_ACCESS 42
#define NT_METHOD_INVOCATION 43
#define NT_LAMBDA_INVOCATION 44
#define NT_ARRAY_DECLARATION 45
#define NT_RETRIEVALS_STATEMENTS 46
#define NT_RETRIEVAL 47
#define NT_CASE 48
#define NT_DEFAULTCASE 49
#define NT_IF_ELSE 50
#define NT_SWITCH 51
#define NT_WHILE 52
#define NT_FOR 53
#define NT_EXPRESSIONS 54
#define NT_BREAK 55
#define NT_CONTINUE 56
#define NT_RETURN 57
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
#define NT_IF_THEN_ELSE 73
#define NT_ASSIGNMENT 74
#define NT_TYPE_ARRAY 75

void AddSubNode(Node* parent, Node* child);

void PrependSubNode(Node* parent, Node* child);

Node* MakeEmptyNode(int nodetype);

Node* MakeNodeFromType(Type* thetype);

Node* MakeNodeFromTypeArray(TypeArray* thearray);

Node* MakeNodeFromString(int nodetype, char* mystring);

Node* MakeNodeFromNumber(int nodetype, int number);

Node* MakeTwoBranchNode(int nodetype, Node* a, Node* b);

Node* MakeOneBranchNode(int nodetype, Node* a);

char* getTypeLabel(Type* thetype);

void printtree (Node *n, int level);

#endif
