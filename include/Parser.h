/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Parser.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_PARSER
#define HEADER_PARSER

#include <stdio.h>
#include "tree.h"

extern "C" {
	#include "node.h"
	#include "wake.tab.h"
	extern int wakeparse();
	extern void wake_scan_string(const char* string);
	extern Node* parsetree;
	extern FILE *wakein;
}

class Parser {
	public:
		 bool parse(FILE* input);
		 bool parse(const char* input);
		 void print();
		 Node* getParseTree();
		 ~Parser();

	private:
		Node* cxxParseTree;
};

#endif
