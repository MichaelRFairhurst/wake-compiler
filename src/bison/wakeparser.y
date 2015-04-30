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
 * Revised By: Nathan Fairhurst
 *
 **************************************************/

#include <stdio.h>
#include <string.h>
#include "node.h"
#include "tree.h"
#include "VarRef.h"
#include "ClassVarRef.h"
#include "VarDecl.h"
#include "SpecializableVarDecl.h"
#include "SpecializablePureType.h"
#include "PureType.h"
#include "PureTypeArray.h"
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
%token EVERY EXTERN CAPABLE KEYWORD_A AN IS RETURN WITH PUBLIC IF ELSE WHILE IMPORT PROVIDES NEEDS THEN NOTHING SWITCH CASE DEFAULT BREAK FOR DO CONTINUE THIS PARENT FN CAST PRIVATE EXISTS VAR FOREACH IN THROW TRY CATCH FROM MODULE
/* symbols */
%token SYM_CURRIER SYM_LE SYM_PROVIDE SYM_RETURN_DECREMENT SYM_AND SYM_OR SYM_EQ SYM_NE SYM_GE SYM_INCREMENT SYM_PLUSEQ SYM_VALEQ SYM_MULTEQ SYM_SUBEQ SYM_DIVEQ SYM_PROVIDE_ARGS_OPEN SYM_EARLYBAILOUT_DOT SYM_TYPESAFE_INDEX SYM_LAMBDA SYM_LAMBDA_AUTORETURN SYM_BITSHIFTLEFT SYM_BITSHIFTRIGHT SYM_MODNATIVE SYM_MODALT;
/* this too */
%token ERRORTOKEN

%locations

%union
{
	double number;
	char* string;
	Node* node;
	PureType* pure_type;
	VarRef* var_ref;
	PureTypeArray* pure_type_array;
	ClassVarRef* class_var_ref;
	VarDecl* var_decl;
	SpecializableVarDecl* specializable_var_decl;
	SpecializablePureType* specializable_pure_type;
}

/* identifiers & values */
%token <string> UIDENTIFIER
%token <string> LIDENTIFIER
%token <string> FQIDENTIFIER
%token <string> SPECIALTY
%token <string> ANNOTATION
%token <string> STRING
%type <string> identifier
%token <number> NUMBER
%token <number> BOOL
%token <number> SYM_SHADOW
%token <void> SYM_ARRAYED
%type <node> imports import classes annotatedclass class parentage inheritances inheritance classbody classprop providable injection injection_subinjections provision provisions injection_subinjection ctor ctorargs value value_invokable method block methodreturn lmethodnamesegments lumethodnamesegments methodbody methodaccess lumethodcallsegments methodcallsegments curryableexpressions expression expressions decls_and_stmts decl_or_stmt decl statement labelstatement existsstatement selectionstatement iterationstatement jumpstatement forinit forcondition forincrement expressionstatements expression_unary expression_logicalunary expression_multiply expression_add expression_bitshift expression_relational expression_bitand expression_bitxor expression_bitor expression_conditionaland expression_conditionalor expression_equality expression_conditional member property property_value retrievalargs objectable expression_cast assignment ctorarg expression_retrieval throwstatement trystatement catchstatement expression_noretrieval expressions_noretrieval provision_args annotatedmethod annotation annotations annotationvals annotationval lambda statement_or_block expression_nolambda module inferenceable_decl_types annotated_specializable_decl_type
%type <pure_type> pure_type function_type class_decl_type unboundtypespecification parameterized_inheritance_type pure_class_type pure_optional_type pure_optional_parameterized_type
%type <class_var_ref> usable_as_variable_type base_class_type
%type <pure_type_array> pure_types unboundtypespecifications decl_types
/*%type <var_ref> */
%type <var_decl> decl_type
%type <specializable_var_decl> specializable_decl_type
%type <specializable_pure_type> specializable_pure_type
%type <string> alias modulename
%start file

/* 3 are from @annotations before class definitions, 1 from if/else, 1 from try/catch, 1 from exists/else */
%expect 6

%expect-rr 0
%%

file:
	module imports classes														{ parsetree = makeTwoBranchNode(NT_PROGRAM, $1, $2, @$); addSubNode(parsetree, $3); }
	| module classes															{ parsetree = makeTwoBranchNode(NT_PROGRAM, $1, $2, @$); }
	;

module:
	/* empty */																	{ YYLTYPE garbage; $$ = makeNodeFromString(NT_MODULE, strdup(""), garbage); }
	| MODULE modulename ';'														{ $$ = makeNodeFromString(NT_MODULE, $2, @1); }
	;

modulename:
	LIDENTIFIER																	{ $$ = $1; }
	| modulename '.' LIDENTIFIER												{ $$ = strcat($1, "."); $$ = strcat($$, $3); }
	;

imports:
	imports import																{ $$ = $1; addSubNode($$, $2); }
	| import																	{ $$ = makeOneBranchNode(NT_IMPORTSET, $1, @$); }
	;

import:
	IMPORT modulename '.' UIDENTIFIER ';'										{ $$ = makeTwoBranchNode(NT_IMPORT, makeNodeFromString(NT_MODULE, $2, @3), makeNodeFromString(NT_IMPORTPATH, $4, @4), @$); }
	| IMPORT UIDENTIFIER ';'													{ $$ = makeTwoBranchNode(NT_IMPORT, makeNodeFromString(NT_MODULE, strdup(""), @2), makeNodeFromString(NT_IMPORTPATH, $2, @2), @$); }
	;

classes:
	classes annotatedclass														{ $$ = $1; addSubNode($$, $2); }
	| annotatedclass															{ $$ = makeOneBranchNode(NT_CLASSSET, $1, @$); }
	;

annotations:
	annotations annotation														{ $$ = $1; addSubNode($$, $2); }
	| annotation																{ $$ = makeOneBranchNode(NT_ANNOTATIONS, $1, @$); }
	;

