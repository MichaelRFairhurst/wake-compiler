%{
/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * wakeparser.y
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <stdio.h>
#include <string.h>
#include "type.h"
#include "node.h"
#include "tree.h"
#include "type.h"
#include "wakelltype.h"
#include "error.h"

#define YYLEX_PARAM &yylval, &yylloc
#define YYERROR_VERBOSE

int line = 1;
int column = 1;
extern char* waketext;
Node* parsetree;

void wakeerror(const char *str)
{
		//error_open_file(YYFILE);
		error_print_error_token(&yylloc);
        puts(str);
}

int wakewrap()
{
        return 1;
}

%}

/* keywords */
%token EVERY CAPABLE A_OR_AN IS RETURN WITH PUBLIC IF ELSE WHILE IMPORT PROVIDES NEEDS THEN NOTHING SWITCH CASE DEFAULT BREAK FOR DO CONTINUE THIS PARENT FN CAST PRIVATE EXISTS VAR FOREACH IN THROW TRY CATCH FROM
/* symbols */
%token SYM_CURRIER SYM_LE SYM_PROVIDE SYM_RETURN_DECREMENT SYM_AND SYM_OR SYM_EQ SYM_NE SYM_GE SYM_INCREMENT SYM_PLUSEQ SYM_VALEQ SYM_MULTEQ SYM_SUBEQ SYM_DIVEQ SYM_PROVIDE_ARGS_OPEN SYM_EARLYBAILOUT_DOT SYM_TYPESAFE_INDEX SYM_LAMBDA;
/* this too */
%token ERRORTOKEN

%locations

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
%token <string> ANNOTATION
%token <string> STRING
%type <string> identifier
%token <number> NUMBER
%token <number> BOOL
%token <number> SYM_SHADOW
%token <void> SYM_ARRAYED
%type <node> imports import importtarget classes annotatedclass class parentage inheritances inheritance classbody classprop providable injection injection_subinjections provision provisions injection_subinjection ctor ctorargs value value_invokable method block methodreturn lmethodnamesegments lumethodnamesegments methodbody methodaccess lumethodcallsegments methodcallsegments curryableexpressions expression expressions declarationsandstatements declarationorstatement declaration statement labelstatement existsstatement selectionstatement iterationstatement jumpstatement forinit forcondition forincrement expressionstatements expression_unary expression_logicalunary expression_multiply expression_add expression_relational expression_conditionaland expression_conditionalor expression_equality expression_conditional member property property_value retrievalargs objectable expression_cast assignment ctorarg expression_retrieval throwstatement trystatement catchstatement expression_noretrieval expressions_noretrieval provision_args annotatedtype annotatedmethod annotation annotations annotationvals annotationval lambda statement_or_block expression_nolambda inferenceabletypes
%type <type> type specializabletype shadowabletype puretype classtype fntype parameterizedtype unboundtypespecification classdeclarationtype
%type <type_array> puretypes types unboundtypespecifications
%start file

/* 2 are from @annotations before class definitions, 1 from if/else, 1 from try/catch, 1 from exists/else */
%expect 5

%expect-rr 0
%%

file:
	imports classes																{ parsetree = MakeTwoBranchNode(NT_PROGRAM, $1, $2, @$); }
	| classes																	{ parsetree = MakeOneBranchNode(NT_PROGRAM, $1, @$); }
	| imports																	{ parsetree = MakeOneBranchNode(NT_PROGRAM, $1, @$); }
	;

imports:
	imports import																{ $$ = $1; AddSubNode($$, $2); }
	| import																	{ $$ = MakeOneBranchNode(NT_IMPORTSET, $1, @$); }
	;

import:
	IMPORT importtarget identifier ';'											{ $$ = MakeTwoBranchNode(NT_IMPORT, $2, MakeNodeFromString(NT_IMPORTPATH, $3, @3), @$); }
	;

importtarget:
	/* empty */																	{ $$ = MakeEmptyNode(NT_EMPTY, @$); }
	| '(' identifier ')'														{ $$ = MakeNodeFromString(NT_IMPORTTARGET, $2, @2); }
	;

classes:
	classes annotatedclass														{ $$ = $1; AddSubNode($$, $2); }
	| annotatedclass															{ $$ = MakeOneBranchNode(NT_CLASSSET, $1, @$); }
	;

annotations:
	annotations annotation														{ $$ = $1; AddSubNode($$, $2); }
	| annotation																{ $$ = MakeOneBranchNode(NT_ANNOTATIONS, $1, @$); }
	;

