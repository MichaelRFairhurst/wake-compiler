/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * wake.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "tree.h"
extern "C" {
	#include "node.h"
	#include "wake.tab.h"
	extern int yyparse();
	extern Node* parsetree;
	extern FILE *yyin;
}

#include "error.h"
#include "Parser.h"
#include "Linker.h"
#include "ParseTreeTraverser.h"
#include "SemanticErrorPrinter.h"
#include "LibraryLoader.h"
#include "ObjectFileGenerator.h"
#include "ObjectFileHeaderData.h"
#include "ObjectFileHeaderRenderer.h"
#include "CompilationExceptions.h"
#include "OptionsParser.h"
#include "EntryPointAnalyzer.h"
#include "AddressAllocator.h"
#include "TableFileWriter.h"
#include "SimpleAddressTable.h"
#include "ImportParseTreeTraverser.h"
#include "ErrorTracker.h"
#include "CompileTargetPrecedenceConfigFactory.h"
#include "PrecedenceEnforcer.h"
#include <boost/filesystem.hpp>
#include <boost/unordered_set.hpp>
using namespace boost;

void writeTableFiles(std::string dirname, ClassSpaceSymbolTable& table) {
	vector<PropertySymbolTable*> tables = table.getDefinedClasses();
	for(vector<PropertySymbolTable*>::iterator it = tables.begin(); it != tables.end(); ++it) {
		fstream file;

		string filename = dirname;
		if(table.getModule().size()) {
			filename += "/" + table.getModule();
			boost::filesystem::create_directory(filename);
		}
		filename += "/" + (*it)->getClassname() + ".table";

		file.open((filename).c_str(), ios::out | ios::binary);
		TableFileWriter writer;
		writer.write(file, *it);
		file.close();
	}
}

void compileFile(Options* options) {

	if(options->inFilenames.size() == 0) {
		printf("[ no file provided ]\n"); exit(1);
	}

	if(options->inFilenames.size() != options->outFilenames.size() && !options->table) {
		printf("[ mismatched counts of input files and output files ]\n"); exit(1);
	}

	ptr_vector<Parser> parsers;
	ptr_vector<ClassSpaceSymbolTable> classTables;
	vector<ClassSpaceSymbolTable*> vectorClassTables;
	ptr_vector<ParseTreeTraverser> parseTreeTraversers;
	ptr_vector<ErrorTracker> errorTrackers;

	for(int i = 0; i < options->inFilenames.size(); ++i) {
		FILE *myfile = fopen(options->inFilenames[i].c_str(), "r");
		error_open_file(options->inFilenames[i].c_str());

		if (!myfile) {
			printf("[ couldn't open file ]\n");
			exit(2);
		}

		parsers.push_back(new Parser());
		Parser& parser = parsers.back();

		classTables.push_back(new ClassSpaceSymbolTable());
		ClassSpaceSymbolTable& table = classTables.back();
		vectorClassTables.push_back(&table);

		errorTrackers.push_back(new ErrorTracker());
		ErrorTracker& errorTracker = errorTrackers.back();

		// Parse the shit out of this
		if(parser.parse(myfile)) exit(3);
		//parser.print();exit(0);

		if(parser.getParseTree()->node_data.nodes[0]->node_type == NT_MODULE) {
			table.setModule(parser.getParseTree()->node_data.nodes[0]->node_data.string);
		}

		ImportParseTreeTraverser importer;
		importer.prepImports(parser.getParseTree(), table);
		LibraryLoader loader;
		loader.prepLangModule(&table);

		// Now do all the semantic analysis
		parseTreeTraversers.push_back(new ParseTreeTraverser(&table, errorTracker));
		ParseTreeTraverser& traverser = parseTreeTraversers.back();
		traverser.classGatheringPass(parser.getParseTree());
	}

	for(int i = 0; i < options->inFilenames.size(); ++i) {
		LibraryLoader loader;
		loader.loadLangModule(&classTables[i]);
		ImportParseTreeTraverser importer;
		errorTrackers[i].pushContext("Import header");
		importer.traverse(parsers[i].getParseTree(), classTables[i], loader, errorTrackers[i], options->tabledir, vectorClassTables);
		errorTrackers[i].popContext();

		parseTreeTraversers[i].methodGatheringPass(parsers[i].getParseTree());
	}

	for(int i = 0; i < options->inFilenames.size(); ++i) {
		parseTreeTraversers[i].finalPass(parsers[i].getParseTree());
	}

	SemanticErrorPrinter printer;

	bool quit = false;
	for(int i = 0; i < parseTreeTraversers.size(); ++i)
	if(!parseTreeTraversers[i].passesForCompilation()) {
		error_open_file(options->inFilenames[i].c_str());
		parseTreeTraversers[i].printErrors(printer);
		quit = true;
	}

	if(quit) exit(4);

	if(options->table) {
		for(ptr_vector<ClassSpaceSymbolTable>::iterator it = classTables.begin(); it != classTables.end(); ++it) {
			writeTableFiles(options->tabledir, *it);
		}

		return;
	}

	CompileTargetPrecedenceConfigFactory precedenceConfigFactory;
	CompileTargetPrecedenceConfig javaPrecedence = precedenceConfigFactory.java();
	PrecedenceEnforcer precedenceEnforcer(javaPrecedence);

	for(int i = 0; i < options->inFilenames.size(); ++i) {
		precedenceEnforcer.enforce(parsers[i].getParseTree());
		basic_ostringstream<char> object;
		ObjectFileHeaderData header;

		ObjectFileGenerator gen(object, &classTables[i], &header);
		gen.generate(parsers[i].getParseTree());

		fstream file;
		ObjectFileHeaderRenderer renderer;

		file.open(options->outFilenames[i].c_str(), ios::out);
		renderer.writeOut(file, &header);
		file << object.str();
		file.close();

		writeTableFiles(options->tabledir, classTables[i]);
	}
}

