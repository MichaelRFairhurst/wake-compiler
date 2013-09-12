%{
#include <stdio.h>
#include "tree.h"
#include "y.tab.h"

extern YYSTYPE yylval;
extern int line;
extern int column;

void count();

%}

%%
every				count(); return EVERY;
capable				count(); return CAPABLE;
an?					count(); return A_OR_AN;
is					count(); return IS;
return				count(); return RETURN;
foreach				count(); return FOREACH;
with				count(); return WITH;
public				count(); return PUBLIC;
if					count(); return IF;
else				count(); return ELSE;
while				count(); return WHILE;
in					count(); return IN;
import				count(); return IMPORT;
provides			count(); return PROVIDES;
needs				count(); return NEEDS;
then				count(); return THEN;
case				count(); return CASE;
default				count(); return DEFAULT;
switch				count(); return SWITCH;
for					count(); return FOR;
break				count(); return BREAK;
do					count(); return DO;
continue			count(); return CONTINUE;
this				count(); return THIS;
parent				count(); return PARENT;
fn					count(); return FN;

:					count(); return ':';
@					count(); return '@';
\(					count(); return '(';
\)					count(); return ')';
\{					count(); return '{';
\}					count(); return '}';
\[					count(); return '[';
\]					count(); return ']';
;					count(); return ';';
,					count(); return ',';
\.					count(); return '.';
-					count(); return '-';
=					count(); return '=';
\/					count(); return '/';
\\					count(); return '\\';
#					count(); return '#';
\|					count(); return '|';
\*					count(); return '*';
!					count(); return '!';
\?					count(); return '?';
\+					count(); return '+';
\<					count(); return '<';
\>					count(); return '>';
\+\+				count(); return SYM_INCREMENT;
!=					count(); return SYM_NE;
==					count(); return SYM_EQ;
&&					count(); return SYM_AND;
\|\|				count(); return SYM_OR;
\<-					count(); return SYM_PROVIDE;
\<=					count(); return SYM_LE;
--					count(); return SYM_RETURN_DECREMENT;
\?\?\?				count(); return SYM_CURRIER;

[\t ]				count();
\n					line++; column = 1;

\$+					count(); yylval.number = strlen(yytext); return SYM_SHADOW;
(\[\])+				count(); yylval.number = strlen(yytext)>>1; return SYM_ARRAYED;

True				count(); yylval.number = 1; return TRUTH;
False				count(); yylval.number = 0; return TRUTH;
[0-9]+				count(); yylval.number = atoi(yytext); return NUMBER;
\{[a-zA-Z0-9]+\}	count(); yylval.string = strdup(yytext); return SPECIALTY;
[a-zA-Z0-9]+		count(); yylval.string = strdup(yytext); return IDENTIFIER;
@[a-zA-Z0-9]+		count(); yylval.string = strdup(yytext); return ALIAS;
\"(\\.|[^\\"])*\"	count(); yylval.string = strdup(yytext); return(STRING);
%%

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\t')
			column += 4 - ((column + 1) % 4);
		else
			column++;
}
