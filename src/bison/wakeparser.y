%{
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "node.h"
#include "tree.h"
#include "type.h"

int line = 1;
int column = 1;
extern char* waketext;
Node* parsetree;

void wakeerror(const char *str)
{
        fprintf(stderr, "[ Error                             ]\n");
        fprintf(stderr, "[ On line %d col %d                 ]\n", line, column);
        fprintf(stderr, "[ %s                                ]\n", str);
        fprintf(stderr, "[ Unexpected %s                     ]\n", waketext);
        fprintf(stderr, "[ yeah this spacing aint pretty yet ]\n");
}

int wakewrap()
{
        return 1;
}

%}

/* keywords */
%token EVERY CAPABLE A_OR_AN IS RETURN FOREACH WITH PUBLIC IF ELSE WHILE IN IMPORT PROVIDES NEEDS THEN NOTHING SWITCH CASE DEFAULT BREAK FOR DO CONTINUE THIS PARENT FN CAST PRIVATE EXISTS VAR FOREACH IN
/* symbols */
%token SYM_CURRIER SYM_LE SYM_PROVIDE SYM_RETURN_DECREMENT SYM_AND SYM_OR SYM_EQ SYM_NE SYM_GE SYM_INCREMENT SYM_PLUSEQ SYM_VALEQ SYM_MULTEQ SYM_SUBEQ SYM_DIVEQ SYM_PROVIDE_ARGS_OPEN
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
%token <string> UIDENTIFIER
%token <string> LIDENTIFIER
%token <string> SPECIALTY
%token <string> STRING
%type <string> identifier
%token <number> NUMBER
%token <number> BOOL
%token <number> SYM_SHADOW
%token <number> SYM_ARRAYED
%type <node> imports import importtarget classes class parentage inheritances inheritance classbody classprop injection_providable injection injection_args provision provisions injection_arg ctor ctorargs value method block methodreturn methodnamesegments methodbody methodaccess methodcallsegments curryableexpressions expression expressions declarationsandstatements declarationorstatement declaration statement labelstatement existsstatement selectionstatement iterationstatement jumpstatement forinit forcondition forincrement expressionstatements expression_unary expression_logicalunary expression_multiply expression_add expression_relational expression_conditionaland expression_conditionalor expression_equality expression_conditional member property property_value retrievalargs objectable expression_cast assignment ctorarg expression_retrieval
%type <type> type specializabletype shadowabletype puretype classtype fntype parameterizedtype unboundtypespecification classdeclarationtype
%type <type_array> puretypes types unboundtypespecifications
%start file

%expect 3
%expect-rr 0
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
	IMPORT importtarget identifier ';'											{ $$ = MakeTwoBranchNode(NT_IMPORT, $2, MakeNodeFromString(NT_IMPORTPATH, $3)); }
	;

importtarget:
	/* empty */																	{ $$ = MakeEmptyNode(NT_EMPTY); }
	| '(' identifier ')'														{ $$ = MakeNodeFromString(NT_IMPORTTARGET, $2); }
	;

classes:
	classes class																{ $$ = $1; AddSubNode($$, $2); }
	| class																		{ $$ = MakeOneBranchNode(NT_CLASSSET, $1); }
	;

class:
	EVERY classdeclarationtype parentage IS ':'									{ $$ = MakeTwoBranchNode(NT_CLASS, MakeNodeFromType($2), $3); }
	| EVERY classdeclarationtype parentage IS ':' classbody						{ $$ = MakeTwoBranchNode(NT_CLASS, MakeNodeFromType($2), $3); AddSubNode($$, $6); }
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
	CAPABLE parameterizedtype													{ $$ = MakeOneBranchNode(NT_INTERFACE, MakeNodeFromType($2)); }
	| A_OR_AN parameterizedtype													{ $$ = MakeOneBranchNode(NT_SUBCLASS, MakeNodeFromType($2)); }
	;

classbody:
	classprop classbody															{ $$ = $2; AddSubNode($$, $1); }
	| classprop																	{ $$ = MakeOneBranchNode(NT_CLASSBODY, $1); }
	;

classprop:
	PROVIDES provisions ';'														{ $$ = $2; }
	| ctor																		{ $$ = $1; }
	| method																	{ $$ = $1; }
	| property																	{ $$ = $1; }
	;

