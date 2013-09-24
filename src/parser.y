%{
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "node.h"
#include "tree.h"
#include "type.h"

int line = 1;
int column = 1;
extern char* yytext;
Node* parsetree;

void yyerror(const char *str)
{
        fprintf(stderr, "[ Error                             ]\n");
        fprintf(stderr, "[ On line %d col %d                 ]\n", line, column);
        fprintf(stderr, "[ %s                                ]\n", str);
        fprintf(stderr, "[ Unexpected %s                     ]\n", yytext);
        fprintf(stderr, "[ yeah this spacing aint pretty yet ]\n");
}

int yywrap()
{
        return 1;
}

%}

/* keywords */
%token EVERY CAPABLE A_OR_AN IS RETURN FOREACH WITH PUBLIC IF ELSE WHILE IN IMPORT PROVIDES NEEDS THEN NOTHING SWITCH CASE DEFAULT BREAK FOR DO CONTINUE THIS PARENT FN
/* symbols */
%token SYM_CURRIER SYM_LE SYM_PROVIDE SYM_RETURN_DECREMENT SYM_AND SYM_OR SYM_EQ SYM_NE SYM_GE SYM_INCREMENT
/* this too */
%token ERRORTOKEN

%union
{
        float number;
        char *string;
        Node *node;
        Type *type;
        TypeArray *type_array;
}

/* identifiers & values */
%token <string> IDENTIFIER
%token <string> ALIAS
%token <string> SPECIALTY
%token <string> STRING
%token <number> NUMBER
%token <number> TRUTH
%token <number> SYM_SHADOW
%token <number> SYM_ARRAYED
%type <node> imports import import importtarget classes class parentage inheritances inheritance classbody classprop injection_providable injection_ctor injection_ctorargs injection_binding injection_bindings injection_ctorarg ctor ctorargs expression value method block methodreturn methodnamesegments methodbody methodcallsegments curryableexpressions expression expressions retrievalsandstatements retrievalorstatement retrieval statement labelstatement selectionstatement iterationstatement jumpstatement forinit forcondition forincrement expressionstatements expression_unary expression_logicalunary expression_multiply expression_add expression_relational expression_conditionaland expression_conditionalor expression_equality expression_conditional type_valued
%type <type>  type_returnable type_arrayablereturn type_assignable type_common type_lambda type_commonorlambda type_provided type_ctor
%type <type_array> assignabletypes commonorlambdatypes
%start file
%%

file:
	imports classes																{ parsetree = MakeTwoBranchNode(NT_PROGRAM, $1, $2); }
	| classes																	{ parsetree = MakeOneBranchNode(NT_PROGRAM, $1); }
	| imports																	{ parsetree = MakeOneBranchNode(NT_PROGRAM, $1); }
	;

imports:
	imports import																{ $$ = $1; AddSubNode($$, $2); }
	| import																	{ $$ = MakeOneBranchNode(NT_IMPORTSET, $1); }
	;

import:
	IMPORT importtarget STRING ';'												{ $$ = MakeTwoBranchNode(NT_IMPORT, $2, MakeNodeFromString(NT_IMPORTPATH, $3)); }
	;

importtarget:
	/* empty */																	{ $$ = MakeEmptyNode(NT_EMPTY); }
	| '(' IDENTIFIER ')'														{ $$ = MakeNodeFromString(NT_IMPORTTARGET, $2); }
	;

classes:
	classes class																{ $$ = $1; AddSubNode($$, $2); }
	| class																		{ $$ = MakeOneBranchNode(NT_CLASSSET, $1); }
	;

class:
	EVERY IDENTIFIER parentage IS ':'											{ $$ = MakeTwoBranchNode(NT_CLASS, MakeNodeFromString(NT_CLASSNAME, $2), $3); }
	| EVERY IDENTIFIER parentage IS ':' classbody								{ $$ = MakeTwoBranchNode(NT_CLASS, MakeNodeFromString(NT_CLASSNAME, $2), $3); AddSubNode($$, $6); }
	;

parentage:
	/* empty */																	{ $$ = MakeEmptyNode(NT_EMPTY); }
	| '(' inheritances ')'														{ $$ = $2; }
	;

inheritances:
	inheritances ',' inheritance												{ $$ = $1; AddSubNode($$, $3); }
	| inheritance																{ $$ = MakeOneBranchNode(NT_INHERITANCESET, $1); }
	;