annotation:
	ANNOTATION																	{ $$ = MakeOneBranchNode(NT_ANNOTATION, MakeNodeFromString(NT_ANNOTATION_NAME, $1, @$), @$); }
	| ANNOTATION '(' annotationvals ')'											{ $$ = MakeTwoBranchNode(NT_ANNOTATION, MakeNodeFromString(NT_ANNOTATION_NAME, $1, @$), $3, @$); }
	;

annotationvals:
	annotationvals ',' annotationval											{ $$ = $1; AddSubNode($$, $3); }
	| annotationval																{ $$ = MakeOneBranchNode(NT_ANNOTATION_VALS, $1, @$); }
	;

annotationval:
	STRING																		{ $$ = MakeNodeFromString(NT_STRINGLIT, $1, @$); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1, @$); }
	| BOOL																		{ $$ = MakeNodeFromNumber(NT_BOOLLIT, $1, @$); }
	| NOTHING																	{ $$ = MakeEmptyNode(NT_NOTHING, @$); }
	;

annotatedclass:
	class																		{ $$ = $1; }
	| annotations class															{ $$ = MakeTwoBranchNode(NT_ANNOTATED_CLASS, $2, $1, @$); }
	;

class:
	EVERY classdeclarationtype parentage IS ':'									{ $$ = MakeTwoBranchNode(NT_CLASS, MakeNodeFromType($2, @$), $3, @$); }
	| EVERY classdeclarationtype parentage IS ':' classbody						{ $$ = MakeTwoBranchNode(NT_CLASS, MakeNodeFromType($2, @$), $3, @$); AddSubNode($$, $6); }
	;

parentage:
	/* empty */																	{ $$ = MakeEmptyNode(NT_EMPTY, @$); }
	| '(' inheritances ')'														{ $$ = $2; }
	;

inheritances:
	inheritances ',' inheritance												{ $$ = $1; AddSubNode($$, $3); }
	| inheritance																{ $$ = MakeOneBranchNode(NT_INHERITANCESET, $1, @$); }
	;

inheritance:
	CAPABLE parameterizedtype													{ $$ = MakeOneBranchNode(NT_INTERFACE, MakeNodeFromType($2, @$), @$); }
	| A_OR_AN parameterizedtype													{ $$ = MakeOneBranchNode(NT_SUBCLASS, MakeNodeFromType($2, @$), @$); }
	;

classbody:
	classprop classbody															{ $$ = $2; AddSubNode($$, $1); }
	| classprop																	{ $$ = MakeOneBranchNode(NT_CLASSBODY, $1, @$); }
	;

classprop:
	PROVIDES provisions ';'														{ $$ = $2; }
	| ctor																		{ $$ = $1; }
	| annotatedmethod															{ $$ = $1; }
	| property																	{ $$ = $1; }
	;

property:
	WITH PUBLIC property_value ';'												{ $$ = MakeTwoBranchNode(NT_PROPERTY, $3, MakeEmptyNode(NT_PUBLIC, @$), @$); }
	| WITH property_value ';'													{ $$ = MakeOneBranchNode(NT_PROPERTY, $2, @$); }
	;

property_value:
	type '=' expression															{ $$ = MakeTwoBranchNode(NT_ASSIGNMENT, MakeNodeFromType($1, @$), $3, @$); }
	| type retrievalargs expression												{	Node* retrieval = MakeTwoBranchNode(NT_RETRIEVAL, MakeNodeFromType($1, @$), $2, @$);
																					AddSubNode(retrieval, $3);
																					$$ = MakeTwoBranchNode(NT_DECLARATION, MakeNodeFromType(copyType($1), @$), retrieval, @$);		}
	;

provisions:
	provisions ',' provision													{ $$ = $1; AddSubNode($$, $3); }
	| provision																	{ $$ = MakeOneBranchNode(NT_PROVISIONS, $1, @$); }
	;

provision:
	type																		{ $$ = MakeOneBranchNode(NT_PROVISION, MakeNodeFromType($1, @$), @$); }
	| type SYM_PROVIDE providable												{ $$ = MakeTwoBranchNode(NT_PROVISION, MakeNodeFromType($1, @$), $3, @$); }
	;