property:
	WITH PUBLIC property_value ';'												{ $$ = MakeTwoBranchNode(NT_PROPERTY, $3, MakeEmptyNode(NT_PUBLIC)); }
	| WITH property_value ';'													{ $$ = MakeOneBranchNode(NT_PROPERTY, $2); }
	;

property_value:
	type '=' expression															{ $$ = MakeTwoBranchNode(NT_ASSIGNMENT, MakeNodeFromType($1), $3); }
	| type retrievalargs expression												{	Node* retrieval = MakeTwoBranchNode(NT_RETRIEVAL, MakeNodeFromType($1), $2);
																					AddSubNode(retrieval, $3);
																					$$ = MakeTwoBranchNode(NT_DECLARATION, MakeNodeFromType(copyType($1)), retrieval);		}
	;

provisions:
	provisions ',' provision													{ $$ = $1; AddSubNode($$, $3); }
	| provision																	{ $$ = MakeOneBranchNode(NT_PROVISIONS, $1); }
	;

provision:
	type																		{ $$ = MakeOneBranchNode(NT_PROVISION, MakeNodeFromType($1)); }
	| type SYM_PROVIDE injection_providable										{ $$ = MakeTwoBranchNode(NT_PROVISION, MakeNodeFromType($1), $3); }
	;

injection_providable:
	type																		{ $$ = MakeNodeFromType($1); }
	| injection																	{ $$ = $1; }
	| block																		{ $$ = $1; }
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1); }
	| BOOL																		{ $$ = MakeNodeFromNumber(NT_BOOLLIT, $1); }
	;

injection:
	UIDENTIFIER '(' injection_args ')'											{ $$ = MakeTwoBranchNode(NT_INJECTION, MakeNodeFromString(NT_CLASSNAME, $1), $3); }
	;

injection_args:
	injection_args ',' injection_arg											{ $$ = $1; AddSubNode($$, $3); }
	| injection_arg																{ $$ = MakeOneBranchNode(NT_INJECTION_ARGS, $1); }
	;

injection_arg:
	type																		{ $$ = MakeNodeFromType($1); }
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1); }
	| BOOL																		{ $$ = MakeNodeFromNumber(NT_BOOLLIT, $1); }
	| SYM_CURRIER																{ $$ = MakeEmptyNode(NT_CURRIED); }
	;

ctor:
	NEEDS ctorargs ';'											{ $$ = MakeOneBranchNode(NT_CTOR, $2); }
	| NEEDS ctorargs THEN block								{ $$ = MakeTwoBranchNode(NT_CTOR, $2, $4);}
	| NEEDS NOTHING THEN block													{ $$ = MakeTwoBranchNode(NT_CTOR, MakeEmptyNode(NT_EMPTY), $4);}
	;

ctorargs:
	ctorargs ',' ctorarg						{ $$ = $1; AddSubNode($$, $3); }
	| ctorarg													{ $$ = MakeOneBranchNode(NT_CTOR_ARGS, $1); }
	;

ctorarg:
	type																		{ $$ = MakeTwoBranchNode(NT_CTOR_ARG, MakeNodeFromType($1), MakeEmptyNode(NT_PRIVATE)); }
	| PUBLIC type																{ $$ = MakeTwoBranchNode(NT_CTOR_ARG, MakeNodeFromType($2), MakeEmptyNode(NT_PUBLIC)); }
	;

method:
	methodaccess methodreturn methodnamesegments methodbody						{ $$ = MakeTwoBranchNode(NT_METHOD_DECLARATION, $1, $2); AddSubNode($$, $3); AddSubNode($$, $4); }
	| methodaccess methodnamesegments methodbody								{ $$ = MakeTwoBranchNode(NT_METHOD_DECLARATION, $1, $2); AddSubNode($$, $3); }
	;

methodaccess:
	/*empty*/																	{ $$ = MakeEmptyNode(NT_PUBLIC); }
	| PRIVATE																	{ $$ = MakeEmptyNode(NT_PRIVATE); }
	| PUBLIC																	{ $$ = MakeEmptyNode(NT_PUBLIC); }

methodreturn:
	puretype SYM_RETURN_DECREMENT												{ $$ = MakeOneBranchNode(NT_METHOD_RETURN_TYPE, MakeNodeFromType($1)); }
	;

methodbody:
	';'																			{ $$ = MakeEmptyNode(NT_ABSTRACT_METHOD); }
	| block																		{ $$ = $1; }
	;