inheritance:
	CAPABLE IDENTIFIER															{ $$ = MakeNodeFromString(NT_INTERFACE, $2); }
	| A_OR_AN IDENTIFIER														{ $$ = MakeNodeFromString(NT_SUBCLASS, $2); }
	;

classbody:
	classprop classbody															{ $$ = $2; AddSubNode($$, $1); }
	| classprop																	{ $$ = MakeOneBranchNode(NT_CLASSBODY, $1); }
	;

classprop:
	PROVIDES injection_bindings ';'												{ $$ = $2; }
	| ctor																		{ $$ = $1; }
	| method																	{ $$ = $1; }
	/*| property*/
	;

injection_bindings:
	injection_bindings ',' injection_binding									{ $$ = $1; AddSubNode($$, $3); }
	| injection_binding															{ $$ = MakeOneBranchNode(NT_PROVISIONS, $1); }
	;

injection_binding:
	type_provided																{ $$ = MakeOneBranchNode(NT_PROVISION, MakeNodeFromType($1)); }
	| type_provided SYM_PROVIDE injection_providable							{ $$ = MakeTwoBranchNode(NT_PROVISION, MakeNodeFromType($1), $3); }
	;

injection_providable:
	type_provided																{ $$ = MakeOneBranchNode(NT_PROVIDING, MakeNodeFromType($1)); }
	| injection_ctor															{ $$ = $1; }
	| block																		{ $$ = $1; }
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1); }
	| TRUTH																		{ $$ = MakeNodeFromNumber(NT_TRUTHLIT, $1); }
	;

injection_ctor:
	IDENTIFIER '(' injection_ctorargs ')'										{ $$ = MakeTwoBranchNode(NT_INJECTED_CTOR, MakeNodeFromString(NT_CLASSNAME, $1), $3); }
	;

injection_ctorargs:
	injection_ctorargs ',' injection_ctorarg									{ $$ = $1; AddSubNode($$, $3); }
	| injection_ctorarg															{ $$ = MakeOneBranchNode(NT_INJECTED_CTOR_ARG, $1); }
	;

injection_ctorarg:
	type_provided																{ $$ = MakeNodeFromType($1); }
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1); }
	| TRUTH																		{ $$ = MakeNodeFromNumber(NT_TRUTHLIT, $1); }
	| SYM_CURRIER																{ $$ = MakeEmptyNode(NT_CURRIED); }
	;

ctor:
	NEEDS ctorargs ';'															{ $$ = MakeOneBranchNode(NT_CTOR, $2); }
	| NEEDS ctorargs THEN block													{ $$ = MakeTwoBranchNode(NT_CTOR, $2, $4);}
	;

ctorargs:
	ctorargs ',' type_ctor														{ $$ = $1; AddSubNode($$, MakeNodeFromType($3)); }
	| type_ctor																	{ $$ = MakeOneBranchNode(NT_CTOR_ARGS, MakeNodeFromType($1)); }
	;

method:
	methodreturn methodnamesegments methodbody									{ $$ = MakeTwoBranchNode(NT_METHOD_DECLARATION, $1, $2); AddSubNode($$, $3); }
	| methodnamesegments methodbody												{ $$ = MakeTwoBranchNode(NT_METHOD_DECLARATION, $1, $2); }
	;

methodreturn:
	type_returnable SYM_RETURN_DECREMENT										{ $$ = MakeOneBranchNode(NT_METHOD_RETURN_TYPE, MakeNodeFromType($1)); }
	;

type_returnable:
	type_arrayablereturn SYM_ARRAYED											{ $$ = $1; $1->arrayed = $2; }
	| type_arrayablereturn														{ $$ = $1; }
	;

type_arrayablereturn:
	IDENTIFIER																	{ $$ = MakeType(TYPE_CLASS); $$->typedata._class.classname = $1; }
	| FN '(' ')'																{ $$ = MakeType(TYPE_LAMBDA); }
	| FN '(' commonorlambdatypes ')'											{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.arguments = $3; }
	| type_returnable SYM_RETURN_DECREMENT FN '(' commonorlambdatypes ')'		{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.returntype = $1; $$->typedata.lambda.arguments = $5; }
	| type_returnable SYM_RETURN_DECREMENT FN '(' ')'							{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.returntype = $1; }
	;

methodbody:
	';'																			{ $$ = MakeEmptyNode(NT_ABSTRACT_METHOD); }
	| block																		{ $$ = $1; }
	;

