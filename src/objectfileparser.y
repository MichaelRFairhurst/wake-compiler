%{
#include <stdio.h>
#include <string.h>

extern char* objectfiletext;

void objectfileerror(const char *str)
{
        fprintf(stderr, "error parsing object file: %s\n", str);
}

int objectfilewrap()
{
        return 1;
}

%}

/* keywords */
%token CLASSES PROPERTIES USAGES ERRORTOKEN

%union
{
        int number;
        char *string;
}

%token <string> SYMBOL
%token <number> NUMBER
%start file
%%

file: classes properties usages

classes: CLASSES symbollist

properties: PROPERTIES symbollist

usages: USAGES symbolusages

symbolusages: /* empty*/
	| symbolusages symbolusage

symbolusage: NUMBER SYMBOL

symbollist: /*empty*/
	| symbollist SYMBOL

%%