methodnamesegments:
	identifier '(' ')'															{ $$ = MakeOneBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1)); }
	| identifier '(' types ')'													{ $$ = MakeTwoBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1), MakeNodeFromTypeArray($3)); }
	| identifier '(' types ')' methodnamesegments								{ $$ = $5; PrependSubNode($5, MakeNodeFromTypeArray($3)); PrependSubNode($5, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1)); }
	;

methodcallsegments:
	identifier '(' ')'															{ $$ = MakeOneBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1)); }
	| identifier '(' curryableexpressions ')'													{ $$ = MakeTwoBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1), $3); }
	| identifier '(' curryableexpressions ')' methodcallsegments								{ $$ = $5; PrependSubNode($5, $3); PrependSubNode($5, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1)); }
	;

member:
	shadowabletype																{ $$ = MakeNodeFromType($1); }
	| LIDENTIFIER																{ $$ = MakeNodeFromString(NT_ALIAS, $1); }
	;

curryableexpressions:
	expression																	{ $$ = MakeOneBranchNode(NT_VALUES, $1); }
	| SYM_CURRIER																{ $$ = MakeOneBranchNode(NT_VALUES, MakeEmptyNode(NT_CURRIED)); }
	| curryableexpressions ',' expression										{ $$ = $1; AddSubNode($1, $3); }
	| curryableexpressions ',' SYM_CURRIER										{ $$ = $1; AddSubNode($1, MakeEmptyNode(NT_CURRIED)); }
	;

value:
	LIDENTIFIER																	{ $$ = MakeNodeFromString(NT_ALIAS, $1); }
	| shadowabletype															{ $$ = MakeNodeFromType($1); }
	| methodcallsegments														{ $$ = MakeTwoBranchNode(NT_METHOD_INVOCATION, MakeEmptyNode(NT_THIS), $1); }
	| THIS																		{ $$ = MakeEmptyNode(NT_THIS); }
	| value '[' expression ']'													{ $$ = MakeTwoBranchNode(NT_ARRAY_ACCESS, $1, $3); }
	| objectable '.' member														{ $$ = MakeTwoBranchNode(NT_MEMBER_ACCESS, $1, $3); }
	| objectable '.' methodcallsegments											{ $$ = MakeTwoBranchNode(NT_METHOD_INVOCATION, $1, $3); }
	/*| value '(' ')'															{ $$ = MakeOneBranchNode(NT_LAMBDA_INVOCATION, $1); }
	| value '(' curryableexpressions ')'										{ $$ = MakeTwoBranchNode(NT_LAMBDA_INVOCATION, $1, $3); }*/
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1); }
	| BOOL																		{ $$ = MakeNodeFromNumber(NT_BOOLLIT, $1); }
	| '(' expression ')'														{ $$ = $2; }
	| '[' expressions ']'														{ $$ = MakeOneBranchNode(NT_ARRAY_DECLARATION, $2); }
	| SYM_ARRAYED																{ $$ = MakeEmptyNode(NT_ARRAY_DECLARATION); }
	| NOTHING																	{ $$ = MakeEmptyNode(NT_NOTHING); }
	;

objectable:
	PARENT																		{ $$ = MakeEmptyNode(NT_PARENT); }
	| value																		{ $$ = $1; }

block:
	'{' declarationsandstatements '}'											{ $$ = MakeOneBranchNode(NT_BLOCK, $2); }
	| '{' '}'																	{ $$ = MakeEmptyNode(NT_EMPTY); }
	;

declarationsandstatements:
	declarationorstatement														{ $$ = MakeOneBranchNode(NT_RETRIEVALS_STATEMENTS, $1); }
	| declarationsandstatements declarationorstatement							{ $$ = $1; AddSubNode($$, $2); }
	;

declarationorstatement:
	declaration																	{ $$ = $1; }
	| statement																	{ $$ = $1; }
	;

declaration:
	VAR type '=' expression ';'													{ $$ = MakeTwoBranchNode(NT_DECLARATION, MakeNodeFromType($2), $4); }
	| VAR type retrievalargs expression ';'										{	Node* retrieval = MakeTwoBranchNode(NT_RETRIEVAL, MakeNodeFromType($2), $3);
																					AddSubNode(retrieval, $4);
																					$$ = MakeTwoBranchNode(NT_DECLARATION, MakeNodeFromType(copyType($2)), retrieval);		}
	;

retrievalargs:
	SYM_PROVIDE																	{ $$ = MakeEmptyNode(NT_EMPTY); }
	| SYM_PROVIDE_ARGS_OPEN expressions ')'										{ $$ = $2; }