void findRecursiveDeps(string module, string classname, map<pair<string, string>, vector<pair<string, string> > >& depInfoMap, unordered_set<pair<string, string> >& recursiveDeps) {
	if(recursiveDeps.find(pair<string, string>(module, classname)) != recursiveDeps.end()) {
		return;
	}

	recursiveDeps.insert(pair<string, string>(module, classname));

	vector<pair<string, string> > importingClasses = depInfoMap[pair<string, string>(module, classname)];

	for(vector<pair<string, string> >::iterator it = importingClasses.begin(); it != importingClasses.end(); ++it) {
		findRecursiveDeps(it->first, it->second, depInfoMap, recursiveDeps);
	}
}

void gatherDependencyInfo(Options* options, string module, string classname, map<pair<string, string>, vector<pair<string, string> > >& depInfoMap, unordered_set<pair<string, string> >& allDeps) {
	boost::filesystem::path p(options->inFilenames[0]);
	boost::filesystem::path dir = p.parent_path();
	dir /=  classname + ".wk"; // sweet operator overloading!
	string filename = dir.string();

	if(allDeps.count(pair<string, string>(module, classname))) {
		return;
	}

	allDeps.insert(pair<string, string>(module, classname));

	vector<pair<string, string> > singleFileImports;

	FILE *myfile = fopen(filename.c_str(), "r");

	if (!myfile) {
		cerr << "Warning: couldn't find dependency " << filename << endl;
		cerr << "It won't be searched for cyclical dependencies." << endl;
		cerr << "Is it in a different source directory, or misnamed?" << endl;
		return;
	}

	{
		Parser parser;
		if(parser.parse(myfile)) return;

		ImportParseTreeTraverser importer;
		singleFileImports = importer.gatherImports(parser.getParseTree());
		fclose(myfile);
	}

	for(vector<pair<string, string> >::iterator it = singleFileImports.begin(); it != singleFileImports.end(); ++it) {
		if(depInfoMap.find(*it) == depInfoMap.end()) {
			vector<pair<string, string> > vec;
			vec.push_back(pair<string, string>(module, classname));
			depInfoMap[*it] = vec;
		} else {
			depInfoMap.find(*it)->second.push_back(pair<string, string>(module, classname));
		}

		if(it->first == module) {
			gatherDependencyInfo(options, it->first, it->second, depInfoMap, allDeps);
		} else {
			allDeps.insert(*it);
		}
	}
}