annotation:
	ANNOTATION																	{ $$ = makeOneBranchNode(NT_ANNOTATION, makeNodeFromString(NT_ANNOTATION_NAME, $1, @$), @$); }
	| ANNOTATION '(' annotationvals ')'											{ $$ = makeTwoBranchNode(NT_ANNOTATION, makeNodeFromString(NT_ANNOTATION_NAME, $1, @$), $3, @$); }
	;

annotationvals:
	annotationvals ',' annotationval											{ $$ = $1; addSubNode($$, $3); }
	| annotationval																{ $$ = makeOneBranchNode(NT_ANNOTATION_VALS, $1, @$); }
	;

annotationval:
	STRING																		{ $$ = makeNodeFromString(NT_STRINGLIT, $1, @$); }
	| NUMBER																	{ $$ = makeNodeFromNumber(NT_NUMBERLIT, $1, @$); }
	| BOOL																		{ $$ = makeNodeFromNumber(NT_BOOLLIT, $1, @$); }
	| NOTHING																	{ $$ = makeEmptyNode(NT_NOTHING, @$); }
	;

annotatedclass:
	class																		{ $$ = $1; }
	| annotations class															{ $$ = makeTwoBranchNode(NT_ANNOTATED_CLASS, $2, $1, @$); }
	;

class:
	EVERY class_decl_type parentage IS ':'									{ $$ = makeTwoBranchNode(NT_CLASS, makeNodeFromPureType($2, @$), $3, @$); }
	| EVERY class_decl_type parentage IS ':' classbody						{ $$ = makeTwoBranchNode(NT_CLASS, makeNodeFromPureType($2, @$), $3, @$); addSubNode($$, $6); }
	| EXTERN class_decl_type parentage IS ':'								{ $$ = makeTwoBranchNode(NT_CLASS_EXTERN, makeNodeFromPureType($2, @$), $3, @$); }
	| EXTERN class_decl_type parentage IS ':' classbody					{ $$ = makeTwoBranchNode(NT_CLASS_EXTERN, makeNodeFromPureType($2, @$), $3, @$); addSubNode($$, $6); }
	;

parentage:
	/* empty */																	{ $$ = makeEmptyNode(NT_EMPTY, @$); }
	| '(' inheritances ')'														{ $$ = $2; }
	;

inheritances:
	inheritances ',' inheritance												{ $$ = $1; addSubNode($$, $3); }
	| inheritance																{ $$ = makeOneBranchNode(NT_INHERITANCESET, $1, @$); }
	;

inheritance:
	CAPABLE parameterized_inheritance_type													{ $$ = makeOneBranchNode(NT_INTERFACE, makeNodeFromPureType($2, @$), @$); }
	| KEYWORD_A parameterized_inheritance_type														{ $$ = makeOneBranchNode(NT_SUBCLASS, makeNodeFromPureType($2, @$), @$); }
	| AN parameterized_inheritance_type														{ $$ = makeOneBranchNode(NT_SUBCLASS, makeNodeFromPureType($2, @$), @$); }
	;

classbody:
	classprop classbody															{ $$ = $2; addSubNode($$, $1); }
	| classprop																	{ $$ = makeOneBranchNode(NT_CLASSBODY, $1, @$); }
	;

classprop:
	PROVIDES provisions ';'														{ $$ = $2; }
	| ctor																		{ $$ = $1; }
	| annotatedmethod															{ $$ = $1; }
	| property																	{ $$ = $1; }
	;

property:
	WITH PUBLIC property_value ';'												{ $$ = makeTwoBranchNode(NT_PROPERTY, $3, makeEmptyNode(NT_PUBLIC, @$), @$); }
	| annotations WITH PUBLIC property_value ';'								{ $$ = makeTwoBranchNode(NT_ANNOTATED_PROPERTY, makeTwoBranchNode(NT_ANNOTATED_PROPERTY, $4, makeEmptyNode(NT_PUBLIC, @$), @$), $1, @$); }
	| WITH property_value ';'													{ $$ = makeOneBranchNode(NT_PROPERTY, $2, @$); }
	| annotations WITH property_value ';'										{ $$ = makeTwoBranchNode(NT_ANNOTATED_PROPERTY, makeOneBranchNode(NT_PROPERTY, $3, @$), $1, @$); }
	;

property_value:
	decl_type '=' expression															{ $$ = makeTwoBranchNode(NT_ASSIGNMENT, makeNodeFromPureType($1, @$), $3, @$); }
	| decl_type retrievalargs expression												{	Node* retrieval = makeTwoBranchNode(NT_RETRIEVAL, makeNodeFromPureType($1, @$), $2, @$);
																					addSubNode(retrieval, $3);
																					$$ = makeTwoBranchNode(NT_DECLARATION, makeNodeFromPureType(copyPureType($1), @$), retrieval, @$);		}
	;

provisions:
	provisions ',' provision													{ $$ = $1; addSubNode($$, $3); }
	| provision																	{ $$ = makeOneBranchNode(NT_PROVISIONS, $1, @$); }
	;

provision:
	specializable_pure_type																		{ $$ = makeOneBranchNode(NT_PROVISION, makeNodeFromPureType($1, @$), @$); }
	| annotations specializable_pure_type															{ $$ = makeTwoBranchNode(NT_ANNOTATED_PROVISION, makeOneBranchNode(NT_PROVISION, makeNodeFromPureType($2, @$), @$), $1, @$); }
	| specializable_pure_type SYM_PROVIDE providable												{ $$ = makeTwoBranchNode(NT_PROVISION, makeNodeFromPureType($1, @$), $3, @$); }
	| annotations specializable_pure_type SYM_PROVIDE providable									{ $$ = makeTwoBranchNode(NT_ANNOTATED_PROVISION, makeTwoBranchNode(NT_PROVISION, makeNodeFromPureType($2, @$), $4, @$), $1, @$); }
	;