statement:
	emptystatement																{ $$ = MakeEmptyNode(NT_EMPTY); }
	| labelstatement															{ $$ = $1; }
	| expression ';'															{ $$ = $1; }
	| selectionstatement														{ $$ = $1; }
	| iterationstatement														{ $$ = $1; }
	| jumpstatement																{ $$ = $1; }
	| existsstatement															{ $$ = $1; }
	| block																		{ $$ = $1; }
	;

emptystatement:
	';'
	;

labelstatement:
	CASE expression ':'															{ $$ = MakeOneBranchNode(NT_CASE, $2); }
	| DEFAULT ':'																{ $$ = MakeEmptyNode(NT_DEFAULTCASE); }
	;

existsstatement:
	IF shadowabletype EXISTS statement											{ $$ = MakeTwoBranchNode(NT_EXISTS, MakeNodeFromType($2), $4); }
	| IF LIDENTIFIER EXISTS statement											{ $$ = MakeTwoBranchNode(NT_EXISTS, MakeNodeFromString(NT_ALIAS, $2), $4); }
	| IF shadowabletype EXISTS statement ELSE statement							{ $$ = MakeTwoBranchNode(NT_EXISTS, MakeNodeFromType($2), $4); AddSubNode($$, $6); }
	| IF LIDENTIFIER EXISTS statement ELSE statement							{ $$ = MakeTwoBranchNode(NT_EXISTS, MakeNodeFromString(NT_ALIAS, $2), $4); AddSubNode($$, $6); }
	;

selectionstatement:
	IF '(' expression ')' statement												{ $$ = MakeTwoBranchNode(NT_IF_ELSE, $3, $5); }
	| IF '(' expression ')' statement ELSE statement							{ $$ = MakeTwoBranchNode(NT_IF_ELSE, $3, $5); AddSubNode($$, $7); }
	| SWITCH '(' expression ')' block											{ $$ = MakeTwoBranchNode(NT_SWITCH, $3, $5); }
	;

iterationstatement:
	WHILE '(' expression ')' statement											{ $$ = MakeTwoBranchNode(NT_WHILE, $3, $5); }
	| FOR '(' forinit forcondition forincrement ')' statement					{ $$ = MakeTwoBranchNode(NT_FOR, $3, $4); AddSubNode($$, $5); AddSubNode($$, $7); }
	| FOREACH '(' expression ')' statement										{ $$ = MakeTwoBranchNode(NT_FOREACH, $3, $5); }
	| FOREACH '(' type IN expression ')' statement								{ $$ = MakeTwoBranchNode(NT_FOREACHIN, $3, $5); AddSubNode($$, $7); }
	;

forinit:
	declaration																	{ $$ = $1; }
	| expression ';'															{ $$ = $1; }
	;

forcondition:
	expression ';'																{ $$ = $1; }
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

expression_unary:
	SYM_INCREMENT expression_unary												{ $$ = MakeOneBranchNode(NT_INCREMENT, $2); }
	| SYM_RETURN_DECREMENT expression_unary										{ $$ = MakeOneBranchNode(NT_DECREMENT, $2); }
	| expression_logicalunary													{ $$ = $1; }
	;

expression_cast:
	value																		{ $$ = $1; }
	| '(' CAST puretype ')' value												{ $$ = MakeTwoBranchNode(NT_CAST, MakeNodeFromType($3), $5); }
	;

expression_logicalunary:
	expression_cast																{ $$ = $1; }
	/*| '~' expression_unary*/
	| '!' expression_cast														{ $$ = MakeOneBranchNode(NT_INVERT, $2); }
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
	| expression_conditionalor IF expression_conditional ELSE expression		{ $$ = MakeTwoBranchNode(NT_IF_THEN_ELSE, $1, $3); AddSubNode($$, $5); }
	;

expression_retrieval:
	expression_conditional														{ $$ = $1; }
	| type retrievalargs expression_conditional									{ $$ = MakeTwoBranchNode(NT_RETRIEVAL, MakeNodeFromType($1), $2); AddSubNode($$, $3); }
	;

