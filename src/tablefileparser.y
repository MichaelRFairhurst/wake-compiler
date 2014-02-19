%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char* tablefiletext;

void tablefileerror(const char *str)
{
        fprintf(stderr, "error parsing table file: %s\n", str);
}

int tablefilewrap()
{
        return 1;
}

%}

%token ABSTRACT PUBLIC METHODS NEEDS LAMBDA TYPE RETURN ERRORTOKEN

%union
{
        int number;
        char *string;
}

%token <string> SYMBOL
%token <number> NUMBER BOOLEAN
%start file
%%

file: SYMBOL BOOLEAN NEEDS types METHODS methods { printf("classname is %s, abstract is %d", $1, $2); }

types: /* empty */
	| types type

methods: /* empty */
	| methods SYMBOL type { printf("method name is %s", $2); }
	| methods PUBLIC SYMBOL type { printf("public method name is %s", $3); }
	| methods ABSTRACT SYMBOL type { printf("abstract method name is %s", $3); }
	| methods PUBLIC ABSTRACT SYMBOL type { printf("public abstract method name is %s", $4); }

type: TYPE SYMBOL NUMBER NUMBER SYMBOL SYMBOL NUMBER { printf("type, name is %s, shadow is %d, arrayed is %d, alias is %s, specialty is %s, optional is %d", $2, $3, $4, $5, $6, $7); }
	| LAMBDA RETURN type types NUMBER SYMBOL SYMBOL NUMBER { printf("lambda, return type plus args, arrayed is %d, alias is %s, specialty is %s, optional is %d", $5, $6, $7, $8); }
	| LAMBDA types NUMBER SYMBOL SYMBOL NUMBER { printf("void lambda, arrayed is %d, alias is %s, specialty is %s, optional is %d", $3, $4, $5, $6); }

%%
