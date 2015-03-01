/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * tree.c
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By: Nathan Fairhurst
 *
 **************************************************/

#include <stdio.h>
#include "tree.h"
#include "wakelltype.h"

extern int line;
extern int column;

char *nodenames[83];
int treeTypesInited = 0;

void initTreeTypes() {
	if(treeTypesInited) return;
	nodenames[NT_CLASS] = "NT_CLASS";
	nodenames[NT_CLASSSET] = "NT_CLASSSET";
	nodenames[NT_IMPORT] = "NT_IMPORT";
	nodenames[NT_IMPORTSET] = "NT_IMPORTSET";
	nodenames[NT_INHERITANCE] = "NT_INHERITANCE";
	nodenames[NT_INHERITANCESET] = "NT_INHERITANCESET";
	nodenames[NT_PROGRAM] = "NT_PROGRAM";
	nodenames[NT_CLASSBODY] = "NT_CLASSBODY";
	nodenames[NT_PROVISIONS] = "NT_PROVISIONS";
	nodenames[NT_PROVISION] = "NT_PROVISION";
	nodenames[NT_INJECTION] = "NT_INJECTION";
	nodenames[NT_SUBINJECTIONS] = "NT_SUBINJECTIONS";
	nodenames[NT_SUBINJECTION] = "NT_SUBINJECTION";
	nodenames[NT_CTOR] = "NT_CTOR";
	nodenames[NT_EMPTY] = "NT_EMPTY";
	nodenames[NT_INTERFACE] = "NT_INTERFACE";
	nodenames[NT_SUBCLASS] = "NT_SUBCLASS";
	nodenames[NT_IMPORTTARGET] = "NT_IMPORTTARGET";
	nodenames[NT_BLOCK] = "NT_BLOCK";
	nodenames[NT_BOOLLIT] = "NT_BOOLLIT";
	nodenames[NT_STRINGLIT] = "NT_STRINGLIT";
	nodenames[NT_NUMBERLIT] = "NT_NUMBERLIT";
	nodenames[NT_CLASSNAME] = "NT_CLASSNAME";
	nodenames[NT_CURRIED] = "NT_CURRIED";
	nodenames[NT_IMPORTPATH] = "NT_IMPORTPATH";
	nodenames[NT_CTOR_ARGS] = "NT_CTOR_ARGS";
	nodenames[NT_METHOD_DECLARATION] = "NT_METHOD_DECLARATION";
	nodenames[NT_METHOD_RETURN_TYPE] = "NT_METHOD_RETURN_TYPE";
	nodenames[NT_ABSTRACT_METHOD] = "NT_ABSTRACT_METHOD";
	nodenames[NT_METHOD_NAME] = "NT_METHOD_NAME";
	nodenames[NT_METHOD_NAME_SEGMENT] = "NT_METHOD_NAME_SEGMENT";
	nodenames[NT_TYPEDATA] = "NT_TYPEDATA";
	nodenames[NT_VALUES] = "NT_VALUES";
	nodenames[NT_ALIAS] = "NT_ALIAS";
	nodenames[NT_THIS] = "NT_THIS";
	nodenames[NT_PARENT] = "NT_PARENT";
	nodenames[NT_ARRAY_ACCESS] = "NT_ARRAY_ACCESS";
	nodenames[NT_MEMBER_ACCESS] = "NT_MEMBER_ACCESS";
	nodenames[NT_METHOD_INVOCATION] = "NT_METHOD_INVOCATION";
	nodenames[NT_LAMBDA_INVOCATION] = "NT_LAMBDA_INVOCATION";
	nodenames[NT_ARRAY_DECLARATION] = "NT_ARRAY_DECLARATION";
	nodenames[NT_RETRIEVALS_STATEMENTS] = "NT_RETRIEVALS_STATEMENTS";
	nodenames[NT_RETRIEVAL] = "NT_RETRIEVAL";
	nodenames[NT_CASE] = "NT_CASE";
	nodenames[NT_DEFAULTCASE] = "NT_DEFAULTCASE";
	nodenames[NT_IF_ELSE] = "NT_IF_ELSE";
	nodenames[NT_SWITCH] = "NT_SWITCH";
	nodenames[NT_WHILE] = "NT_WHILE";
	nodenames[NT_FOR] = "NT_FOR";
	nodenames[NT_EXPRESSIONS] = "NT_EXPRESSIONS";
	nodenames[NT_BREAK] = "NT_BREAK";
	nodenames[NT_CONTINUE] = "NT_CONTINUE";
	nodenames[NT_RETURN] = "NT_RETURN";
	nodenames[NT_INCREMENT] = "NT_INCREMENT";
	nodenames[NT_DECREMENT] = "NT_DECREMENT";
	nodenames[NT_INVERT] = "NT_INVERT";
	nodenames[NT_MULTIPLY] = "NT_MULTIPLY";
	nodenames[NT_DIVIDE] = "NT_DIVIDE";
	nodenames[NT_ADD] = "NT_ADD";
	nodenames[NT_SUBTRACT] = "NT_SUBTRACT";
	nodenames[NT_LESSTHAN] = "NT_LESSTHAN";
	nodenames[NT_GREATERTHAN] = "NT_GREATERTHAN";
	nodenames[NT_LESSTHANEQUAL] = "NT_LESSTHANEQUAL";
	nodenames[NT_GREATERTHANEQUAL] = "NT_GREATERTHANEQUAL";
	nodenames[NT_EQUALITY] = "NT_EQUALITY";
	nodenames[NT_INEQUALITY] = "NT_INEQUALITY";
	nodenames[NT_BITAND] = "NT_BITAND";
	nodenames[NT_BITXOR] = "NT_BITXOR";
	nodenames[NT_BITOR] = "NT_BITOR";
	nodenames[NT_AND] = "NT_AND";
	nodenames[NT_OR] = "NT_OR";
	nodenames[NT_IF_THEN_ELSE] = "NT_IF_THEN_ELSE";
	nodenames[NT_ASSIGNMENT] = "NT_ASSIGNMENT";
	nodenames[NT_TYPE_ARRAY] = "NT_TYPE_ARRAY";
	nodenames[NT_DECLARATION] = "NT_DECLARATION";
	nodenames[NT_PROPERTY] = "NT_PROPERTY";
	nodenames[NT_CAST] = "NT_CAST";
	nodenames[NT_COMPILER_HINT] = "NT_COMPILER_HINT";
	nodenames[NT_BITSHIFTLEFT] = "NT_BITSHIFTLEFT";
	nodenames[NT_BITSHIFTRIGHT] = "NT_BITSHIFTRIGHT";
	nodenames[NT_BITNOT] = "NT_BITNOT";
	nodenames[NT_MOD] = "NT_MOD";
	nodenames[NT_MODNATIVE] = "NT_MODNATIVE";
	nodenames[NT_MODALT] = "NT_MODALT";
	treeTypesInited = 1;
}

