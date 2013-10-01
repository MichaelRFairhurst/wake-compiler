#include <stdio.h>
#include <iostream>

extern "C" {
	#include "type.h"
	#include "node.h"
	#include "tree.h"
	#include "y.tab.h"
	extern int yyparse();
	extern Node* parsetree;
	extern FILE *yyin;
}

#include "Parser.h"
#include "ParseTreeTraverser.h"
#include "SemanticErrorPrinter.h"

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("[ no file provided ]\n"); exit(1);
	}

	if(strcmp("--version", argv[1]) == 0 || strcmp("-v", argv[1]) == 0) {
		printf("[ Wake    ---- std compiler ]\n");
		printf("[ v0.01   Michael Fairhurst ]\n");
		exit(0);
	}

	FILE *myfile = fopen(argv[1], "r");

	if (!myfile) {
		printf("[ couldn't open file ]\n");
		exit(2);
	}

	Parser parser;

	// Parse the shit out of this
	if(parser.parse(myfile)) exit(1);
	//parser.print();

	// Now do all the semantic analysis
	ParseTreeTraverser traverser;
	traverser.traverse(parser.getParseTree());

	SemanticErrorPrinter printer;

	//if(traverser.getErrors()->size())
		traverser.printErrors(printer);
	//else
		//cout << "Everything checked out!\n" << endl;
}
