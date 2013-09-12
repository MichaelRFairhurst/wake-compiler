%{
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "tree.h"



int line = 1;
int column = 1;
extern char* yytext;
extern FILE *yyin;

void yyerror(const char *str)
{
        fprintf(stderr,"error on line %d col %d : %s on input %s \n", line, column, str, yytext);

}

int yywrap()
{
        return 1;
}



main(int argc, char** argv)
{
#if YYDEBUG
		yydebug = 1;
#endif
	if(argc < 2) {
		printf("no file provided\n"); exit(1);
	}
	// open a file handle to a particular file:
	FILE *myfile = fopen(argv[1], "r");
	// make sure it is valid:
	if (!myfile) {
		printf("couldn't open file");
		exit(1);
	}
	// set lex to read from it instead of defaulting to STDIN:
	yyin = myfile;

	// parse through the input until there is no more:
	yyparse();
}


%}

/* keywords */
%token EVERY CAPABLE A_OR_AN IS RETURN FOREACH WITH PUBLIC IF ELSE WHILE IN IMPORT PROVIDES NEEDS THEN NOTHING SWITCH CASE DEFAULT BREAK FOR DO CONTINUE THIS PARENT FN
/* symbols */
%token SYM_CURRIER SYM_LE SYM_PROVIDE SYM_RETURN_DECREMENT SYM_AND SYM_OR SYM_EQ SYM_NE SYM_GE SYM_INCREMENT