providable:
	specializable_pure_type																		{ $$ = makeNodeFromPureType($1, @$); }
	| injection																	{ $$ = $1; }
	| provision_args block														{ $$ = $1; addSubNode($$, $2); }
	| STRING																	{ $$ = makeNodeFromString(NT_STRINGLIT, $1, @$); }
	| NUMBER																	{ $$ = makeNodeFromNumber(NT_NUMBERLIT, $1, @$); }
	| BOOL																		{ $$ = makeNodeFromNumber(NT_BOOLLIT, $1, @$); }
	;

provision_args:
	/* empty */																	{ $$ = makeOneBranchNode(NT_PROVISION_BEHAVIOR, makeNodeFromPureTypeArray(makePureTypeArray(), @$), @$); }
	| '(' decl_types ')'																{ $$ = makeOneBranchNode(NT_PROVISION_BEHAVIOR, makeNodeFromPureTypeArray($2, @$), @$); }
	;

injection:
	UIDENTIFIER '(' injection_subinjections ')'									{ $$ = makeTwoBranchNode(NT_INJECTION, makeNodeFromString(NT_CLASSNAME, $1, @$), $3, @$); }
	;

injection_subinjections:
	injection_subinjections ',' injection_subinjection							{ $$ = $1; addSubNode($$, $3); }
	| injection_subinjection													{ $$ = makeOneBranchNode(NT_SUBINJECTIONS, $1, @$); }
	;

injection_subinjection:
	specializable_pure_type														{ $$ = makeNodeFromSpecalizablePureType($1, @$); }
	| STRING																	{ $$ = makeNodeFromString(NT_STRINGLIT, $1, @$); }
	| NUMBER																	{ $$ = makeNodeFromNumber(NT_NUMBERLIT, $1, @$); }
	| BOOL																		{ $$ = makeNodeFromNumber(NT_BOOLLIT, $1, @$); }
	| '?' pure_type																{ $$ = makeOneBranchNode(NT_INJECTION_ARG, makeNodeFromPureType($2, @$), @$); }
	;

ctor:
	NEEDS ctorargs ';'															{ $$ = makeOneBranchNode(NT_CTOR, $2, @$); }
	| NEEDS ctorargs THEN block													{ $$ = makeTwoBranchNode(NT_CTOR, $2, $4, @$);}
	| NEEDS NOTHING THEN block													{ $$ = makeTwoBranchNode(NT_CTOR, makeEmptyNode(NT_EMPTY, @$), $4, @$);}
	;

ctorargs:
	ctorargs ',' ctorarg														{ $$ = $1; addSubNode($$, $3); }
	| ctorarg																	{ $$ = makeOneBranchNode(NT_CTOR_ARGS, $1, @$); }
	;

ctorarg:
	annotated_specializable_decl_type																{ $$ = makeTwoBranchNode(NT_CTOR_ARG, $1, makeEmptyNode(NT_PRIVATE, @$), @$); }
	| PUBLIC annotated_specializable_decl_type														{ $$ = makeTwoBranchNode(NT_CTOR_ARG, $2, makeEmptyNode(NT_PUBLIC, @$), @$); }
	;

annotatedmethod:
	annotations method															{ $$ = makeTwoBranchNode(NT_ANNOTATED_METHOD, $2, $1, @$); }
	| method																	{ $$ = $1; }
	;

method:
	methodaccess methodreturn lmethodnamesegments methodbody					{ $$ = makeTwoBranchNode(NT_METHOD_DECLARATION, $1, $2, @$); addSubNode($$, $3); addSubNode($$, $4); }
	| methodaccess lmethodnamesegments methodbody								{ $$ = makeTwoBranchNode(NT_METHOD_DECLARATION, $1, $2, @$); addSubNode($$, $3); }
	| methodaccess '{' unboundtypespecifications '}'
		methodreturn lmethodnamesegments methodbody								{ $$ = makeTwoBranchNode(NT_METHOD_DECLARATION, $1, $5, @$); addSubNode($$, $6); addSubNode($$, $7); addSubNode($$, makeNodeFromPureTypeArray($3, @3)); }
	| methodaccess '{' unboundtypespecifications '}'
		lmethodnamesegments methodbody											{ $$ = makeTwoBranchNode(NT_METHOD_DECLARATION, $1, $5, @$); addSubNode($$, $6); addSubNode($$, makeNodeFromPureTypeArray($3, @3)); }
	;

methodaccess:
	/*empty*/																	{ $$ = makeEmptyNode(NT_PUBLIC, @$); }
	| PRIVATE																	{ $$ = makeEmptyNode(NT_PRIVATE, @$); }
	| PUBLIC																	{ $$ = makeEmptyNode(NT_PUBLIC, @$); }
	;

methodreturn:
	pure_type SYM_RETURN_DECREMENT												{ $$ = makeOneBranchNode(NT_METHOD_RETURN_TYPE, makeNodeFromPureType($1, @$), @$); }
	;

methodbody:
	';'																			{ $$ = makeEmptyNode(NT_ABSTRACT_METHOD, @$); }
	| block																		{ $$ = $1; }
	;

lmethodnamesegments:
	alias '(' ')'																{ $$ = makeOneBranchNode(NT_METHOD_NAME, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), @$); }
	| alias '(' decl_types ')'														{ $$ = makeTwoBranchNode(NT_METHOD_NAME, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), makeNodeFromPureTypeArray($3, @$), @$); }
	| alias '(' decl_types ')' lumethodnamesegments									{ $$ = $5; PrependSubNode($5, makeNodeFromPureTypeArray($3, @$)); PrependSubNode($5, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$)); }
	;

