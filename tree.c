#include <stdio.h>
#include "tree.h"

void AddSubNode(Node* parent, Node* child) {
	parent->subnodes++;
	parent->node_data.nodes = realloc(parent->node_data.nodes, sizeof(Node*) * parent->subnodes);
	parent->node_data.nodes[parent->subnodes - 1] = child;
}

Node* MakeNodeFromString(char* mystring) {
	Node* mynode = (Node*) malloc(sizeof(Node));
	mynode->subnodes = 0;
	mynode->node_data.string = strdup(mystring);
	mynode->node_type = NT_STRING;
	return mynode;
}

Node* MakeNodeFromNumber(int number) {
	Node* mynode = (Node*) malloc(sizeof(Node));
	mynode->subnodes = 0;
	mynode->node_data.number = number;
	mynode->node_type = NT_NUMBER;
	return mynode;
}

Node* MakeTwoBranchNode(int nodetype, Node* a, Node* b) {
	Node* mynode = (Node*) malloc(sizeof(Node));
	mynode->subnodes = 2;
	mynode->node_type = nodetype;
	mynode->node_data.nodes = malloc(2 * sizeof(Node*));
	mynode->node_data.nodes[0] = a; mynode->node_data.nodes[1] = b;
	return mynode;
}

Node* MakeOneBranchNode(int nodetype, Node* a) {
	Node* mynode = (Node*) malloc(sizeof(Node));
	mynode->subnodes = 1;
	mynode->node_type = nodetype;
	mynode->node_data.nodes = malloc(1 * sizeof(Node*));
	mynode->node_data.nodes[0] = a;
	return mynode;
}

void printSubNodes(Node *n, int level, char* name) {
	int i;
	printf ("%*c %s\n", level, ' ', name);
	for(i = 0; i < n->subnodes; i++)
		printtree (n->node_data.nodes[i], level+1);
}

void printtree (Node *n, int level) {
	if (!n) return;

	switch (n->node_type) {
		case NT_PROGRAM:
			printSubNodes(n, level, "PROGRAM"); break;
		case NT_CLASS:
			printSubNodes(n, level, "CLASS"); break;
		case NT_CLASSSET:
			printSubNodes(n, level, "CLASSES"); break;
		case NT_INHERITANCE:
			printSubNodes(n, level, "INHERITANCE"); break;
		case NT_INHERITANCESET:
			printSubNodes(n, level, "INHERITANCES"); break;
		case NT_IMPORTSET:
			printSubNodes(n, level, "IMPORTS"); break;
		case NT_IMPORT:
			printSubNodes(n, level, "IMPORT"); break;
		case NT_CLASSBODY:
			printSubNodes(n, level, "CLASSBODY"); break;
		case NT_PROVISIONS:
			printSubNodes(n, level, "PROVISIONS"); break;
		case NT_PROVISION:
			printSubNodes(n, level, "PROVISION"); break;
		case NT_PROVIDED:
			printSubNodes(n, level, "PROVIDED"); break;
		case NT_PROVIDING:
			printSubNodes(n, level, "PROVIDING"); break;
		case NT_INJECTED_CTOR:
			printSubNodes(n, level, "INJECTED CONSTRUCTOR"); break;
		case NT_INJECTED_CTOR_ARG:
			printSubNodes(n, level, "INJECTED CONSTRUCTOR ARG"); break;
		case NT_CTOR:
			printSubNodes(n, level, "CONSTRUCTOR"); break;
		case NT_ARGUMENTS:
			printSubNodes(n, level, "ARGUMENTS"); break;
		case NT_ARGUMENT:
			printSubNodes(n, level, "ARGUMENT"); break;
		case NT_SHADOW:
			printSubNodes(n, level, "SHADOW"); break;
		case NT_NUMBER:
			printf("%*c NUMBER %d\n", level, ' ', n->node_data.number);
			break;
		case NT_STRING:
			printf("%*c STRING %s\n", level, ' ', n->node_data.string);
			break;
		default:
			printf("%*c UNRECOGNIZED: %d\n", level, ' ', n->node_type);
	}
}