void AddSubNode(Node* parent, Node* child) {
	parent->subnodes++;
	parent->node_data.nodes = realloc(parent->node_data.nodes, sizeof(Node*) * parent->subnodes);
	parent->node_data.nodes[parent->subnodes - 1] = child;
}

void PrependSubNode(Node* parent, Node* child) {
	int i;
	parent->subnodes++;
	parent->node_data.nodes = realloc(parent->node_data.nodes, sizeof(Node*) * parent->subnodes);
	for(i = parent->subnodes - 1; i > 0; i--) parent->node_data.nodes[i] = parent->node_data.nodes[i-1];
	parent->node_data.nodes[0] = child;
}

Node* NodeFactory(int nodetype, YYLTYPE loc) {
	Node* mynode = malloc(sizeof(Node));
	mynode->line = line;
	mynode->col = column;
	mynode->node_type = nodetype;
	mynode->subnodes = 0;
	mynode->loc = loc;
	return mynode;
}

Node* MakeEmptyNode(int nodetype, YYLTYPE loc) {
	Node* mynode = NodeFactory(nodetype, loc);
	mynode->node_data.nodes = NULL;
	return mynode;
}

Node* MakeNodeFromType(Type* thetype, YYLTYPE loc) {
	Node* mynode = NodeFactory(NT_TYPEDATA, loc);
	mynode->node_data.type = thetype;
	return mynode;
}

Node* MakeNodeFromTypeArray(TypeArray* thearray, YYLTYPE loc) {
	Node* mynode = NodeFactory(NT_TYPE_ARRAY, loc);
	mynode->node_data.typearray = thearray;

	return mynode;
}