lumethodnamesegments:
	identifier '(' ')'															{ $$ = makeOneBranchNode(NT_METHOD_NAME, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), @$); }
	| identifier '(' decl_types ')'													{ $$ = makeTwoBranchNode(NT_METHOD_NAME, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), makeNodeFromPureTypeArray($3, @$), @$); }
	| identifier '(' decl_types ')' lumethodnamesegments								{ $$ = $5; PrependSubNode($5, makeNodeFromPureTypeArray($3, @$)); PrependSubNode($5, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$)); }
	;

methodcallsegments:
	'(' ')'																		{ $$ = NULL; }
	| '(' curryableexpressions ')'												{ $$ = makeOneBranchNode(NT_INVOCATION_PARTS_TEMP, $2, @$); }
	| '(' curryableexpressions ')' lumethodcallsegments							{ $$ = makeTwoBranchNode(NT_INVOCATION_PARTS_TEMP, $2, $4, @$); }
	;

lumethodcallsegments:
	identifier '(' ')'															{ $$ = makeOneBranchNode(NT_METHOD_NAME, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), @$); }
	| identifier '(' curryableexpressions ')'									{ $$ = makeTwoBranchNode(NT_METHOD_NAME, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$), $3, @$); }
	| identifier '(' curryableexpressions ')' lumethodcallsegments				{ $$ = $5; PrependSubNode($5, $3); PrependSubNode($5, makeNodeFromString(NT_METHOD_NAME_SEGMENT, $1, @$)); }
	;

curryableexpressions:
	expression																	{ $$ = makeOneBranchNode(NT_VALUES, $1, @$); }
	| SYM_CURRIER																{ $$ = makeOneBranchNode(NT_VALUES, makeEmptyNode(NT_CURRIED, @$), @$); }
	| curryableexpressions ',' expression										{ $$ = $1; addSubNode($1, $3); }
	| curryableexpressions ',' SYM_CURRIER										{ $$ = $1; addSubNode($1, makeEmptyNode(NT_CURRIED, @$)); }
	;

value:
	usable_as_variable_type														{ $$ = makeNodeFromPureType($1, @$); }
	| value_invokable															{ $$ = $1; }
	| THIS																		{ $$ = makeEmptyNode(NT_THIS, @$); }
	| STRING																	{ $$ = makeNodeFromString(NT_STRINGLIT, $1, @$); }
	| NUMBER																	{ $$ = makeNodeFromNumber(NT_NUMBERLIT, $1, @$); }
	| BOOL																		{ $$ = makeNodeFromNumber(NT_BOOLLIT, $1, @$); }
	| '[' expressions ']'														{ $$ = $2; $$->loc = @$; $$->node_type = NT_ARRAY_DECLARATION; }
	| SYM_ARRAYED																{ $$ = makeEmptyNode(NT_ARRAY_DECLARATION, @$); }
	| NOTHING																	{ $$ = makeEmptyNode(NT_NOTHING, @$); }
	| objectable '.' usable_as_variable_type									{ $$ = makeTwoBranchNode(NT_MEMBER_ACCESS, $1, makeNodeFromPureType($3, @3), @$); }
	| objectable SYM_EARLYBAILOUT_DOT usable_as_variable_type					{ $$ = makeTwoBranchNode(NT_EARLYBAILOUT_MEMBER_ACCESS, $1, makeNodeFromPureType($3, @3), @$); }
	;

