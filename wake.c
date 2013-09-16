#include <stdio.h>
#include "type.h"
#include "node.h"
#include "tree.h"
#include "y.tab.h"

extern FILE *yyin;

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("[ no file provided ]\n"); exit(1);
	}

	if(strcmp("--version", argv[1]) == 0) {
		printf("[ Wake    ---- std compiler ]\n");
		printf("[ v0.01   Michael Fairhurst ]\n");
		exit(0);
	}

	FILE *myfile = fopen(argv[1], "r");

	if (!myfile) {
		printf("[ couldn't open file ]\n");
		exit(2);
	}

	// set lex to read from it instead of defaulting to STDIN:
	yyin = myfile;

	// And we're off!
	if(!yyparse()) exit(1);
}
