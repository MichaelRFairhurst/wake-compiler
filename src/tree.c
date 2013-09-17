#include <stdio.h>
#include "tree.h"

extern int line;
extern int column;

char *nodenames[83];
int treeTypesInited = 0;

void initTreeTypes() {
	if(treeTypesInited) return;
	nodenames[NT_NUMBER] = "NT_NUMBER";
	nodenames[NT_STRING] = "NT_STRING";
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
	nodenames[NT_PROVIDED] = "NT_PROVIDED";
	nodenames[NT_PROVIDED_SPECIALIZED] = "NT_PROVIDED_SPECIALIZED";
	nodenames[NT_PROVIDING] = "NT_PROVIDING";
	nodenames[NT_INJECTED_CTOR] = "NT_INJECTED_CTOR";
	nodenames[NT_INJECTED_CTOR_ARGS] = "NT_INJECTED_CTOR_ARGS";
	nodenames[NT_INJECTED_CTOR_ARG] = "NT_INJECTED_CTOR_ARG";
	nodenames[NT_CTOR] = "NT_CTOR";
	nodenames[NT_ARGUMENTS] = "NT_ARGUMENTS";
	nodenames[NT_ARGUMENT] = "NT_ARGUMENT";
	nodenames[NT_SHADOW] = "NT_SHADOW";
	nodenames[NT_EMPTY] = "NT_EMPTY";
	nodenames[NT_INTERFACE] = "NT_INTERFACE";
	nodenames[NT_SUBCLASS] = "NT_SUBCLASS";
	nodenames[NT_IMPORTTARGET] = "NT_IMPORTTARGET";
	nodenames[NT_BLOCK] = "NT_BLOCK";
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
	nodenames[NT_AND] = "NT_AND";
	nodenames[NT_OR] = "NT_OR";
	nodenames[NT_IF_THEN_ELSE] = "NT_IF_THEN_ELSE";
	nodenames[NT_ASSIGNMENT] = "NT_ASSIGNMENT";
	nodenames[NT_TYPE_ARRAY] = "NT_TYPE_ARRAY";
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

Node* NodeFactory(int nodetype) {
	Node* mynode = (Node*) malloc(sizeof(Node));
	mynode->line = line;
	mynode->col = column;
	mynode->node_type = nodetype;
	mynode->subnodes = 0;
	return mynode;
}

Node* MakeEmptyNode(int nodetype) {
	Node* mynode = NodeFactory(nodetype);
	mynode->node_data.nodes = NULL;
	return mynode;
}

Node* MakeNodeFromType(Type* thetype) {
	Node* mynode = NodeFactory(NT_TYPEDATA);
	mynode->node_data.type = thetype;
	return mynode;
}

Node* MakeNodeFromTypeArray(TypeArray* thearray) {
	Node* mynode = NodeFactory(NT_TYPE_ARRAY);
	int i;
	for(i = 0; i < thearray->typecount; i++)
		AddSubNode(mynode, MakeNodeFromType(thearray->types[i]));

	free(thearray->types);
	free(thearray);

	return mynode;
}

Node* MakeNodeFromString(int nodetype, char* mystring) {
	Node* mynode = NodeFactory(nodetype);
	mynode->node_data.string = strdup(mystring);
	return mynode;
}

Node* MakeNodeFromNumber(int nodetype, int number) {
	Node* mynode = NodeFactory(nodetype);
	mynode->node_data.number = number;
	return mynode;
}

Node* MakeTwoBranchNode(int nodetype, Node* a, Node* b) {
	Node* mynode = NodeFactory(nodetype);
	mynode->subnodes = 2;
	mynode->node_data.nodes = malloc(2 * sizeof(Node*));
	mynode->node_data.nodes[0] = a; mynode->node_data.nodes[1] = b;
	return mynode;
}

Node* MakeOneBranchNode(int nodetype, Node* a) {
	Node* mynode = NodeFactory(nodetype);
	mynode->subnodes = 1;
	mynode->node_data.nodes = malloc(1 * sizeof(Node*));
	mynode->node_data.nodes[0] = a;
	return mynode;
}

void printSubNodes(Node *n, int level, char* name) {
	if(n->subnodes == 0) return;
	int i;
	printf ("%*c %s\n", level, ' ', name);
	for(i = 0; i < n->subnodes; i++)
		printtree (n->node_data.nodes[i], level+1);
}

char* getTypeLabel(Type* thetype) {
	char *string = malloc(100 * sizeof(char));
	switch(thetype->type) {
		case TYPE_LAMBDA:
			{
				char *rettype = thetype->typedata.lambda.returntype == NULL ? "void" : getTypeLabel(thetype->typedata.lambda.returntype);
				char *argtypes = NULL;
				if(thetype->typedata.lambda.arguments == NULL || thetype->typedata.lambda.arguments->typecount == 0) {
					argtypes = "none";
				} else {
					int i;
					int memlen = 1;
					argtypes = strdup("");
					for(i = 0; i < thetype->typedata.lambda.arguments->typecount; i++) {
						char *argtype = getTypeLabel(thetype->typedata.lambda.arguments->types[i]);
						memlen += strlen(argtype);
						if(i > 0) memlen += 2;
						argtypes = realloc(argtypes, sizeof(char) * memlen);
						if(i > 0) strcat(argtypes, ", ");
						strcat(argtypes, argtype);
						free(argtype);
					}
				}
				string = realloc(string, sizeof(char) * (100 + strlen(argtypes) + strlen(rettype)));
				sprintf(string, "lambda, []^%d, (%s -- (%s)), {%s}, @%s", thetype->arrayed, rettype, argtypes, thetype->specialty, thetype->alias);
				if(thetype->typedata.lambda.returntype) free(rettype);
				if(thetype->typedata.lambda.arguments && thetype->typedata.lambda.arguments->typecount) free(argtypes);
			}
			break;
		case TYPE_CLASS:
			sprintf(string, "class %s, $^%d, []^%d, {%s}, @%s", thetype->typedata._class.classname, thetype->typedata._class.shadow, thetype->arrayed, thetype->specialty, thetype->alias);
			break;
		default: printf("BAD BAD BAD BAD\n");
	}
	return string;
}

void printtree (Node *n, int level) {
	if (!n) return;

	initTreeTypes();

	char* myname = nodenames[n->node_type];

	switch (n->node_type) {
		case NT_NUMBERLIT:
			printf("%*c %s %d\n", level, ' ', myname, n->node_data.number);
			break;
		case NT_IMPORTPATH:
		case NT_IMPORTTARGET:
		case NT_CLASSNAME:
		case NT_INTERFACE:
		case NT_SUBCLASS:
		case NT_STRINGLIT:
		case NT_METHOD_NAME_SEGMENT:
		case NT_ALIAS:
			printf("%*c %s %s\n", level, ' ', myname, n->node_data.string);
			break;
		case NT_TYPEDATA:
			printf("%*c %s %s\n", level, ' ', myname, getTypeLabel(n->node_data.type));
			break;
		default:
			printSubNodes(n, level, myname); break;
	}
}
