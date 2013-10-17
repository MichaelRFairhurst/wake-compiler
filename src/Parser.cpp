#include "Parser.h"
#include <iostream>

extern "C" int yylex_destroy  (void);

bool Parser::parse(const char* input) {
	parsetree = NULL;
	yy_scan_string(input);
	return yyparse();
}

bool Parser::parse(FILE* input) {
	parsetree = NULL;
	yyin = input;
	return yyparse();
}

Node* Parser::getParseTree() {
	return parsetree;
}

void Parser::print() {
	printtree(parsetree, 0);
}

Parser::~Parser() {
	yylex_destroy();
	if(parsetree != NULL) freeNode(parsetree);
}