methodnamesegments:
	IDENTIFIER '(' ')'															{ $$ = MakeOneBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1)); }
	| IDENTIFIER '(' assignabletypes ')'										{ $$ = MakeTwoBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1), MakeNodeFromTypeArray($3)); }
	| IDENTIFIER '(' assignabletypes ')' methodnamesegments						{ $$ = $5; PrependSubNode($5, MakeNodeFromTypeArray($3)); PrependSubNode($5, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1)); }
	;

methodcallsegments:
	IDENTIFIER '(' ')'															{ $$ = MakeOneBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1)); }
	| IDENTIFIER '(' curryableexpressions ')'										{ $$ = MakeTwoBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1), $3); }
	| IDENTIFIER '(' curryableexpressions ')' methodcallsegments						{ $$ = $5; PrependSubNode($5, $3); PrependSubNode($5, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1)); }
	;

assignabletypes:
	assignabletypes ',' type_assignable											{ $$ = $1; AddTypeToTypeArray($3, $1); }
	| type_assignable															{ $$ = MakeTypeArray(); AddTypeToTypeArray($1, $$); }
	;

type_common:
	IDENTIFIER SYM_ARRAYED														{ $$ = MakeType(TYPE_CLASS); $$->typedata._class.classname = $1; $$->arrayed = $2; }
	| IDENTIFIER																{ $$ = MakeType(TYPE_CLASS); $$->typedata._class.classname = $1; }
	;

type_valued:
	type_common																	{ $$ = MakeNodeFromType($1); }
	| SYM_SHADOW type_common													{ $$ = MakeNodeFromType($2); $2->typedata._class.shadow = $1; }
	| ALIAS																		{ $$ = MakeNodeFromString(NT_ALIAS, $1); }
	;

type_assignable:
	type_common																	{ $$ = $1; }
	| SYM_SHADOW type_common													{ $$ = $2; $$->typedata._class.shadow = $1; }
	| ALIAS type_common															{ $$ = $2; $$->alias = $1; }
	| type_common ALIAS															{ $$ = $1; $$->alias = $2; }
	| ALIAS type_lambda															{ $$ = $2; $$->alias = $1; }
	| type_lambda ALIAS															{ $$ = $1; $$->alias = $2; }
	;

type_lambda:
	FN '(' ')'																	{ $$ = MakeType(TYPE_LAMBDA); }
	| FN '(' commonorlambdatypes ')'											{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.arguments = $3; }
	| type_commonorlambda SYM_RETURN_DECREMENT FN '(' commonorlambdatypes ')'	{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.returntype = $1; $$->typedata.lambda.arguments = $5; }
	| type_commonorlambda SYM_RETURN_DECREMENT FN '(' ')'						{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.returntype = $1; }
	;

commonorlambdatypes:
	commonorlambdatypes ',' type_commonorlambda									{ $$ = $1; AddTypeToTypeArray($3, $1); }
	| type_commonorlambda														{ $$ = MakeTypeArray(); AddTypeToTypeArray($1, $$); }
	;

type_commonorlambda:
	type_common																	{ $$ = $1; }
	| type_lambda																{ $$ = $1; }
	;

type_provided:
	type_common																	{ $$ = $1; }
	| type_common SPECIALTY														{ $$ = $1; $$->specialty = $2; }
	| type_lambda																{ $$ = $1; }
	| type_lambda SPECIALTY														{ $$ = $1; $$->specialty = $2; }
	;

type_ctor:
	type_assignable																{ $$ = $1; }
	| type_common SPECIALTY														{ $$ = $1; $$->specialty = $2; }
	| SYM_SHADOW type_common SPECIALTY											{ $$ = $2; $$->typedata._class.shadow = $1; $$->specialty = $3;}
	| ALIAS type_common SPECIALTY												{ $$ = $2; $$->alias = $1; $$->specialty = $3; }
	| type_common SPECIALTY ALIAS												{ $$ = $1; $$->alias = $3; $$->specialty = $2; }
	| ALIAS type_lambda SPECIALTY												{ $$ = $2; $$->alias = $1; $$->specialty = $3; }
	| type_lambda SPECIALTY ALIAS												{ $$ = $1; $$->alias = $3; $$->specialty = $2; }
	;

