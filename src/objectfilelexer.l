%{
#include <stdio.h>
#include "objectfile.tab.h"
#include "parseUtil.h"

extern YYSTYPE objectfilelval;

%}

%%

%-CLASSES-%				return CLASSES;
%-PROPERTIES-%			return PROPERTIES;
%-USAGES-%				return USAGES;
%-END-%					yyterminate();

[0-9]+(\.[0-9]+)?		count(); objectfilelval.number = atoi(objectfiletext); return NUMBER;
\{[a-zA-Z0-9]+\}		count(); objectfilelval.string = keepInner(objectfiletext); return SYMBOL;

.						count(); return ERRORTOKEN;
%%