%union
{
        int number;
        char *string;
        Node *node;
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
%type <node> imports import import importtarget classes class parentage inheritances inheritance classbody classprop type_provided injection_providable injection_ctor injection_ctorargs injection_binding injection_bindings injection_ctorarg ctor ctorargs expression value
%start file
%%

file:
	imports classes										{ printtree(MakeTwoBranchNode(NT_PROGRAM, $1, $2), 1); }
	| classes											{ printtree($1, 1); }
	| imports											{ printtree($1, 1); }
	;

imports:
	import imports										{ $$ = $2; AddSubNode($2, $1); }
	| import											{ $$ = MakeOneBranchNode(NT_IMPORTSET, $1); }
	;

import:
	IMPORT importtarget STRING ';'						{ $$ = MakeTwoBranchNode(NT_IMPORT, $2, MakeNodeFromString($3)); }
	;

importtarget:
	/* empty */											{ $$ = MakeNodeFromString("*"); }
	| '(' IDENTIFIER ')'								{ $$ = MakeNodeFromString($2); }
	;

classes:
	class classes										{ $$ = $2; AddSubNode($2, $1); }
	| class												{ $$ = MakeOneBranchNode(NT_CLASSSET, $1); }
	;

class:
	EVERY IDENTIFIER parentage IS ':'					{ $$ = MakeTwoBranchNode(NT_CLASS, MakeNodeFromString($2), $3); }
	| EVERY IDENTIFIER parentage IS ':' classbody		{ $$ = MakeTwoBranchNode(NT_CLASS, MakeNodeFromString($2), $3); AddSubNode($$, $6); }
	;

parentage:
	/* empty */											{ $$ = MakeNodeFromString("no inheritance"); }
	| '(' inheritances ')'								{ $$ = $2; }
	;

inheritances:
	inheritance ',' inheritances						{ $$ = $3; AddSubNode($3, $1); }
	| inheritance										{ $$ = MakeOneBranchNode(NT_INHERITANCESET, $1); }
	;

inheritance:
	CAPABLE IDENTIFIER									{ $$ = MakeTwoBranchNode(NT_INHERITANCE, MakeNodeFromString("interface"), MakeNodeFromString($2)); }
	| A_OR_AN IDENTIFIER								{ $$ = MakeTwoBranchNode(NT_INHERITANCE, MakeNodeFromString("subclass"), MakeNodeFromString($2)); }
	;

classbody:
	classprop classbody									{ $$ = $2; AddSubNode($2, $1); }
	| classprop											{ $$ = MakeOneBranchNode(NT_CLASSBODY, $1); }
	;

classprop:
	PROVIDES injection_bindings ';'						{ $$ = $2; }
	| ctor												{ $$ = $1; }
	| method											{ $$ = 0; }
	/*| property*/
	;

injection_bindings:
	injection_binding ',' injection_bindings			{ $$ = $3; AddSubNode($3, $1); }
	| injection_binding									{ $$ = MakeOneBranchNode(NT_PROVISIONS, $1); }
	;

injection_binding:
	type_provided										{ $$ = MakeOneBranchNode(NT_PROVISION, $1); }
	| type_provided SYM_PROVIDE injection_providable	{ $$ = MakeTwoBranchNode(NT_PROVISION, $1, $3); }
	;

injection_providable:
	type_provided										{ $$ = MakeOneBranchNode(NT_PROVIDING, $1); }
	| injection_ctor									{ $$ = $1; }
	| block												{ $$ = MakeNodeFromString("block"); }
	| STRING											{ $$ = MakeOneBranchNode(NT_PROVIDING, MakeNodeFromString($1)); }
	| NUMBER											{ $$ = MakeOneBranchNode(NT_PROVIDING, MakeNodeFromNumber($1)); }
	| TRUTH												{ $$ = MakeOneBranchNode(NT_PROVIDING, MakeNodeFromNumber($1)); }
	;

injection_ctor:
	IDENTIFIER '(' injection_ctorargs ')'				{ $$ = MakeTwoBranchNode(NT_INJECTED_CTOR, MakeNodeFromString($1), $3); }
	;

injection_ctorargs:
	injection_ctorarg ',' injection_ctorargs			{ $$ = $3; AddSubNode($3, $1); }
	| injection_ctorarg									{ $$ = MakeOneBranchNode(NT_INJECTED_CTOR_ARG, $1); }
	;

injection_ctorarg:
	type_provided										{ $$ = $1; }
	| STRING											{ $$ = MakeNodeFromString($1); }
	| NUMBER											{ $$ = MakeNodeFromNumber($1); }
	| SYM_CURRIER										{ $$ = MakeNodeFromString("???"); }
	;

ctor:
	NEEDS ctorargs ';'									{ $$ = MakeOneBranchNode(NT_CTOR, $2); }
	| NEEDS ctorargs THEN block							{ $$ = MakeTwoBranchNode(NT_CTOR, $2, MakeNodeFromString("block"));}
	;

ctorargs:
	type_ctor ',' ctorargs
	| type_ctor
	;

method:
	methodreturn methodnamesegments methodbody
	| methodnamesegments methodbody
	;

methodreturn:
	type_returnable SYM_RETURN_DECREMENT
	;

type_returnable:
	type_arrayablereturn SYM_ARRAYED
	| type_arrayablereturn
	;

type_arrayablereturn:
	IDENTIFIER
	| FN '(' ')'
	| FN '(' commonorlambdatypes ')'
	| type_returnable SYM_RETURN_DECREMENT FN '(' commonorlambdatypes ')'
	| type_returnable SYM_RETURN_DECREMENT FN '(' ')'
	;

methodbody:
	';'
	| block
	;

methodnamesegments:
	IDENTIFIER '(' ')'
	| IDENTIFIER '(' assignabletypes ')'
	| IDENTIFIER '(' assignabletypes ')' methodnamesegments
	;

methodcallsegments:
	IDENTIFIER '(' ')'
	| IDENTIFIER '(' values ')'
	| IDENTIFIER '(' values ')' methodcallsegments
	;

assignabletypes:
	type_assignable
	| type_assignable ',' assignabletypes
	;

type_common:
	IDENTIFIER SYM_ARRAYED
	/* type_common SYM_ARRAYED isnt necessary; the lexer sends mytype[][][][] as IDENTIFIER(mytpe) SYM_ARRRAYED(4) */
	| IDENTIFIER
	;

type_valued:
	type_common
	| SYM_SHADOW type_common
	| ALIAS
	;

type_assignable:
	type_common
	| SYM_SHADOW type_common
	| ALIAS type_common
	| type_common ALIAS
	| ALIAS type_lambda
	| type_lambda ALIAS
	;

type_lambda:
	FN '(' ')'
	| FN '(' commonorlambdatypes ')'
	| type_commonorlambda SYM_RETURN_DECREMENT FN '(' commonorlambdatypes ')'
	| type_commonorlambda SYM_RETURN_DECREMENT FN '(' ')'
	;

commonorlambdatypes:
	type_commonorlambda ',' commonorlambdatypes
	| type_commonorlambda
	;

type_commonorlambda:
	type_common
	| type_lambda
	;

type_provided:
	type_common
	| type_common SPECIALTY
	| type_lambda
	| type_lambda SPECIALTY
	;

type_ctor:
	type_assignable
	| type_common SPECIALTY
	| SYM_SHADOW type_common SPECIALTY
	| ALIAS type_common SPECIALTY
	| type_common SPECIALTY ALIAS
	| ALIAS type_lambda SPECIALTY
	| type_lambda SPECIALTY ALIAS
	;

values:
	value
	| value values
	;

value:
	type_valued
	| value '[' expression ']'
	| value '.' IDENTIFIER
	| value '.' methodcallsegments
	| STRING
	| NUMBER
	| '(' expression ')'
	;

block:
	'{' retrievalsandstatements '}' { printf("'{' retrievalsandstatements '}'\n"); }
	| '{' '}' { printf("| '{' '}'\n"); }
	;

retrievalsandstatements:
	retrievalorstatement { printf("retrievalorstatement\n"); }
	| retrievalsandstatements retrievalorstatement { printf("| retrievalsandstatements retrievalorstatement\n"); }
	;

retrievalorstatement:
	retrieval { printf("retrieval\n"); }
	| statement { printf("| statement\n"); }
	;

retrieval:
	type_assignable SYM_PROVIDE type_valued ';' { printf("type SYM_PROVIDE IDENTIFIER ';'\n"); }
	;

statement:
	emptystatement { printf("emptystatement\n"); }
	| labelstatement { printf("| labelstatement\n"); }
	| expression ';' { printf("| expression';'\n"); }
	| selectionstatement { printf("| selectionstatement\n"); }
	| iterationstatement { printf("| iterationstatement\n"); }
	| jumpstatement { printf("| jumpstatement\n"); }
	| block { printf("| block\n"); }
	;

emptystatement:
	';' { printf("';'\n"); }
	;

labelstatement:
	IDENTIFIER ':' { printf("IDENTIFIER ':'\n"); }
	| CASE expression ':' { printf("| CASE expression ':'\n"); }
	| DEFAULT ':' { printf("| DEFAULT ':'\n"); }
	;

selectionstatement:
	IF '(' expression ')' statement { printf("IF '(' expression ')' statement\n"); }
	| IF '(' expression ')' statement ELSE statement { printf("| IF '(' expression ')' statement ELSE statement\n"); }
	| SWITCH '(' expression ')' block { printf("| SWITCH '(' expression ')' block\n"); }
	;

iterationstatement:
	WHILE '(' expression ')' statement { printf("WHILE '(' expression ')' statement\n"); }
	| DO statement WHILE '(' expression ')' ';' { printf("| DO statement WHILE '(' expression ')' ';'\n"); }
	| FOR '(' forinit forcondition forincrement ')' statement { printf("| FOR '(' forinit forcondition forincrement ')' statement\n"); }
	| FOR '(' forinit forcondition ')' statement { printf("| FOR '(' forinit forcondition ')' statement\n"); }
	;

forinit:
	retrieval { printf("retrieval\n"); }
	| ';' { printf("| ';'\n"); }
	;

forcondition:
	expression ';' { printf("expression ';'\n"); }
	| ';' { printf("| ';'\n"); }
	;

forincrement:
	expressionstatements { printf("expressionstatements\n"); }
	;

expressionstatements:
	expression { printf("expression\n"); }
	| expressionstatements ',' expression { printf("| expressionstatements ',' expression\n"); }
	;

jumpstatement:
	BREAK ';' { printf("BREAK ';'\n"); }
	| CONTINUE ';' { printf("| CONTINUE ';'\n"); }
	| RETURN expression ';' { printf("| RETURN expression ';'\n"); }
	| RETURN ';' { printf("| RETURN ';'\n"); }
	;

specialname:
	THIS { printf("THIS\n"); }
	| PARENT { printf("| PARENT\n"); }
	;

expression_unary:
	SYM_INCREMENT expression_unary { printf("SYM_INCREMENT expression_unary\n"); }
	| SYM_RETURN_DECREMENT expression_unary { printf("| SYM_RETURN_DECREMENT expression_unary\n"); }
	| arithmeticunaryoperator value { printf("| arithmeticunaryoperator value\n"); }
	| expression_logicalunary { printf("| expression_logicalunary\n"); }
	;

expression_logicalunary:
	logicalunaryoperator expression_unary { printf("| logicalunaryoperator expression_unary\n"); }
	;

logicalunaryoperator:
	'~' { printf("'~'\n"); }
	| '!' { printf("| '!'\n"); }
	;

arithmeticunaryoperator:
	'+' { printf("'+'\n"); }
	| '-' { printf("| '-'\n"); }
	;

expression_multiply:
	value { printf("value\n"); }
	| expression_multiply '*' value { printf("| expression_multiply '*' value\n"); }
	| expression_multiply '/' value { printf("| expression_multiply '/' value\n"); }
	;

expression_add:
	expression_multiply { printf("expression_multiply\n"); }
	| expression_add '+' expression_multiply { printf("| expression_add '+' expression_multiply\n"); }
	| expression_add '-' expression_multiply { printf("| expression_add '-' expression_multiply\n"); }
	;

expression_relational:
	expression_add { printf("expression_add\n"); }
	| expression_relational '<' expression_add { printf("| expression_relational '<' expression_add\n"); }
	| expression_relational '>' expression_add { printf("| expression_relational '>' expression_add\n"); }
	| expression_relational SYM_LE expression_add { printf("| expression_relational SYM_LE expression_add\n"); }
	| expression_relational SYM_GE expression_add { printf("| expression_relational SYM_GE expression_add\n"); }
	;

expression_equality:
	expression_relational { printf("expression_relational\n"); }
	| expression_equality SYM_EQ expression_relational { printf("| expression_equality SYM_EQ expression_relational\n"); }
	| expression_equality SYM_NE expression_relational { printf("| expression_equality SYM_NE expression_relational\n"); }
	;

expression_conditionaland:
	expression_equality { printf("expression_equality\n"); }
	| expression_conditionaland SYM_AND expression_equality { printf("| expression_conditionaland SYM_AND expression_equality\n"); }
	;

expression_conditionalor:
	expression_conditionaland { printf("expression_conditionaland\n"); }
	| expression_conditionalor SYM_OR expression_conditionaland { printf("| expression_conditionalor SYM_OR expression_conditionaland\n"); }
	;

expression_conditional:
	expression_conditionalor { printf("expression_conditionalor\n"); }
	| expression_conditionalor '?' expression ':' expression_conditional { printf("| expression_conditionalor '?' expression ':' expression_conditional\n"); }
	;

expression:
	expression_conditional { printf("expression_conditional\n"); }
	| expression_unary '=' expression { printf("| expression_unary '=' expression\n"); }
	;

%%
