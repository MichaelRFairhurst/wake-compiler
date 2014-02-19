%{
#include <stdio.h>
#include "tablefile.tab.h"
#include "parseUtil.h"

extern YYSTYPE tablefilelval;

%}

%%

"Y"						tablefilelval.number = 0; return BOOLEAN;
"N"						tablefilelval.number = 1; return BOOLEAN;

"A"						return ABSTRACT;
"E"						return NEEDS;
"L"						return LAMBDA;
"P"						return PUBLIC;
"R"						return RETURN;
"T"						return TYPE;

[0-9]+(\.[0-9]+)?		tablefilelval.number = atoi(tablefiletext); return NUMBER;
\{[^}]+\}				tablefilelval.string = keepInner(tablefiletext); return SYMBOL;
\{\}					tablefilelval.string = NULL; return SYMBOL;

[ \n\t\r]				// nothing
.						return ERRORTOKEN;
%%
