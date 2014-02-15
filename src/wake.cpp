#include <stdio.h>
#include <iostream>

extern "C" {
	#include "type.h"
	#include "node.h"
	#include "tree.h"
	#include "wake.tab.h"
	extern int yyparse();
	extern Node* parsetree;
	extern FILE *yyin;
}

#include "Parser.h"
#include "ParseTreeTraverser.h"
#include "SemanticErrorPrinter.h"
#include "LibraryLoader.h"
#include "ObjectFileGenerator.h"
#include "OptionsParser.h"
#include "EntryPointAnalyzer.h"

int main(int argc, char** argv) {
	OptionsParser optionsparser;
	Options* options = optionsparser.parse(argc, argv);
	if(options->showVersion) {
		printf("[ Wake    ---- std compiler ]\n");
		printf("[ v0.01   Michael Fairhurst ]\n");
		exit(0);
	}

	if(options->showHelp) {
		printf("usage: wake flags filename\n");
		printf("flags: -o|--out file             - compile to this file\n");
		printf("       -c|--mainclass class      - begin execution with this file\n");
		printf("       -m|--mainemethod method   - begin execution with this method\n");
		printf("       -v|--version              - show version and exit\n");
		printf("       -h|--help                 - show help and exit\n");
		printf("       -l|--listmains            - list compilable entrypoints\n");
		exit(0);
	}

	if(options->infilename == "") {
		printf("[ no file provided ]\n"); exit(1);
	}

	FILE *myfile = fopen(options->infilename.c_str(), "r");

	if (!myfile) {
		printf("[ couldn't open file ]\n");
		exit(2);
	}

	Parser parser;

	// Parse the shit out of this
	if(parser.parse(myfile)) exit(3);
	//parser.print();

	ObjectSymbolTable table;
	LibraryLoader loader;
	loader.loadToTable(&table);

	// Now do all the semantic analysis
	ParseTreeTraverser traverser(&table);
	traverser.traverse(parser.getParseTree());

	SemanticErrorPrinter printer;

	if(!traverser.passesForCompilation()) {
		traverser.printErrors(printer);
		exit(4);
	}

	EntryPointAnalyzer entrypointanalyzer;
	if(options->listMains) {
		table.printEntryPoints(&entrypointanalyzer);
		exit(0);
	} else {
		if(!entrypointanalyzer.checkMethodCanBeMain(options->mainclass, options->mainmethod, &table)) {
			printf("Entry point %s.%s in not valid, cannot continue.\nTry wake yourfile -l to get entry points\n", options->mainclass.c_str(), options->mainmethod.c_str());
			exit(5);
		}
	}

	fstream file;
	file.open(options->outfilename.c_str(), ios::out);
	ObjectFileGenerator gen(file, &table);
	gen.generate(parser.getParseTree());
	gen.setMain(options->mainclass, options->mainmethod);
}
