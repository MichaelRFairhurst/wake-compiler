/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * OptionsParser.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "OptionsParser.h"

#include <string>

using namespace std;

Options OptionsParser::parse(int argc, char** argv) {
	int i = 1;

	Options options;
	options.hasErrors = false;
	options.link = false;
	options.showHelp = false;
	options.listMains = false;
	options.listDeps = false;
	options.showVersion = false;
	options.table = false;
	//options.outFilename = "a.out";
	options.tabledir = ".";
	options.mainclass = "Main";
	options.mainmethod = "main()";

	bool pastDashO = false;

	while(i < argc) {
		string arg = argv[i]; i++;
		string nextarg;
		if(i < argc) nextarg = argv[i];

		if(arg.at(0) != '-') {
			//if(arg == "wake") continue;

			if(pastDashO) options.outFilenames.push_back(arg);
			else options.inFilenames.push_back(arg);
		}

		else if(arg == "-v" || arg == "--version") {
			options.showVersion = true;
		}

		else if(arg == "-h" || arg == "--help") {
			options.showHelp = true;
		}

		else if(arg == "-i" || arg == "--listmains") {
			options.listMains = true;
		}

		else if(arg == "-o" || arg == "--out") {
			pastDashO = true;
		}

		else if(arg == "-c" || arg == "--mainclass") {
			options.mainclass = nextarg; i++;
		}

		else if(arg == "-m" || arg == "--mainmethod") {
			options.mainmethod = nextarg; i++;
		}

		else if(arg == "-l" || arg == "--link") {
			options.link = true;
		}

		else if(arg == "-t" || arg == "--table") {
			options.table = true;
		}

		else if(arg == "-d" || arg == "--tabledir") {
			options.tabledir = nextarg; i++;
		}

		else if(arg == "-e" || arg == "--dependencies") {
			options.listDeps = true;
		}

		else {
			options.hasErrors = true;
		}
	}

	if(options.outFilenames.size() == 0) {
		options.outFilenames.push_back("a.out");
	}

	return options;
}
