%{
/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * objectfileparser.y
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

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
%token CLASSES PROPERTIES ERRORTOKEN

%union
{
        int number;
        char *string;
}

%token <string> SYMBOL
%token <number> NUMBER
%start file
%%

file: classes properties

classes: CLASSES symbolusages		{
										myobjectfile->classusagecount = myobjectfile->propertyusagecount;
										myobjectfile->classusages = myobjectfile->propertyusages;
										myobjectfile->propertyusagecount = 0;
										myobjectfile->propertyusages = NULL;
									}

properties: PROPERTIES symbolusages

symbolusages: /* empty*/
	| symbolusages symbolusage

symbolusage: NUMBER SYMBOL			{
										symbolusage* mysymbolusage = malloc(sizeof(symbolusage));
										mysymbolusage->pos = $1;
										mysymbolusage->symbol = $2;
										myobjectfile->propertyusages = realloc(myobjectfile->propertyusages, ++myobjectfile->propertyusagecount * sizeof(symbolusage*));
										myobjectfile->propertyusages[myobjectfile->propertyusagecount-1] = mysymbolusage;
									}

%%