providable:
	type																		{ $$ = MakeNodeFromType($1, @$); }
	| injection																	{ $$ = $1; }
	| provision_args block														{ $$ = $1; AddSubNode($$, $2); }
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1, @$); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1, @$); }
	| BOOL																		{ $$ = MakeNodeFromNumber(NT_BOOLLIT, $1, @$); }
	;

provision_args:
	/* empty */																	{ $$ = MakeOneBranchNode(NT_PROVISION_BEHAVIOR, MakeNodeFromTypeArray(MakeTypeArray(), @$), @$); }
	| '(' types ')'																{ $$ = MakeOneBranchNode(NT_PROVISION_BEHAVIOR, MakeNodeFromTypeArray($2, @$), @$); }
	;

injection:
	UIDENTIFIER '(' injection_subinjections ')'									{ $$ = MakeTwoBranchNode(NT_INJECTION, MakeNodeFromString(NT_CLASSNAME, $1, @$), $3, @$); }
	;

injection_subinjections:
	injection_subinjections ',' injection_subinjection							{ $$ = $1; AddSubNode($$, $3); }
	| injection_subinjection													{ $$ = MakeOneBranchNode(NT_SUBINJECTIONS, $1, @$); }
	;

injection_subinjection:
	type																		{ $$ = MakeNodeFromType($1, @$); }
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1, @$); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1, @$); }
	| BOOL																		{ $$ = MakeNodeFromNumber(NT_BOOLLIT, $1, @$); }
	| '?' type																	{ $$ = MakeOneBranchNode(NT_INJECTION_ARG, MakeNodeFromType($2, @$), @$); }
	;

ctor:
	NEEDS ctorargs ';'															{ $$ = MakeOneBranchNode(NT_CTOR, $2, @$); }
	| NEEDS ctorargs THEN block													{ $$ = MakeTwoBranchNode(NT_CTOR, $2, $4, @$);}
	| NEEDS NOTHING THEN block													{ $$ = MakeTwoBranchNode(NT_CTOR, MakeEmptyNode(NT_EMPTY, @$), $4, @$);}
	;

ctorargs:
	ctorargs ',' ctorarg														{ $$ = $1; AddSubNode($$, $3); }
	| ctorarg																	{ $$ = MakeOneBranchNode(NT_CTOR_ARGS, $1, @$); }
	;

ctorarg:
	annotatedtype																{ $$ = MakeTwoBranchNode(NT_CTOR_ARG, $1, MakeEmptyNode(NT_PRIVATE, @$), @$); }
	| PUBLIC annotatedtype														{ $$ = MakeTwoBranchNode(NT_CTOR_ARG, $2, MakeEmptyNode(NT_PUBLIC, @$), @$); }
	;

annotatedmethod:
	annotations method															{ $$ = MakeTwoBranchNode(NT_ANNOTATED_METHOD, $2, $1, @$); }
	| method																	{ $$ = $1; }
	;

method:
	methodaccess methodreturn lmethodnamesegments methodbody					{ $$ = MakeTwoBranchNode(NT_METHOD_DECLARATION, $1, $2, @$); AddSubNode($$, $3); AddSubNode($$, $4); }
	| methodaccess lmethodnamesegments methodbody								{ $$ = MakeTwoBranchNode(NT_METHOD_DECLARATION, $1, $2, @$); AddSubNode($$, $3); }
	;

methodaccess:
	/*empty*/																	{ $$ = MakeEmptyNode(NT_PUBLIC, @$); }
	| PRIVATE																	{ $$ = MakeEmptyNode(NT_PRIVATE, @$); }
	| PUBLIC																	{ $$ = MakeEmptyNode(NT_PUBLIC, @$); }
	;

methodreturn:
	puretype SYM_RETURN_DECREMENT												{ $$ = MakeOneBranchNode(NT_METHOD_RETURN_TYPE, MakeNodeFromType($1, @$), @$); }
	;

methodbody:
	';'																			{ $$ = MakeEmptyNode(NT_ABSTRACT_METHOD, @$); }
	| block																		{ $$ = $1; }
	;

lmethodnamesegments:
	LIDENTIFIER '(' ')'															{ $$ = MakeOneBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), @$); }
	| LIDENTIFIER '(' types ')'													{ $$ = MakeTwoBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), MakeNodeFromTypeArray($3, @$), @$); }
	| LIDENTIFIER '(' types ')' lumethodnamesegments							{ $$ = $5; PrependSubNode($5, MakeNodeFromTypeArray($3, @$)); PrependSubNode($5, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$)); }
	;