curryableexpressions:
	expression																	{ $$ = MakeOneBranchNode(NT_VALUES, $1); }
	| SYM_CURRIER																{ $$ = MakeOneBranchNode(NT_VALUES, MakeEmptyNode(NT_CURRIED)); }
	| curryableexpressions ',' expression										{ $$ = $1; AddSubNode($1, $3); }
	| curryableexpressions ',' SYM_CURRIER										{ $$ = $1; AddSubNode($1, MakeEmptyNode(NT_CURRIED)); }
	;

value:
	type_valued																	{ $$ = $1; }
	| value '[' expression ']'													{ $$ = MakeTwoBranchNode(NT_ARRAY_ACCESS, $1, $3); }
	| value '.' type_valued														{ $$ = MakeTwoBranchNode(NT_MEMBER_ACCESS, $1, $3); }
	| methodcallsegments														{ $$ = $1; }
	| value '.' methodcallsegments												{ $$ = MakeTwoBranchNode(NT_METHOD_INVOCATION, $1, $3); }
	| value '(' ')'																{ $$ = MakeOneBranchNode(NT_LAMBDA_INVOCATION, $1); }
	| value '(' curryableexpressions ')'										{ $$ = MakeTwoBranchNode(NT_LAMBDA_INVOCATION, $1, $3); }
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1); }
	| TRUTH																		{ $$ = MakeNodeFromNumber(NT_TRUTHLIT, $1); }
	| '(' expression ')'														{ $$ = $2; }
	| '[' expressions ']'														{ $$ = MakeOneBranchNode(NT_ARRAY_DECLARATION, $2); }
	| '[' ']'																	{ $$ = MakeEmptyNode(NT_ARRAY_DECLARATION); }
	;

block:
	'{' retrievalsandstatements '}'												{ $$ = $2; }
	| '{' '}'																	{ $$ = MakeEmptyNode(NT_EMPTY); }
	;

retrievalsandstatements:
	retrievalorstatement														{ $$ = MakeOneBranchNode(NT_RETRIEVALS_STATEMENTS, $1); }
	| retrievalsandstatements retrievalorstatement								{ $$ = $1; AddSubNode($$, $2); }
	;

retrievalorstatement:
	retrieval																	{ $$ = $1; }
	| statement																	{ $$ = $1; }
	;

retrieval:
	type_assignable SYM_PROVIDE value ';'										{ $$ = MakeTwoBranchNode(NT_RETRIEVAL, MakeNodeFromType($1), $3); }
	;

statement:
	emptystatement																{ $$ = MakeEmptyNode(NT_EMPTY); }
	| labelstatement															{ $$ = $1; }
	| expression ';'															{ $$ = $1; }
	| selectionstatement														{ $$ = $1; }
	| iterationstatement														{ $$ = $1; }
	| jumpstatement																{ $$ = $1; }
	| block																		{ $$ = $1; }
	;

emptystatement:
	';'
	;

labelstatement:
	CASE expression ':'															{ $$ = MakeOneBranchNode(NT_CASE, $2); }
	| DEFAULT ':'																{ $$ = MakeEmptyNode(NT_DEFAULTCASE); }
	;

selectionstatement:
	IF '(' expression ')' statement												{ $$ = MakeTwoBranchNode(NT_IF_ELSE, $3, $5); }
	| IF '(' expression ')' statement ELSE statement							{ $$ = MakeTwoBranchNode(NT_IF_ELSE, $3, $5); AddSubNode($$, $7); }
	| SWITCH '(' expression ')' block											{ $$ = MakeTwoBranchNode(NT_SWITCH, $3, $5); }
	;

iterationstatement:
	WHILE '(' expression ')' statement											{ $$ = MakeTwoBranchNode(NT_WHILE, $3, $5); }
	/*| DO statement WHILE '(' expression ')' ';' { printf("| DO statement WHILE '(' expression ')' ';'\n"); }*/
	| FOR '(' forinit forcondition forincrement ')' statement					{ $$ = MakeTwoBranchNode(NT_FOR, $3, $4); AddSubNode($$, $5); AddSubNode($$, $7); }
	/*| FOR '(' forinit forcondition ')' statement */
	;

forinit:
	retrieval																	{ $$ = $1; }
	| ';'																		{ $$ = MakeEmptyNode(NT_EMPTY); }
	;

forcondition:
	expression ';'																{ $$ = $1; }
	| ';'																		{ $$ = MakeEmptyNode(NT_EMPTY); }
	;

