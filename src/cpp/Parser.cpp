/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Parser.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "Parser.h"
#include <iostream>
#include "error.h"
#include "wake.tab.h"

extern "C" int wakelex_destroy  (void);
extern "C" int line;
extern "C" int column;

bool Parser::parse(const char* input) {
	parsetree = NULL;
	error_set_buffer(input);
	wake_scan_string(input);
	line = 1; column = 1;
	bool res = wakeparse();
	cxxParseTree = parsetree;
	return res;
}

bool Parser::parse(FILE* input) {
	parsetree = NULL;
	wakein = input;
	line = 1; column = 1;
	bool res = wakeparse();
	cxxParseTree = parsetree;
	return res;
}

Node* Parser::getParseTree() {
	return cxxParseTree;
}

void Parser::print() {
	printtree(cxxParseTree, 0);
}

Parser::~Parser() {
	wakelex_destroy();
	if(cxxParseTree != NULL) freeNode(cxxParseTree);
}
