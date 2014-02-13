#include "Parser.h"
#include <iostream>

extern "C" int wakelex_destroy  (void);

bool Parser::parse(const char* input) {
	parsetree = NULL;
	wake_scan_string(input);
	return wakeparse();
}

bool Parser::parse(FILE* input) {
	parsetree = NULL;
	wakein = input;
	return wakeparse();
}

Node* Parser::getParseTree() {
	return parsetree;
}

void Parser::print() {
	printtree(parsetree, 0);
}

Parser::~Parser() {
	wakelex_destroy();
	if(parsetree != NULL) freeNode(parsetree);
}
