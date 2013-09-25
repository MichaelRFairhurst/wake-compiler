#ifndef HEADER_PARSER
#define HEADER_PARSER

#include <stdio.h>

extern "C" {
	#include "type.h"
	#include "node.h"
	#include "tree.h"
	#include "y.tab.h"
	extern int yyparse();
	extern void yy_scan_string(char* string);
	extern Node* parsetree;
	extern FILE *yyin;
}

class Parser {
	public:
		 bool parse(FILE* input);
		 bool parse(char* input);
		 void print();
		 Node* getParseTree();
		 ~Parser();
};

#endif
