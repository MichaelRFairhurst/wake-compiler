%{
#include <stdio.h>
#include "tree.h"
#include "y.tab.h"

extern YYSTYPE yylval;
extern int line;
extern int column;

void count();
char* unEscapeStringLiteral(char* in, char terminator);
char* keepInner(char* in);

%}

%x ST_BLOCKCOMMENT
%x ST_LINECOMMENT

%%

#>						count(); BEGIN(ST_BLOCKCOMMENT);
##						count();
<ST_BLOCKCOMMENT>##		count(); BEGIN(INITIAL);
<ST_BLOCKCOMMENT>.		count();

<INITIAL>\/\/			count(); BEGIN(ST_LINECOMMENT);
<ST_LINECOMMENT>\n		count(); BEGIN(INITIAL);
<ST_LINECOMMENT>.		count();

#[a-zA-Z0-9]+			count();

every					count(); return EVERY;
capable					count(); return CAPABLE;
an?						count(); return A_OR_AN;
is						count(); return IS;
return					count(); return RETURN;
foreach					count(); return FOREACH;
with					count(); return WITH;
public					count(); return PUBLIC;
if						count(); return IF;
else					count(); return ELSE;
while					count(); return WHILE;
in						count(); return IN;
import					count(); return IMPORT;
provides				count(); return PROVIDES;
needs					count(); return NEEDS;
then					count(); return THEN;
case					count(); return CASE;
default					count(); return DEFAULT;
switch					count(); return SWITCH;
for						count(); return FOR;
break					count(); return BREAK;
do						count(); return DO;
continue				count(); return CONTINUE;
this					count(); return THIS;
parent					count(); return PARENT;
fn						count(); return FN;
nothing					count(); return NOTHING;
cast					count(); return CAST;

:						count(); return ':';
@						count(); return '@';
\(						count(); return '(';
\)						count(); return ')';
\{						count(); return '{';
\}						count(); return '}';
\[						count(); return '[';
\]						count(); return ']';
;						count(); return ';';
,						count(); return ',';
\.						count(); return '.';
-						count(); return '-';
=						count(); return '=';
\/						count(); return '/';
\\						count(); return '\\';
#						count(); return '#';
\|						count(); return '|';
\*						count(); return '*';
!						count(); return '!';
\?						count(); return '?';
\+						count(); return '+';
\<						count(); return '<';
\>						count(); return '>';
\+\+					count(); return SYM_INCREMENT;
!=						count(); return SYM_NE;
==						count(); return SYM_EQ;
&&						count(); return SYM_AND;
\|\|					count(); return SYM_OR;
\<-						count(); return SYM_PROVIDE;
\<=						count(); return SYM_LE;
\>=						count(); return SYM_GE;
--						count(); return SYM_RETURN_DECREMENT;
\?\?\?					count(); return SYM_CURRIER;

[\t ]					count();
\n						line++; column = 1;

\$+						count(); yylval.number = strlen(yytext); return SYM_SHADOW;
(\[\])+					count(); yylval.number = strlen(yytext)>>1; return SYM_ARRAYED;

True					count(); yylval.number = 1; return TRUTH;
False					count(); yylval.number = 0; return TRUTH;
[0-9]+(\.[0-9]+)?		count(); yylval.number = atof(yytext); return NUMBER;
\{[a-zA-Z0-9]+\}		count(); yylval.string = keepInner(yytext); return SPECIALTY;
[a-zA-Z0-9]+			count(); yylval.string = strdup(yytext); return IDENTIFIER;
@[a-zA-Z0-9]+			count(); yylval.string = strdup(&yytext[1]); return ALIAS;
\"(\\.|[^\\"])*\"		count(); yylval.string = unEscapeStringLiteral(keepInner(yytext), '"'); return(STRING);
'(\\.|[^\'])*\'			count(); yylval.string = unEscapeStringLiteral(keepInner(yytext), '\''); return(STRING);

.						count(); return ERRORTOKEN;
%%

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
	switch(yytext[i]) {
		case '\t':
			column += 4 - ((column + 1) % 4); break;
		case '\n':
			column = 1; line++; break;
		default:
			column++;
	}
}

char* keepInner(char* in) {
	char* out = malloc(sizeof(char)*strlen(in) - 1);
	memcpy(out, &in[1], strlen(in) - 2);
	out[strlen(in) - 2] = '\0';
	return out;
}

char* unEscapeStringLiteral(char* in, char terminator) {
	int i = 0;
	int c = 0;
	char* out = malloc(sizeof(char) * strlen(in) + 1);

	for(i = 0; i < strlen(in); i++)
	if(in[i] == '\\')
	switch(in[i + 1]) {
		case '\\':
			out[c] = '\\';
			c++; i++; break;
		case 'n':
			out[c] = '\n';
			c++; i++; break;
		case 't':
			out[c] = '\t';
			c++; i++; break;
		case 'r':
			out[c] = '\r';
			c++; i++; break;
		case 'b':
			out[c] = '\b';
			c++; i++; break;
		case 'f':
			out[c] = '\f';
			c++; i++; break;
		case 'v':
			out[c] = '\v';
			c++; i++; break;
		default:
			if(in[i + 1] == terminator) continue;
			out[c] = '\\';
			c++;
	}
	else {
		out[c] = in[i];
		c++;
	}

	out[c] = '\0';

	// free some memory
	out = realloc(out, sizeof(char) * c + 1);
	free(in);
	return out;
}