forincrement:
	expressionstatements														{ $$ = $1; }
	;

expressionstatements:
	expression																	{ $$ = MakeOneBranchNode(NT_EXPRESSIONS, $1); }
	| expressionstatements ',' expression										{ $$ = $1; AddSubNode($$, $3); }
	;

jumpstatement:
	BREAK ';'																	{ $$ = MakeEmptyNode(NT_BREAK); }
	| CONTINUE ';'																{ $$ = MakeEmptyNode(NT_CONTINUE); }
	| RETURN expression ';'														{ $$ = MakeOneBranchNode(NT_RETURN, $2); }
	| RETURN ';'																{ $$ = MakeEmptyNode(NT_RETURN); }
	;

specialname:
	THIS { printf("THIS\n"); }
	| PARENT { printf("| PARENT\n"); }
	;

expression_unary:
	SYM_INCREMENT expression_unary												{ $$ = MakeOneBranchNode(NT_INCREMENT, $2); }
	| SYM_RETURN_DECREMENT expression_unary										{ $$ = MakeOneBranchNode(NT_DECREMENT, $2); }
	| expression_logicalunary													{ $$ = $1; }
	;

expression_logicalunary:
	value																		{ $$ = $1; }
	/*| '~' expression_unary*/
	| '!' value																	{ $$ = MakeOneBranchNode(NT_INVERT, $2); }
	;

expression_multiply:
	expression_unary															{ $$ = $1; }
	| expression_multiply '*' expression_unary									{ $$ = MakeTwoBranchNode(NT_MULTIPLY, $1, $3); }
	| expression_multiply '/' expression_unary									{ $$ = MakeTwoBranchNode(NT_DIVIDE, $1, $3); }
	;

expression_add:
	expression_multiply															{ $$ = $1; }
	| expression_add '+' expression_multiply									{ $$ = MakeTwoBranchNode(NT_ADD, $1, $3); }
	| expression_add '-' expression_multiply									{ $$ = MakeTwoBranchNode(NT_SUBTRACT, $1, $3); }
	;

expression_relational:
	expression_add																{ $$ = $1; }
	| expression_relational '<' expression_add									{ $$ = MakeTwoBranchNode(NT_LESSTHAN, $1, $3); }
	| expression_relational '>' expression_add									{ $$ = MakeTwoBranchNode(NT_GREATERTHAN, $1, $3); }
	| expression_relational SYM_LE expression_add								{ $$ = MakeTwoBranchNode(NT_LESSTHANEQUAL, $1, $3); }
	| expression_relational SYM_GE expression_add								{ $$ = MakeTwoBranchNode(NT_GREATERTHANEQUAL, $1, $3); }
	;

expression_equality:
	expression_relational														{ $$ = $1; }
	| expression_equality SYM_EQ expression_relational							{ $$ = MakeTwoBranchNode(NT_EQUALITY, $1, $3); }
	| expression_equality SYM_NE expression_relational							{ $$ = MakeTwoBranchNode(NT_INEQUALITY, $1, $3); }
	;

expression_conditionaland:
	expression_equality															{ $$ = $1; }
	| expression_conditionaland SYM_AND expression_equality						{ $$ = MakeTwoBranchNode(NT_AND, $1, $3); }
	;

expression_conditionalor:
	expression_conditionaland													{ $$ = $1; }
	| expression_conditionalor SYM_OR expression_conditionaland					{ $$ = MakeTwoBranchNode(NT_OR, $1, $3); }
	;

expression_conditional:
	expression_conditionalor													{ $$ = $1; }
	| expression_conditionalor '?' expression ':' expression_conditional		{ $$ = MakeTwoBranchNode(NT_IF_THEN_ELSE, $1, $3); AddSubNode($$, $5); }
	;

expression:
	expression_conditional														{ $$ = $1; }
	| type_assignable '=' expression											{ $$ = MakeTwoBranchNode(NT_ASSIGNMENT, MakeNodeFromType($1), $3); }
	| ALIAS '=' expression														{ $$ = MakeTwoBranchNode(NT_ASSIGNMENT, MakeNodeFromString(NT_ALIAS, $1), $3); }
	;

expressions:
	expression																	{ $$ = MakeOneBranchNode(NT_EXPRESSIONS, $1); }
	| expression ',' expressions												{ $$ = $3; AddSubNode($$, $1); }
	;

%%