lumethodnamesegments:
	identifier '(' ')'															{ $$ = MakeOneBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), @$); }
	| identifier '(' types ')'													{ $$ = MakeTwoBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), MakeNodeFromTypeArray($3, @$), @$); }
	| identifier '(' types ')' lumethodnamesegments								{ $$ = $5; PrependSubNode($5, MakeNodeFromTypeArray($3, @$)); PrependSubNode($5, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$)); }
	;

methodcallsegments:
	'(' ')'																		{ $$ = NULL; }
	| '(' curryableexpressions ')'												{ $$ = MakeOneBranchNode(NT_INVOCATION_PARTS_TEMP, $2, @$); }
	| '(' curryableexpressions ')' lumethodcallsegments							{ $$ = MakeTwoBranchNode(NT_INVOCATION_PARTS_TEMP, $2, $4, @$); }
	;

lumethodcallsegments:
	identifier '(' ')'															{ $$ = MakeOneBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), @$); }
	| identifier '(' curryableexpressions ')'									{ $$ = MakeTwoBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), $3, @$); }
	| identifier '(' curryableexpressions ')' lumethodcallsegments				{ $$ = $5; PrependSubNode($5, $3); PrependSubNode($5, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$)); }
	;

curryableexpressions:
	expression																	{ $$ = MakeOneBranchNode(NT_VALUES, $1, @$); }
	| SYM_CURRIER																{ $$ = MakeOneBranchNode(NT_VALUES, MakeEmptyNode(NT_CURRIED, @$), @$); }
	| curryableexpressions ',' expression										{ $$ = $1; AddSubNode($1, $3); }
	| curryableexpressions ',' SYM_CURRIER										{ $$ = $1; AddSubNode($1, MakeEmptyNode(NT_CURRIED, @$)); }
	;

value:
	shadowabletype																{ $$ = MakeNodeFromType($1, @$); }
	| value_invokable															{ $$ = $1; }
	| THIS																		{ $$ = MakeEmptyNode(NT_THIS, @$); }
	| STRING																	{ $$ = MakeNodeFromString(NT_STRINGLIT, $1, @$); }
	| NUMBER																	{ $$ = MakeNodeFromNumber(NT_NUMBERLIT, $1, @$); }
	| BOOL																		{ $$ = MakeNodeFromNumber(NT_BOOLLIT, $1, @$); }
	| '[' expressions ']'														{ $$ = $2; $$->loc = @$; $$->node_type = NT_ARRAY_DECLARATION; }
	| SYM_ARRAYED																{ $$ = MakeEmptyNode(NT_ARRAY_DECLARATION, @$); }
	| NOTHING																	{ $$ = MakeEmptyNode(NT_NOTHING, @$); }
	| objectable '.' shadowabletype												{ $$ = MakeTwoBranchNode(NT_MEMBER_ACCESS, $1, MakeNodeFromType($3, @3), @$); }
	| objectable SYM_EARLYBAILOUT_DOT shadowabletype							{ $$ = MakeTwoBranchNode(NT_EARLYBAILOUT_MEMBER_ACCESS, $1, MakeNodeFromType($3, @3), @$); }
	;

