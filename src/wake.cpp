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

#include "ObjectSymTable.h"

int main(int argc, char** argv) {
	ObjectSymTable objsymt;
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
	if(yyparse()) exit(1);
	//printtree(parsetree, 1);

	objsymt.build(parsetree);
	vector<SemanticError>* errors = objsymt.getErrors();
	for(std::vector<SemanticError>::iterator vit = errors->begin(); vit != errors->end(); vit++)
		cout << vit->msg << endl;

}