assignment:
	value '=' expression														{ $$ = MakeTwoBranchNode(NT_ASSIGNMENT, $1, $3); }
	| value SYM_VALEQ expression												{ $$ = MakeTwoBranchNode(NT_VALUED_ASSIGNMENT, $1, $3); }
	| value SYM_PLUSEQ expression												{ $$ = MakeTwoBranchNode(NT_ADD_ASSIGNMENT, $1, $3); }
	| value SYM_SUBEQ expression												{ $$ = MakeTwoBranchNode(NT_SUB_ASSIGNMENT, $1, $3); }
	| value SYM_MULTEQ expression												{ $$ = MakeTwoBranchNode(NT_MULT_ASSIGNMENT, $1, $3); }
	| value SYM_DIVEQ expression												{ $$ = MakeTwoBranchNode(NT_DIV_ASSIGNMENT, $1, $3); }
	;

expression:
	expression_retrieval														{ $$ = $1; }
	| assignment																{ $$ = $1; }
	;

expressions:
	expression																	{ $$ = MakeOneBranchNode(NT_EXPRESSIONS, $1); }
	| expression ',' expressions												{ $$ = $3; AddSubNode($$, $1); }
	;

identifier:
	UIDENTIFIER																	{ $$ = $1; }
	| LIDENTIFIER																{ $$ = $1; }
	;

type:
	LIDENTIFIER specializabletype												{ $$ = $2; $$->alias = $1; }
	| specializabletype LIDENTIFIER												{ $$ = $1; $$->alias = $2; }
	| specializabletype															{ $$ = $1; }
	;

specializabletype:
	shadowabletype																{ $$ = $1; }
	| shadowabletype SPECIALTY													{ $$ = $1; $$->specialty = $2; }
	;

shadowabletype:
	puretype																	{ $$ = $1; }
	| SYM_SHADOW classtype														{ $$ = $2; $$->typedata._class.shadow = $1; }
	;

puretype:
	classtype																	{ $$ = $1; }
	| fntype																	{ $$ = $1; }
	;

puretypes:
	puretype																	{ $$ = MakeTypeArray(); AddTypeToTypeArray($1, $$); }
	| puretypes ',' puretype													{ $$ = $1; AddTypeToTypeArray($3, $$); }
	;

types:
	type																		{ $$ = MakeTypeArray(); AddTypeToTypeArray($1, $$); }
	| types ',' type															{ $$ = $1; AddTypeToTypeArray($3, $$); }
	;

classtype:
	parameterizedtype															{ $$ = $1; }
	| parameterizedtype '?'														{ $$ = $1; $$->optional = 1; }
	| parameterizedtype SYM_ARRAYED												{ $$ = $1; $$->arrayed += $2; }
	;

parameterizedtype:
	UIDENTIFIER																	{ $$ = MakeType(TYPE_CLASS); $$->typedata._class.classname = $1; }
	| UIDENTIFIER '{' puretypes '}'												{ $$ = MakeType(TYPE_CLASS); $$->typedata._class.classname = $1; $$->typedata._class.parameters = $3; }
	;

fntype:
	puretype SYM_RETURN_DECREMENT FN '(' puretypes ')'							{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.returntype = $1; $$->typedata.lambda.arguments = $5; }
	| puretype SYM_RETURN_DECREMENT FN '(' ')'									{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.returntype = $1; }
	| FN '(' puretypes ')'														{ $$ = MakeType(TYPE_LAMBDA); $$->typedata.lambda.arguments = $3; }
	| FN '(' ')'																{ $$ = MakeType(TYPE_LAMBDA); }
	| fntype '?'																{ $$ = $1; $$->optional = 1; }
	| fntype SYM_ARRAYED														{ $$ = $1; $$->arrayed += $2; }
	;

unboundtypespecifications:
	unboundtypespecification													{ $$ = MakeTypeArray(); AddTypeToTypeArray($1, $$); }
	| unboundtypespecifications ',' unboundtypespecification						{ $$ = $1; AddTypeToTypeArray($3, $1); }
	;

unboundtypespecification:
	UIDENTIFIER																	{ $$ = MakeType(TYPE_PARAMETERIZED); $$->typedata.parameterized.label = $1; }
	/* | UIDENTIFIER from puretype
	   | UIDENTIFIER to puretype
	   | UIDENTIFIER from puretype to puretype */
	;

classdeclarationtype:
	UIDENTIFIER																	{ $$ = MakeType(TYPE_CLASS); $$->typedata._class.classname = $1; }
	| UIDENTIFIER '{' unboundtypespecifications '}'								{ $$ = MakeType(TYPE_CLASS); $$->typedata._class.classname = $1; $$->typedata._class.parameters = $3; }
	;

%%