void printDependencies(Options* options) {
	vector<pair<string, string> > singleFileImports;
	unordered_set<pair<string, string> > allDeps;
	unordered_set<pair<string, string> > recursiveDeps;
	map<pair<string, string>, vector<pair<string, string> > > depInfoMap;
	string module;
	string classname;

	if(options->inFilenames.size() == 0) {
		printf("[ no file provided ]\n"); exit(1);
	}

	if(options->inFilenames.size() > 1) {
		printf("[ too many files provided ]\n"); exit(1);
	}

	FILE *myfile = fopen(options->inFilenames[0].c_str(), "r");

	if (!myfile) {
		printf("[ couldn't open file ]\n");
		exit(2);
	}

	{
		Parser parser;
		if(parser.parse(myfile)) return;

		Node* tree = parser.getParseTree();
		module = tree->node_data.nodes[0]->node_data.string;

		tree = tree->node_data.nodes[tree->subnodes - 1];
		while(tree->node_type != NT_TYPEDATA) {
			tree = tree->node_data.nodes[0];
		}

		classname = tree->node_data.pure_type->typedata._class.classname;

		ImportParseTreeTraverser importer;
		singleFileImports = importer.gatherImports(parser.getParseTree());
		fclose(myfile);
	}

	for(vector<pair<string, string> >::iterator it = singleFileImports.begin(); it != singleFileImports.end(); ++it) {
		vector<pair<string, string> > vec;
		vec.push_back(pair<string, string>(module, classname));
		depInfoMap[*it] = vec;
	}

	allDeps.insert(pair<string, string>(module, classname));

	for(vector<pair<string, string> >::iterator it = singleFileImports.begin(); it != singleFileImports.end(); ++it)
	if(it->first == module) {
		gatherDependencyInfo(options, it->first, it->second, depInfoMap, allDeps);
	} else {
		allDeps.insert(*it);
	}

	if(depInfoMap.count(pair<string, string>(module, classname))) {
		findRecursiveDeps(module, classname, depInfoMap, recursiveDeps);
	}

	for(unordered_set<pair<string, string> >::iterator it = allDeps.begin(); it != allDeps.end(); ++it)
	if(it->first != module || it->second != classname) {
		if(recursiveDeps.count(*it)) {
			cout << it->first << "," << it->second << ",TRUE" << endl;
		} else {
			cout << it->first << "," << it->second << ",FALSE" << endl;
		}
	}
}

int main(int argc, char** argv) {
	OptionsParser optionsparser;
	Options options = optionsparser.parse(argc, argv);
	if(options.showVersion) {
		printf("[ Wake    ---- std compiler ]\n");
		printf("[ v0.2.1  Michael Fairhurst ]\n");
		exit(0);
	}

	if(options.showHelp) {
		printf("usage: wake flags filename\n");
		printf("flags: -o|--out file             - compile to this file\n");
		printf("       -c|--mainclass class      - begin execution with this file\n");
		printf("       -m|--mainemethod method   - begin execution with this method\n");
		printf("       -v|--version              - show version and exit\n");
		printf("       -h|--help                 - show help and exit\n");
		printf("       -i|--listmains            - list compilable entrypoints\n");
		printf("       -l|--link                 - link compiled files into an executable\n");
		printf("       -t|--table                - only generate table files\n");
		printf("       -d|--tabledir             - dir for finding and creating .table files\n");
		printf("       -e|--dependencies         - print out dependency information instead of compiling\n");
		exit(0);
	}

	if(!options.link) {
		options.listDeps ? printDependencies(&options) : compileFile(&options);
	} else {
		try {
			AddressAllocator classAllocator;
			AddressAllocator propAllocator;
			ClassSpaceSymbolTable table;
			SimpleAddressTable classTable(classAllocator);
			SimpleAddressTable propTable(propAllocator);
			Linker linker(classTable, propTable);
			for(std::vector<std::string>::iterator it = options.inFilenames.begin(); it != options.inFilenames.end(); ++it) {
				linker.loadObject(*it);
			}

			LibraryLoader loader;
			loader.loadLangModule(&table);
			linker.loadTables(options.tabledir, table);

			try {
				table.assertNoNeedsAreCircular();
			} catch(SemanticError* e) {
				e->token = NULL;
				SemanticErrorPrinter printer;
				printer.print(e);
				delete e;
				return 1;
			}

			fstream file;
			file.open(options.outFilenames[0].c_str(), ios::out);

			linker.write(file);
			linker.writeDebugSymbols(file); // @todo make this optional

			EntryPointAnalyzer entrypointanalyzer;
			if(options.listMains) {
				table.printEntryPoints(&entrypointanalyzer);
				exit(0);
			} else {
				if(!entrypointanalyzer.checkFQClassMethodCanBeMain(options.mainclass, options.mainmethod, &table)) {
					printf("Entry point %s.%s in not valid, cannot continue.\nTry wake yourfile --listmains to get entry points\n", options.mainclass.c_str(), options.mainmethod.c_str());
					exit(5);
				}
			}

			linker.setMain(file, options.mainclass, options.mainmethod, table);
		} catch(SymbolNotFoundException* e) {
			cout << "Missing symbol in object files at link time: " << e->errormsg << endl;
			delete e;
		}

	}

}