value_invokable:
	LIDENTIFIER																	{ $$ = MakeNodeFromString(NT_ALIAS, $1, @$); }
	| value '[' expression ']'													{ $$ = MakeTwoBranchNode(NT_ARRAY_ACCESS, $1, $3, @$); }
	| value SYM_TYPESAFE_INDEX expression ']'									{ $$ = MakeTwoBranchNode(NT_TYPESAFE_ARRAY_ACCESS, $1, $3, @$); }
	| objectable '.' LIDENTIFIER												{ $$ = MakeTwoBranchNode(NT_MEMBER_ACCESS, $1, MakeNodeFromString(NT_ALIAS, $3, @3), @$); }
	| objectable SYM_EARLYBAILOUT_DOT LIDENTIFIER								{ $$ = MakeTwoBranchNode(NT_EARLYBAILOUT_MEMBER_ACCESS, $1, MakeNodeFromString(NT_ALIAS, $3, @3), @$); }
	| '(' expression ')'														{ $$ = $2; }
	| value_invokable methodcallsegments										{
																					if ($1->node_type == NT_ALIAS) {
																						// Turn (NT_ALIAS) and (NT_INVOCATION_PARTS_TEMP (expressions) [NT_METHOD_NAME_SEGMENT (METHOD_NAME) (expressions)])
																						// into (NT_METHOD_INVOCATION (NT_THIS) [NT_METHOD_NAME_SEGMENT (METHOD_NAME) (expressions)])
																						$1->node_type == NT_METHOD_NAME_SEGMENT;
																						$$ = MakeTwoBranchNode(NT_METHOD_INVOCATION, MakeEmptyNode(NT_THIS, @$), MakeOneBranchNode(NT_METHOD_NAME, $1, @$), @$);

																						if($2 != NULL) {
																							AddSubNode($$->node_data.nodes[1], $2->node_data.nodes[0]);
																							if($2->subnodes == 2) {
																								int i = 0;
																								for(; i < $2->node_data.nodes[1]->subnodes; i++) {
																									AddSubNode($$->node_data.nodes[1], $2->node_data.nodes[1]->node_data.nodes[i]);
																								}
																								//free($2->node_data.nodes[1]->node_data.nodes);
																								//free($2->node_data.nodes[1]);
																							}
																							//free($2->node_data.nodes);
																							//free($2);
																						}
																					} else if ($1->node_type == NT_MEMBER_ACCESS || $1->node_type == NT_EARLYBAILOUT_MEMBER_ACCESS && $1->node_data.nodes[1]->node_type == NT_ALIAS) {
																						// Turn (NT_MEMBER_ACCESS (expression) (NT_ALIAS)) and (NT_INVOCATION_PARTS_TEMP (expressions) [NT_METHOD_NAME_SEGMENT (METHOD_NAME) (expressions)])
																						// into (NT_METHOD_INVOCATION (expression) [NT_METHOD_NAME_SEGMENT (METHOD_NAME) (expressions)])
																						$$ = MakeOneBranchNode($1->node_type == NT_MEMBER_ACCESS ? NT_METHOD_INVOCATION : NT_EARLYBAILOUT_METHOD_INVOCATION, $1->node_data.nodes[0], @$);
																						AddSubNode($$, MakeOneBranchNode(NT_METHOD_NAME, MakeNodeFromString(NT_METHOD_NAME_SEGMENT, strdup($1->node_data.nodes[1]->node_data.string), $1->node_data.nodes[1]->loc), @1));

																						if($2 != NULL) {
																							AddSubNode($$->node_data.nodes[1], $2->node_data.nodes[0]);
																							if($2->subnodes == 2) {
																								int i = 0;
																								for(; i < $2->node_data.nodes[1]->subnodes; i++) {
																									AddSubNode($$->node_data.nodes[1], $2->node_data.nodes[1]->node_data.nodes[i]);
																								}
																								//free($2->node_data.nodes[1]->node_data.nodes);
																								//free($2->node_data.nodes[1]);
																							}
																							//free($2->node_data.nodes);
																							//free($2);
																						}
																					} else if ($2 != NULL && $2->subnodes) {
																						wakeerror("Cannot invoke anonymous function with named segments"); YYERROR;
																					} else {
																						// Turn (expression) and (NT_INVOCATION_PARTS_TEMP (expressions)
																						// into (NT_LAMBDA_INVOCATION (expression) (expressions)
																						$$ = MakeOneBranchNode(NT_LAMBDA_INVOCATION, $1, @$);
																						if ($2 != NULL) {
																							AddSubNode($$, $2->node_data.nodes[0]);
																						}
																					}
																				}
	;

objectable:
	PARENT																		{ $$ = MakeEmptyNode(NT_PARENT, @$); }
	| value																		{ $$ = $1; }
	;

block:
	'{' declarationsandstatements '}'											{ $$ = MakeOneBranchNode(NT_BLOCK, $2, @$); }
	| '{' '}'																	{ $$ = MakeEmptyNode(NT_EMPTY, @$); }
	;

declarationsandstatements:
	declarationorstatement														{ $$ = MakeOneBranchNode(NT_RETRIEVALS_STATEMENTS, $1, @$); }
	| declarationsandstatements declarationorstatement							{ $$ = $1; AddSubNode($$, $2); }
	;

declarationorstatement:
	declaration																	{ $$ = $1; }
	| statement																	{ $$ = $1; }
	;

declaration:
	VAR type '=' expression ';'													{ $$ = MakeTwoBranchNode(NT_DECLARATION, MakeNodeFromType($2, @2), $4, @$); }
	| VAR type retrievalargs expression ';'										{	Node* retrieval = MakeTwoBranchNode(NT_RETRIEVAL, MakeNodeFromType($2, @2), $3, @$);
																					AddSubNode(retrieval, $4);
																					$$ = MakeTwoBranchNode(NT_DECLARATION, MakeNodeFromType(copyType($2), @2), retrieval, @$);		}
	;

