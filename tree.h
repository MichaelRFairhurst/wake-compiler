#ifndef HEADER_TREE
#define HEADER_TREE

#include <stdlib.h>
#include <string.h>
#include "node.h"
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
#define NT_INJECTED_CTOR_ARG 17
#define NT_CTOR 18
#define NT_ARGUMENTS 19
#define NT_ARGUMENT 20
#define NT_SHADOW 21


Node* MakeNodeFromString(char* mystring);

void AddSubNode(Node* parent, Node* child);

Node* MakeNodeFromNumber(int number);

Node* MakeTwoBranchNode(int nodetype, Node* a, Node* b);

Node* MakeOneBranchNode(int nodetype, Node* a);

void printtree (Node *n, int level);

#endif