value_invokable:
	alias																		{ $$ = makeNodeFromString(NT_ALIAS, $1, @$); }
	| value '[' expression ']'													{ $$ = makeTwoBranchNode(NT_ARRAY_ACCESS, $1, $3, @$); }
	| value SYM_TYPESAFE_INDEX expression ']'									{ $$ = makeTwoBranchNode(NT_TYPESAFE_ARRAY_ACCESS, $1, $3, @$); }
	| objectable '.' alias														{ $$ = makeTwoBranchNode(NT_MEMBER_ACCESS, $1, makeNodeFromString(NT_ALIAS, $3, @3), @$); }
	| objectable SYM_EARLYBAILOUT_DOT alias										{ $$ = makeTwoBranchNode(NT_EARLYBAILOUT_MEMBER_ACCESS, $1, makeNodeFromString(NT_ALIAS, $3, @3), @$); }
	| '(' expression ')'														{ $$ = $2; }
	| value_invokable methodcallsegments										{
																					if ($1->node_type == NT_ALIAS) {
																						// Turn (NT_ALIAS) and (NT_INVOCATION_PARTS_TEMP (expressions) [NT_METHOD_NAME_SEGMENT (METHOD_NAME) (expressions)])
																						// into (NT_METHOD_INVOCATION (NT_THIS) [NT_METHOD_NAME_SEGMENT (METHOD_NAME) (expressions)])
																						$1->node_type == NT_METHOD_NAME_SEGMENT;
																						$$ = makeTwoBranchNode(NT_METHOD_INVOCATION, makeEmptyNode(NT_THIS, @$), makeOneBranchNode(NT_METHOD_NAME, $1, @$), @$);

																						if($2 != NULL) {
																							addSubNode($$->node_data.nodes[1], $2->node_data.nodes[0]);
																							if($2->subnodes == 2) {
																								int i = 0;
																								for(; i < $2->node_data.nodes[1]->subnodes; i++) {
																									addSubNode($$->node_data.nodes[1], $2->node_data.nodes[1]->node_data.nodes[i]);
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
																						$$ = makeOneBranchNode($1->node_type == NT_MEMBER_ACCESS ? NT_METHOD_INVOCATION : NT_EARLYBAILOUT_METHOD_INVOCATION, $1->node_data.nodes[0], @$);
																						addSubNode($$, makeOneBranchNode(NT_METHOD_NAME, makeNodeFromString(NT_METHOD_NAME_SEGMENT, strdup($1->node_data.nodes[1]->node_data.string), $1->node_data.nodes[1]->loc), @1));

																						if($2 != NULL) {
																							addSubNode($$->node_data.nodes[1], $2->node_data.nodes[0]);
																							if($2->subnodes == 2) {
																								int i = 0;
																								for(; i < $2->node_data.nodes[1]->subnodes; i++) {
																									addSubNode($$->node_data.nodes[1], $2->node_data.nodes[1]->node_data.nodes[i]);
																								}
																								//free($2->node_data.nodes[1]->node_data.nodes);
																								//free($2->node_data.nodes[1]);
																							}
																							//free($2->node_data.nodes);
																							//free($2);
																						}
																					} else if ($2 != NULL && $2->subnodes > 1) {
																						wakeerror("Cannot invoke anonymous function with named segments"); YYERROR;
																					} else {
																						// Turn (expression) and (NT_INVOCATION_PARTS_TEMP (expressions)
																						// into (NT_LAMBDA_INVOCATION (expression) (expressions)
																						$$ = makeOneBranchNode(NT_LAMBDA_INVOCATION, $1, @$);
																						if ($2 != NULL) {
																							addSubNode($$, $2->node_data.nodes[0]);
																						}
																					}
																				}
	;

objectable:
	PARENT																		{ $$ = makeEmptyNode(NT_PARENT, @$); }
	| value																		{ $$ = $1; }
	;

block:
	'{' decls_and_stmts '}'											{ $$ = makeOneBranchNode(NT_BLOCK, $2, @$); }
	| '{' '}'																	{ $$ = makeEmptyNode(NT_EMPTY, @$); }
	;

decls_and_stmts:
	decl_or_stmt														{ $$ = makeOneBranchNode(NT_RETRIEVALS_STATEMENTS, $1, @$); }
	| decls_and_stmts decl_or_stmt							{ $$ = $1; addSubNode($$, $2); }
	;

decl_or_stmt:
	decl																	{ $$ = $1; }
	| statement																	{ $$ = $1; }
	;

decl:
	VAR decl_type '=' expression ';'													{ $$ = makeTwoBranchNode(NT_DECLARATION, makeNodeFromPureType($2, @2), $4, @$); }
	| VAR specializable_decl_type retrievalargs expression ';'										{	Node* retrieval = makeTwoBranchNode(NT_RETRIEVAL, makeNodeFromPureType($2, @2), $3, @$);
																					addSubNode(retrieval, $4);
																					$$ = makeTwoBranchNode(NT_DECLARATION, makeNodeFromPureType(copyPureType($2), @2), retrieval, @$);		}
	;

retrievalargs:
	FROM																		{ $$ = makeEmptyNode(NT_EMPTY, @$); }
	| '(' expressions_noretrieval ')' FROM										{ $$ = $2; }
	;

statement:
	emptystatement																{ $$ = makeEmptyNode(NT_EMPTY, @$); }
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
	emptystatement																{ $$ = makeEmptyNode(NT_EMPTY, @$); }
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
	CASE expression ':'															{ $$ = makeOneBranchNode(NT_CASE, $2, @$); }
	| DEFAULT ':'																{ $$ = makeEmptyNode(NT_DEFAULTCASE, @$); }
	;

existsstatement:
	IF usable_as_variable_type EXISTS statement_or_block									{ $$ = makeTwoBranchNode(NT_EXISTS, makeNodeFromPureType($2, @2), $4, @$); }
	| IF alias EXISTS statement_or_block										{ $$ = makeTwoBranchNode(NT_EXISTS, makeNodeFromString(NT_ALIAS, $2, @2), $4, @$); }
	| IF usable_as_variable_type EXISTS statement_or_block ELSE statement_or_block		{ $$ = makeTwoBranchNode(NT_EXISTS, makeNodeFromPureType($2, @2), $4, @$); addSubNode($$, $6); }
	| IF alias EXISTS statement_or_block ELSE statement_or_block				{ $$ = makeTwoBranchNode(NT_EXISTS, makeNodeFromString(NT_ALIAS, $2, @2), $4, @$); addSubNode($$, $6); }
	;

selectionstatement:
	IF '(' expression ')' statement_or_block									{ $$ = makeTwoBranchNode(NT_IF_ELSE, $3, $5, @$); }
	| IF '(' expression ')' statement_or_block ELSE statement_or_block			{ $$ = makeTwoBranchNode(NT_IF_ELSE, $3, $5, @$); addSubNode($$, $7); }
	| SWITCH '(' expression ')' block											{ $$ = makeTwoBranchNode(NT_SWITCH, $3, $5, @$); }
	;

iterationstatement:
	WHILE '(' expression ')' statement_or_block									{ $$ = makeTwoBranchNode(NT_WHILE, $3, $5, @$); }
	| FOR '(' forinit forcondition forincrement ')' statement_or_block			{ $$ = makeTwoBranchNode(NT_FOR, $3, $4, @$); addSubNode($$, $5); addSubNode($$, $7); }
	| FOREACH '(' expression ')' statement_or_block								{ $$ = makeTwoBranchNode(NT_FOREACH, $3, $5, @$); }
	| FOREACH '(' member IN expression ')' statement_or_block					{ $$ = makeTwoBranchNode(NT_FOREACHIN, $3, $5, @$); addSubNode($$, $7); }
	;

member:
	usable_as_variable_type																{ $$ = makeNodeFromPureType($1, @$); }
	| alias																		{ $$ = makeNodeFromString(NT_ALIAS, $1, @$); }
	;

forinit:
	decl																	{ $$ = $1; }
	| expression ';'															{ $$ = $1; }
	;

forcondition:
	expression ';'																{ $$ = $1; }
	;

forincrement:
	expressionstatements														{ $$ = $1; }
	;

expressionstatements:
	expression																	{ $$ = makeOneBranchNode(NT_EXPRESSIONS, $1, @$); }
	| expressionstatements ',' expression										{ $$ = $1; addSubNode($$, $3); }
	;

jumpstatement:
	BREAK ';'																	{ $$ = makeEmptyNode(NT_BREAK, @$); }
	| CONTINUE ';'																{ $$ = makeEmptyNode(NT_CONTINUE, @$); }
	| RETURN expression ';'														{ $$ = makeOneBranchNode(NT_RETURN, $2, @$); }
	| RETURN ';'																{ $$ = makeEmptyNode(NT_RETURN, @$); }
	;

throwstatement:
	THROW expression ';'														{ $$ = makeOneBranchNode(NT_THROW, $2, @$); }
	;

trystatement:
	TRY block																	{ $$ = makeOneBranchNode(NT_TRY, $2, @$); }
	| TRY block catchstatement													{ $$ = makeTwoBranchNode(NT_TRY, $2, $3, @$); }
	;

catchstatement:
	CATCH '(' decl_type ')' block													{ $$ = makeTwoBranchNode(NT_CATCH, makeNodeFromPureType($3, @$), $5, @$); }
	;

expression_unary:
	SYM_INCREMENT expression_unary												{ $$ = makeOneBranchNode(NT_INCREMENT, $2, @$); }
	| SYM_RETURN_DECREMENT expression_unary										{ $$ = makeOneBranchNode(NT_DECREMENT, $2, @$); }
	| '~' expression_unary														{ $$ = makeOneBranchNode(NT_BITNOT, $2, @$); }
	| '-' expression_unary														{ $$ = makeTwoBranchNode(NT_SUBTRACT, makeNodeFromNumber(NT_NUMBERLIT, 0, @2), $2, @$); }
	| expression_logicalunary													{ $$ = $1; }
	;

expression_cast:
	value																		{ $$ = $1; }
	| '(' CAST pure_type ')' value												{ $$ = makeTwoBranchNode(NT_CAST, makeNodeFromPureType($3, @$), $5, @$); }
	;

expression_logicalunary:
	expression_cast																{ $$ = $1; }
	| '!' expression_cast														{ $$ = makeOneBranchNode(NT_INVERT, $2, @$); }
	;

expression_multiply:
	expression_unary															{ $$ = $1; }
	| expression_multiply '*' expression_unary									{ $$ = makeTwoBranchNode(NT_MULTIPLY, $1, $3, @$); }
	| expression_multiply '/' expression_unary									{ $$ = makeTwoBranchNode(NT_DIVIDE, $1, $3, @$); }
	| expression_multiply '%' expression_unary									{ $$ = makeTwoBranchNode(NT_MOD, $1, $3, @$); }
	| expression_multiply SYM_MODNATIVE expression_unary						{ $$ = makeTwoBranchNode(NT_MODNATIVE, $1, $3, @$); }
	| expression_multiply SYM_MODALT expression_unary							{ $$ = makeTwoBranchNode(NT_MODALT, $1, $3, @$); }
	;

expression_add:
	expression_multiply															{ $$ = $1; }
	| expression_add '+' expression_multiply									{ $$ = makeTwoBranchNode(NT_ADD, $1, $3, @$); }
	| expression_add '-' expression_multiply									{ $$ = makeTwoBranchNode(NT_SUBTRACT, $1, $3, @$); }
	| expression_add NUMBER														{
																					if ($2 > 0) {
																						wakeerror("Number follows expression without an operator.");
																						YYERROR;
																					} else {
																						$$ = makeTwoBranchNode(NT_ADD, $1, makeNodeFromNumber(NT_NUMBERLIT,$2,@2), @$);
																					}
																				}
	;

expression_bitshift:
	expression_add																{ $$ = $1; }
	| expression_bitshift SYM_BITSHIFTLEFT expression_add						{ $$ = makeTwoBranchNode(NT_BITSHIFTLEFT, $1, $3, @$); }
	| expression_bitshift SYM_BITSHIFTRIGHT expression_add						{ $$ = makeTwoBranchNode(NT_BITSHIFTRIGHT, $1, $3, @$); }
	;

expression_relational:
	expression_bitshift															{ $$ = $1; }
	| expression_relational '<' expression_bitshift								{ $$ = makeTwoBranchNode(NT_LESSTHAN, $1, $3, @$); }
	| expression_relational '>' expression_bitshift								{ $$ = makeTwoBranchNode(NT_GREATERTHAN, $1, $3, @$); }
	| expression_relational SYM_LE expression_bitshift							{ $$ = makeTwoBranchNode(NT_LESSTHANEQUAL, $1, $3, @$); }
	| expression_relational SYM_GE expression_bitshift							{ $$ = makeTwoBranchNode(NT_GREATERTHANEQUAL, $1, $3, @$); }
	;

expression_equality:
	expression_relational														{ $$ = $1; }
	| expression_equality SYM_EQ expression_relational							{ $$ = makeTwoBranchNode(NT_EQUALITY, $1, $3, @$); }
	| expression_equality SYM_NE expression_relational							{ $$ = makeTwoBranchNode(NT_INEQUALITY, $1, $3, @$); }
	;

expression_bitand:
	expression_equality															{ $$ = $1; }
	| expression_bitand '&' expression_equality									{ $$ = makeTwoBranchNode(NT_BITAND, $1, $3, @$); }
	;

expression_bitxor:
	expression_bitand															{ $$ = $1; }
	| expression_bitxor '^' expression_bitand									{ $$ = makeTwoBranchNode(NT_BITXOR, $1, $3, @$); }
	;

expression_bitor:
	expression_bitxor															{ $$ = $1; }
	| expression_bitor '|' expression_bitxor										{ $$ = makeTwoBranchNode(NT_BITOR, $1, $3, @$); }
	;


expression_conditionaland:
	expression_bitor															{ $$ = $1; }
	| expression_conditionaland SYM_AND expression_bitor						{ $$ = makeTwoBranchNode(NT_AND, $1, $3, @$); }
	;

expression_conditionalor:
	expression_conditionaland													{ $$ = $1; }
	| expression_conditionalor SYM_OR expression_conditionaland					{ $$ = makeTwoBranchNode(NT_OR, $1, $3, @$); }
	;

expression_conditional:
	expression_conditionalor													{ $$ = $1; }
	| expression_conditionalor IF expression_conditional ELSE expression		{ $$ = makeTwoBranchNode(NT_IF_THEN_ELSE, $1, $3, @$); addSubNode($$, $5); }
	;

expression_retrieval:
	expression_conditional														{ $$ = $1; }
	| specializable_pure_type retrievalargs expression_conditional									{ $$ = makeTwoBranchNode(NT_RETRIEVAL, makeNodeFromPureType($1, @$), $2, @$); addSubNode($$, $3); }
	;

assignment:
	value '=' expression														{ $$ = makeTwoBranchNode(NT_ASSIGNMENT, $1, $3, @$); }
	| value SYM_VALEQ expression												{ $$ = makeTwoBranchNode(NT_VALUED_ASSIGNMENT, $1, $3, @$); }
	| value SYM_PLUSEQ expression												{ $$ = makeTwoBranchNode(NT_ADD_ASSIGNMENT, $1, $3, @$); }
	| value SYM_SUBEQ expression												{ $$ = makeTwoBranchNode(NT_SUB_ASSIGNMENT, $1, $3, @$); }
	| value SYM_MULTEQ expression												{ $$ = makeTwoBranchNode(NT_MULT_ASSIGNMENT, $1, $3, @$); }
	| value SYM_DIVEQ expression												{ $$ = makeTwoBranchNode(NT_DIV_ASSIGNMENT, $1, $3, @$); }
	;

expression:
	expression_retrieval														{ $$ = $1; }
	| lambda																	{ $$ = $1; }
	| assignment																{ $$ = $1; }
	;

lambda:
	'{' SYM_LAMBDA decls_and_stmts '}'								{ $$ = makeTwoBranchNode(NT_LAMBDA_DECLARATION, makeEmptyNode(NT_INFERENCEABLE_TYPES, @2), $3, @$); }
	| '{' inferenceable_decl_types SYM_LAMBDA decls_and_stmts '}'			{ $$ = makeTwoBranchNode(NT_LAMBDA_DECLARATION, $2, $4, @$); }
	| '{' inferenceable_decl_types SYM_LAMBDA '}'										{ $$ = makeTwoBranchNode(NT_LAMBDA_DECLARATION, $2, makeEmptyNode(NT_EMPTY, @3), @$); }
	| '{' SYM_LAMBDA_AUTORETURN expression '}'									{ $$ = makeTwoBranchNode(NT_LAMBDA_DECLARATION, makeEmptyNode(NT_INFERENCEABLE_TYPES, @2), makeOneBranchNode(NT_RETURN, $3, @3), @$); }
	| '{' SYM_LAMBDA_AUTORETURN expression ';' '}'								{ $$ = makeTwoBranchNode(NT_LAMBDA_DECLARATION, makeEmptyNode(NT_INFERENCEABLE_TYPES, @2), makeOneBranchNode(NT_RETURN, $3, @3), @$); }
	| '{' inferenceable_decl_types SYM_LAMBDA_AUTORETURN expression '}'				{ $$ = makeTwoBranchNode(NT_LAMBDA_DECLARATION, $2, makeOneBranchNode(NT_RETURN, $4, @4), @$); }
	| '{' inferenceable_decl_types SYM_LAMBDA_AUTORETURN expression ';' '}'			{ $$ = makeTwoBranchNode(NT_LAMBDA_DECLARATION, $2, makeOneBranchNode(NT_RETURN, $4, @4), @$); }
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
	expression																	{ $$ = makeOneBranchNode(NT_EXPRESSIONS, $1, @$); }
	| expressions ',' expression												{ $$ = $1; addSubNode($$, $3); }
	;

expressions_noretrieval:
	expression_noretrieval														{ $$ = makeOneBranchNode(NT_EXPRESSIONS, $1, @$); }
	| expressions_noretrieval ',' expression_noretrieval						{ $$ = $1; addSubNode($$, $3); }
	;

identifier:
	UIDENTIFIER																	{ $$ = $1; }
	| alias																		{ $$ = $1; }
	;

alias:
	LIDENTIFIER																	{ $$ = $1; }
	| KEYWORD_A																	{ $$ = strdup("a"); }
	| AN																		{ $$ = strdup("an"); }
	;

usable_as_variable_type:
	base_class_type																{ $$ = $1; }
	| SYM_SHADOW base_class_type												{ $$ = $2; $$->shadow = $1; }
	;

decl_type:
	alias pure_type																{ $$ = makeVarDecl($2); $$->alias = strdup($1); }
	| pure_type alias															{ $$ = makeVarDecl($1); $$->alias = strdup($2); }
	| pure_type																	{ $$ = makeVarDecl($1); }
	| SYM_SHADOW pure_class_type												{ $$ = makeVarDecl($2); $$->shadow = $1; }
	;

inferenceable_decl_types:
	decl_type																		{ $$ = makeOneBranchNode(NT_INFERENCEABLE_TYPES, makeNodeFromPureType($1, @1), @$); }
	| alias																		{ $$ = makeOneBranchNode(NT_INFERENCEABLE_TYPES, makeNodeFromString(NT_ALIAS, $1, @1), @$); }
	| inferenceable_decl_types ',' decl_type												{ $$ = $1; addSubNode($$, makeNodeFromPureType($3, @3)); $$->loc = @$; }
	| inferenceable_decl_types ',' alias												{ $$ = $1; addSubNode($$, makeNodeFromString(NT_ALIAS, $3, @3)); $$->loc = @$; }
	;

pure_type:
	pure_class_type																{ $$ = $1; }
	| function_type																{ $$ = $1; }
	;

specializable_pure_type:
	pure_type																	{ $$ = $1; }
	| pure_type SPECIALTY														{ $$ = $1; $$->specialty = strdup($2); }
	;

specializable_decl_type:
	alias pure_type																{ $$ = makeSpecializableVarDecl($2); $$->decl.alias = strdup($1); }
	| alias pure_type SPECIALTY													{ $$ = makeSpecializableVarDecl($2); $$->decl.alias = strdup($1); $$->specialty = strdup($3); }
	| pure_type alias															{ $$ = makeSpecializableVarDecl($1); $$->decl.alias = strdup($2); }
	| pure_type SPECIALTY alias													{ $$ = makeSpecializableVarDecl($1); $$->decl.alias = strdup($3); $$->specialty = strdup($2); }
	| pure_type																	{ $$ = makeSpecializableVarDecl($1); }
	| pure_type SPECIALTY														{ $$ = makeSpecializableVarDecl($1); $$->specialty = strdup($2); }
	| SYM_SHADOW pure_class_type												{ $$ = makeSpecializableVarDecl($2); $$->decl.shadow = $1; }
	| SYM_SHADOW pure_class_type SPECIALTY										{ $$ = makeSpecializableVarDecl($2); $$->decl.shadow = $1; $$->specialty = strdup($3); }
	;

base_class_type:
	UIDENTIFIER																	{ $$ = makeClassVarRef($1, 0, 0); }
	| base_class_type SYM_ARRAYED												{ $$->arrayed++; }
	;

pure_class_type:
	base_class_type																{ $$ = makeTypeFromClassVarRef($1); }
	| pure_optional_type														{ $$ = $1; }
	| pure_optional_parameterized_type											{ $$ = $1; }
	;

pure_optional_type:
	base_class_type '?'															{ $$ = makePureType(TYPE_OPTIONAL); $$->typedata.optional.contained = makeTypeFromClassVarRef($1); }
	| pure_optional_type '?'													{ $$ = makePureType(TYPE_OPTIONAL); $$->typedata.optional.contained = $1; }
	| pure_optional_type SYM_ARRAYED											{ $$ = makePureType(TYPE_LIST); $$->typedata.list.contained = $1; }
	;

pure_optional_parameterized_type:
	UIDENTIFIER '{' pure_types '}'												{ $$ = makePureType(TYPE_CLASS); $$->typedata._class.classname = strdup($1); $$->typedata._class.parameters = $3; }
	| pure_optional_parameterized_type SYM_ARRAYED								{ $$ = makePureType(TYPE_LIST); $$->typedata.list.contained = $1; }
	| pure_optional_parameterized_type '?'										{ $$ = makePureType(TYPE_OPTIONAL); $$->typedata.optional.contained = $1; }
	;

pure_types:
	pure_type																	{ $$ = makePureTypeArray(); addPureTypeToPureTypeArray($1, $$); }
	| pure_types ',' pure_type													{ $$ = $1; addPureTypeToPureTypeArray($3, $$); }
	;

function_type:
	pure_type SYM_RETURN_DECREMENT FN '(' pure_types ')'							{ $$ = makePureType(TYPE_LAMBDA); $$->typedata.lambda.returntype = $1; $$->typedata.lambda.arguments = $5; }
	| pure_type SYM_RETURN_DECREMENT FN '(' ')'									{ $$ = makePureType(TYPE_LAMBDA); $$->typedata.lambda.returntype = $1; }
	| FN '(' pure_types ')'														{ $$ = makePureType(TYPE_LAMBDA); $$->typedata.lambda.arguments = $3; }
	| FN '(' ')'																{ $$ = makePureType(TYPE_LAMBDA); }
	| function_type '?'																{ $$ = makePureType(TYPE_OPTIONAL); $$->typedata.optional.contained = $1; }
	| function_type SYM_ARRAYED														{ $$ = makePureType(TYPE_LIST); $$->typedata.list.contained = $1; }
	;

class_decl_type:
	UIDENTIFIER																	{ $$ = makePureType(TYPE_CLASS); $$->typedata._class.classname = $1; }
	| UIDENTIFIER '{' unboundtypespecifications '}'								{ $$ = makePureType(TYPE_CLASS); $$->typedata._class.classname = $1; $$->typedata._class.parameters = $3; }
	;

unboundtypespecifications:
	unboundtypespecification													{ $$ = makePureTypeArray(); addPureTypeToPureTypeArray($1, $$); }
	| unboundtypespecifications ',' unboundtypespecification					{ $$ = $1; addPureTypeToPureTypeArray($3, $1); }
	;

unboundtypespecification:
	UIDENTIFIER																	{ $$ = makePureType(TYPE_PARAMETERIZED); $$->typedata.parameterized.label = $1; }
	/* | UIDENTIFIER from pure_type
	   | UIDENTIFIER to pure_type
	   | UIDENTIFIER from pure_type to pure_type */
	;

parameterized_inheritance_type:
	UIDENTIFIER																	{ $$ = makePureType(TYPE_CLASS); $$->typedata._class.classname = $1; }
	| UIDENTIFIER '{' pure_types '}'												{ $$ = makePureType(TYPE_CLASS); $$->typedata._class.classname = $1; $$->typedata._class.parameters = $3; }
	;

decl_types:
	decl_type																		{ $$ = makePureTypeArray(); addPureTypeToPureTypeArray($1, $$); }
	| decl_types ',' decl_type															{ $$ = $1; addPureTypeToPureTypeArray($3, $$); }
	;

annotated_specializable_decl_type:
	annotations specializable_decl_type														{ $$ = makeTwoBranchNode(NT_ANNOTATED_TYPE, makeNodeFromPureType($2, @2), $1, @$); }
	| specializable_decl_type																		{ $$ = makeNodeFromPureType($1, @$); }
	;

%%