retrievalargs:
	FROM																		{ $$ = MakeEmptyNode(NT_EMPTY, @$); }
	| '(' expressions_noretrieval ')' FROM										{ $$ = $2; }
	;

statement:
	emptystatement																{ $$ = MakeEmptyNode(NT_EMPTY, @$); }
	| labelstatement															{ $$ = $1; }
	| expression ';'															{ $$ = $1; }
	| selectionstatement														{ $$ = $1; }
	| iterationstatement														{ $$ = $1; }
	| jumpstatement																{ $$ = $1; }
	| existsstatement															{ $$ = $1; }
	| throwstatement															{ $$ = $1; }
	| trystatement																{ $$ = $1; }
	;

statement_or_block:
	emptystatement																{ $$ = MakeEmptyNode(NT_EMPTY, @$); }
	| labelstatement															{ $$ = $1; }
	| selectionstatement														{ $$ = $1; }
	| iterationstatement														{ $$ = $1; }
	| jumpstatement																{ $$ = $1; }
	| existsstatement															{ $$ = $1; }
	| throwstatement															{ $$ = $1; }
	| trystatement																{ $$ = $1; }
	| expression_nolambda ';'													{ $$ = $1; }
	| block																		{ $$ = $1; }
	;

emptystatement:
	';'
	;

labelstatement:
	CASE expression ':'															{ $$ = MakeOneBranchNode(NT_CASE, $2, @$); }
	| DEFAULT ':'																{ $$ = MakeEmptyNode(NT_DEFAULTCASE, @$); }
	;

existsstatement:
	IF shadowabletype EXISTS statement_or_block									{ $$ = MakeTwoBranchNode(NT_EXISTS, MakeNodeFromType($2, @2), $4, @$); }
	| IF LIDENTIFIER EXISTS statement_or_block									{ $$ = MakeTwoBranchNode(NT_EXISTS, MakeNodeFromString(NT_ALIAS, $2, @2), $4, @$); }
	| IF shadowabletype EXISTS statement_or_block ELSE statement_or_block		{ $$ = MakeTwoBranchNode(NT_EXISTS, MakeNodeFromType($2, @2), $4, @$); AddSubNode($$, $6); }
	| IF LIDENTIFIER EXISTS statement_or_block ELSE statement_or_block			{ $$ = MakeTwoBranchNode(NT_EXISTS, MakeNodeFromString(NT_ALIAS, $2, @2), $4, @$); AddSubNode($$, $6); }
	;

selectionstatement:
	IF '(' expression ')' statement_or_block									{ $$ = MakeTwoBranchNode(NT_IF_ELSE, $3, $5, @$); }
	| IF '(' expression ')' statement_or_block ELSE statement_or_block			{ $$ = MakeTwoBranchNode(NT_IF_ELSE, $3, $5, @$); AddSubNode($$, $7); }
	| SWITCH '(' expression ')' block											{ $$ = MakeTwoBranchNode(NT_SWITCH, $3, $5, @$); }
	;

iterationstatement:
	WHILE '(' expression ')' statement_or_block									{ $$ = MakeTwoBranchNode(NT_WHILE, $3, $5, @$); }
	| FOR '(' forinit forcondition forincrement ')' statement_or_block			{ $$ = MakeTwoBranchNode(NT_FOR, $3, $4, @$); AddSubNode($$, $5); AddSubNode($$, $7); }
	| FOREACH '(' expression ')' statement_or_block								{ $$ = MakeTwoBranchNode(NT_FOREACH, $3, $5, @$); }
	| FOREACH '(' member IN expression ')' statement_or_block					{ $$ = MakeTwoBranchNode(NT_FOREACHIN, $3, $5, @$); AddSubNode($$, $7); }
	;

member:
	shadowabletype																{ $$ = MakeNodeFromType($1, @$); }
	| LIDENTIFIER																{ $$ = MakeNodeFromString(NT_ALIAS, $1, @$); }
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
	expression																	{ $$ = MakeOneBranchNode(NT_EXPRESSIONS, $1, @$); }
	| expressionstatements ',' expression										{ $$ = $1; AddSubNode($$, $3); }
	;

