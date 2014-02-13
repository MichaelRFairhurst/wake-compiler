%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <objectfile.h>

extern char* objectfiletext;

objectfile* myobjectfile;

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

classes: CLASSES symbollist			{
										myobjectfile->classcount = myobjectfile->propertycount;
										myobjectfile->classes = myobjectfile->properties;
									}

properties: PROPERTIES symbollist

usages: USAGES symbolusages

symbolusages: /* empty*/
	| symbolusages symbolusage

symbolusage: NUMBER SYMBOL			{
										symbolusage* mysymbolusage = malloc(sizeof(symbolusage));
										mysymbolusage->pos = $1;
										mysymbolusage->symbol = $2;
										myobjectfile->usages = realloc(myobjectfile->usages, ++myobjectfile->usagecount * sizeof(symbolusage*));
										myobjectfile->usages[myobjectfile->usagecount-1] = mysymbolusage;
									}

symbollist: /*empty*/
	| symbollist SYMBOL				{
										myobjectfile->properties = realloc(myobjectfile->properties, ++myobjectfile->propertycount * sizeof(char*));
										myobjectfile->properties[myobjectfile->propertycount-1] = $2;
									}

%%
