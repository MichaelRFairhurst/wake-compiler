%{
/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * objectfilelexer.l
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <stdio.h>
#include "objectfile.tab.h"
#include "parseUtil.h"

extern YYSTYPE objectfilelval;

%}

%%

"%-CLASSES-%"			return CLASSES;
"%-PROPERTIES-%"		return PROPERTIES;
"%-END-%"				yyterminate();

[0-9]+(\.[0-9]+)?		objectfilelval.number = atoi(objectfiletext); return NUMBER;
%[^%]+%					objectfilelval.string = keepInner(objectfiletext); return SYMBOL;

[ \n\t\r]				// nothing
.						return ERRORTOKEN;
%%