jumpstatement:
	BREAK ';'																	{ $$ = MakeEmptyNode(NT_BREAK, @$); }
	| CONTINUE ';'																{ $$ = MakeEmptyNode(NT_CONTINUE, @$); }
	| RETURN expression ';'														{ $$ = MakeOneBranchNode(NT_RETURN, $2, @$); }
	| RETURN ';'																{ $$ = MakeEmptyNode(NT_RETURN, @$); }
	;

throwstatement:
	THROW expression ';'														{ $$ = MakeOneBranchNode(NT_THROW, $2, @$); }
	;

trystatement:
	TRY block																	{ $$ = MakeOneBranchNode(NT_TRY, $2, @$); }
	| TRY block catchstatement													{ $$ = MakeTwoBranchNode(NT_TRY, $2, $3, @$); }
	;

catchstatement:
	CATCH '(' type ')' block													{ $$ = MakeTwoBranchNode(NT_CATCH, MakeNodeFromType($3, @$), $5, @$); }
	;

expression_unary:
	SYM_INCREMENT expression_unary												{ $$ = MakeOneBranchNode(NT_INCREMENT, $2, @$); }
	| SYM_RETURN_DECREMENT expression_unary										{ $$ = MakeOneBranchNode(NT_DECREMENT, $2, @$); }
	| expression_logicalunary													{ $$ = $1; }
	;

expression_cast:
	value																		{ $$ = $1; }
	| '(' CAST puretype ')' value												{ $$ = MakeTwoBranchNode(NT_CAST, MakeNodeFromType($3, @$), $5, @$); }
	;

expression_logicalunary:
	expression_cast																{ $$ = $1; }
	/*| '~' expression_unary*/
	| '!' expression_cast														{ $$ = MakeOneBranchNode(NT_INVERT, $2, @$); }
	;

expression_multiply:
	expression_unary															{ $$ = $1; }
	| expression_multiply '*' expression_unary									{ $$ = MakeTwoBranchNode(NT_MULTIPLY, $1, $3, @$); }
	| expression_multiply '/' expression_unary									{ $$ = MakeTwoBranchNode(NT_DIVIDE, $1, $3, @$); }
	;

expression_add:
	expression_multiply															{ $$ = $1; }
	| expression_add '+' expression_multiply									{ $$ = MakeTwoBranchNode(NT_ADD, $1, $3, @$); }
	| expression_add '-' expression_multiply									{ $$ = MakeTwoBranchNode(NT_SUBTRACT, $1, $3, @$); }
	;

expression_relational:
	expression_add																{ $$ = $1; }
	| expression_relational '<' expression_add									{ $$ = MakeTwoBranchNode(NT_LESSTHAN, $1, $3, @$); }
	| expression_relational '>' expression_add									{ $$ = MakeTwoBranchNode(NT_GREATERTHAN, $1, $3, @$); }
	| expression_relational SYM_LE expression_add								{ $$ = MakeTwoBranchNode(NT_LESSTHANEQUAL, $1, $3, @$); }
	| expression_relational SYM_GE expression_add								{ $$ = MakeTwoBranchNode(NT_GREATERTHANEQUAL, $1, $3, @$); }
	;

expression_equality:
	expression_relational														{ $$ = $1; }
	| expression_equality SYM_EQ expression_relational							{ $$ = MakeTwoBranchNode(NT_EQUALITY, $1, $3, @$); }
	| expression_equality SYM_NE expression_relational							{ $$ = MakeTwoBranchNode(NT_INEQUALITY, $1, $3, @$); }
	;

expression_conditionaland:
	expression_equality															{ $$ = $1; }
	| expression_conditionaland SYM_AND expression_equality						{ $$ = MakeTwoBranchNode(NT_AND, $1, $3, @$); }
	;

expression_conditionalor:
	expression_conditionaland													{ $$ = $1; }
	| expression_conditionalor SYM_OR expression_conditionaland					{ $$ = MakeTwoBranchNode(NT_OR, $1, $3, @$); }
	;

expression_conditional:
	expression_conditionalor													{ $$ = $1; }
	| expression_conditionalor IF expression_conditional ELSE expression		{ $$ = MakeTwoBranchNode(NT_IF_THEN_ELSE, $1, $3, @$); AddSubNode($$, $5); }
	;

expression_retrieval:
	expression_conditional														{ $$ = $1; }
	| type retrievalargs expression_conditional									{ $$ = MakeTwoBranchNode(NT_RETRIEVAL, MakeNodeFromType($1, @$), $2, @$); AddSubNode($$, $3); }
	;

