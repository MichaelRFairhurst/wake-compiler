#include "Parser.h"

bool Parser::parse(char* input) {
	yy_scan_string(input);
	return yyparse();
}

bool Parser::parse(FILE* input) {
	yyin = input;
	return yyparse();
}

Node* Parser::getParseTree() {
	return parsetree;
}

void Parser::print() {
	printtree(parsetree, 0);
}