Node* MakeNodeFromString(int nodetype, char* mystring, YYLTYPE loc) {
	Node* mynode = NodeFactory(nodetype, loc);
	mynode->node_data.string = mystring;
	//mynode->node_data.string = strdup(mystring);
	return mynode;
}

Node* MakeNodeFromNumber(int nodetype, double number, YYLTYPE loc) {
	Node* mynode = NodeFactory(nodetype, loc);
	mynode->node_data.number = number;
	return mynode;
}

Node* MakeTwoBranchNode(int nodetype, Node* a, Node* b, YYLTYPE loc) {
	Node* mynode = NodeFactory(nodetype, loc);
	mynode->subnodes = 2;
	mynode->node_data.nodes = malloc(2 * sizeof(Node*));
	mynode->node_data.nodes[0] = a; mynode->node_data.nodes[1] = b;
	return mynode;
}

Node* MakeOneBranchNode(int nodetype, Node* a, YYLTYPE loc) {
	Node* mynode = NodeFactory(nodetype, loc);
	mynode->subnodes = 1;
	mynode->node_data.nodes = malloc(1 * sizeof(Node*));
	mynode->node_data.nodes[0] = a;
	return mynode;
}

void printSubNodes(Node *n, int level, char* name) {
	printf ("%*c %s\n", level, ' ', name);
	if(n->subnodes == 0) return;
	int i;
	for(i = 0; i < n->subnodes; i++)
		printtree (n->node_data.nodes[i], level+1);
}

void printtype(Type* thetype, int level) {
	/*
	switch(thetype->type) {
		case TYPE_LAMBDA:
			printf("%*c lambda, []^%d, {%s}, @%s\n", level, ' ', thetype->arrayed, thetype->specialty, thetype->alias);
			if(thetype->typedata.lambda.returntype != NULL) {
				printf("%*c return:\n", level, ' ');
				printtype(thetype->typedata.lambda.returntype, level + 1);
			}
			printf("%*c arguments:\n", level, ' ');
			if(thetype->typedata.lambda.arguments != NULL) {
				int i;
				for(i = 0; i < thetype->typedata.lambda.arguments->typecount; i++) {
					printtype(thetype->typedata.lambda.arguments->types[i], level + 1);
				}
			}
			break;
		case TYPE_CLASS:
			printf("%*c class %s, $^%d, []^%d, {%s}, @%s\n", level, ' ', thetype->typedata._class.classname, thetype->typedata._class.shadow, thetype->arrayed, thetype->specialty, thetype->alias);
			break;
	}*/
}

void printtree (Node *n, int level) {
	if (!n) return;

	initTreeTypes();

	char* myname = nodenames[n->node_type];

	switch (n->node_type) {
		case NT_NUMBERLIT:
			printf("%*c %s %f\n", level, ' ', myname, n->node_data.number);
			break;
		case NT_IMPORTPATH:
		case NT_IMPORTTARGET:
		case NT_CLASSNAME:
		case NT_INTERFACE:
		case NT_SUBCLASS:
		case NT_STRINGLIT:
		case NT_METHOD_NAME_SEGMENT:
		case NT_ALIAS:
		case NT_COMPILER_HINT:
			printf("%*c %s %s\n", level, ' ', myname, n->node_data.string);
			break;
		case NT_TYPEDATA:
			printtype(n->node_data.type, level);
			break;
		case NT_TYPE_ARRAY:
			printf("%*c %s\n", level, ' ', myname);
			{
				int i;
				for(i = 0; i < n->node_data.typearray->typecount; i++)
					printtype(n->node_data.typearray->types[i], level + 1);
			}
			break;
		default:
			printSubNodes(n, level, myname); break;
	}
}

void freeNode(Node* n) {
	if(n->subnodes) {
		int i;
		for(i = 0; i < n->subnodes; i++) freeNode(n->node_data.nodes[i]);
		free(n->node_data.nodes);
	}

	switch(n->node_type) {
		case NT_IMPORTPATH:
		case NT_IMPORTTARGET:
		case NT_CLASSNAME:
		case NT_STRINGLIT:
		case NT_METHOD_NAME_SEGMENT:
		case NT_ALIAS:
		case NT_COMPILER_HINT:
			free(n->node_data.string);
			break;
		case NT_TYPEDATA:
			freeType(n->node_data.type);
			break;
		case NT_TYPE_ARRAY:
			freeTypeArray(n->node_data.typearray);
			break;
	}

	free(n);
}