assignment:
	value '=' expression														{ $$ = MakeTwoBranchNode(NT_ASSIGNMENT, $1, $3, @$); }
	| value SYM_VALEQ expression												{ $$ = MakeTwoBranchNode(NT_VALUED_ASSIGNMENT, $1, $3, @$); }
	| value SYM_PLUSEQ expression												{ $$ = MakeTwoBranchNode(NT_ADD_ASSIGNMENT, $1, $3, @$); }
	| value SYM_SUBEQ expression												{ $$ = MakeTwoBranchNode(NT_SUB_ASSIGNMENT, $1, $3, @$); }
	| value SYM_MULTEQ expression												{ $$ = MakeTwoBranchNode(NT_MULT_ASSIGNMENT, $1, $3, @$); }
	| value SYM_DIVEQ expression												{ $$ = MakeTwoBranchNode(NT_DIV_ASSIGNMENT, $1, $3, @$); }
	;

expression:
	expression_retrieval														{ $$ = $1; }
	| lambda																	{ $$ = $1; }
	| assignment																{ $$ = $1; }
	;

lambda:
	'{' inferenceabletypes SYM_LAMBDA declarationsandstatements '}'				{ $$ = MakeTwoBranchNode(NT_LAMBDA_DECLARATION, $2, $4, @$); }
	;

expression_noretrieval:
	expression_conditional														{ $$ = $1; }
	| assignment																{ $$ = $1; }
	;

expression_nolambda:
	expression_retrieval														{ $$ = $1; }
	| assignment																{ $$ = $1; }
	;

expressions:
	expression																	{ $$ = MakeOneBranchNode(NT_EXPRESSIONS, $1, @$); }
	| expressions ',' expression												{ $$ = $1; AddSubNode($$, $3); }
	;

expressions_noretrieval:
	expression_noretrieval														{ $$ = MakeOneBranchNode(NT_EXPRESSIONS, $1, @$); }
	| expressions_noretrieval ',' expression_noretrieval						{ $$ = $1; AddSubNode($$, $3); }
	;

identifier:
	UIDENTIFIER																	{ $$ = $1; }
	| LIDENTIFIER																{ $$ = $1; }
	;

annotatedtype:
	annotations type															{ $$ = MakeTwoBranchNode(NT_ANNOTATED_TYPE, MakeNodeFromType($2, @2), $1, @$); }
	| type																		{ $$ = MakeNodeFromType($1, @$); }
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
	| SYM_SHADOW classtype														{ $$ = $2;
																					Type* classtype = $$;
																					while(classtype->type != TYPE_CLASS)
																						classtype = classtype->type == TYPE_OPTIONAL ? classtype->typedata.optional.contained : classtype->typedata.list.contained;
																					classtype->typedata._class.shadow = $1; }
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

inferenceabletypes:
	type																		{ $$ = MakeOneBranchNode(NT_INFERENCEABLE_TYPES, MakeNodeFromType($1, @1), @$); }
	| LIDENTIFIER																{ $$ = MakeOneBranchNode(NT_INFERENCEABLE_TYPES, MakeNodeFromString(NT_ALIAS, $1, @1), @$); }
	| inferenceabletypes ',' type												{ $$ = $1; AddSubNode($$, MakeNodeFromType($3, @3)); $$->loc = @$; }
	| inferenceabletypes ',' LIDENTIFIER										{ $$ = $1; AddSubNode($$, MakeNodeFromString(NT_ALIAS, $3, @3)); $$->loc = @$; }
	;

classtype:
	parameterizedtype															{ $$ = $1; }
	| classtype '?'																{ $$ = MakeType(TYPE_OPTIONAL); $$->typedata.optional.contained = $1; }
	| classtype SYM_ARRAYED														{ $$ = MakeType(TYPE_LIST); $$->typedata.list.contained = $1; }
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
	| fntype '?'																{ $$ = MakeType(TYPE_OPTIONAL); $$->typedata.optional.contained = $1; }
	| fntype SYM_ARRAYED														{ $$ = MakeType(TYPE_LIST); $$->typedata.list.contained = $1; }
	;

unboundtypespecifications:
	unboundtypespecification													{ $$ = MakeTypeArray(); AddTypeToTypeArray($1, $$); }
	| unboundtypespecifications ',' unboundtypespecification					{ $$ = $1; AddTypeToTypeArray($3, $1); }
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
