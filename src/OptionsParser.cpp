#include "OptionsParser.h"

#include <string>

using namespace std;

Options* OptionsParser::parse(int argc, char** argv) {
	int i = 0;

	Options* options = new Options;
	options->hasErrors = false;
	options->showHelp = false;
	options->listMains = false;
	options->showVersion = false;
	options->outfilename = "a.out";
	options->mainclass = "Main";
	options->mainmethod = "Main";

	while(i < argc) {
		string arg = argv[i]; i++;
		string nextarg;
		if(i < argc) nextarg = argv[i];

		if(arg.at(0) != '-') {
			if(arg == "wake") continue;

			options->infilename = arg;
		}

		else if(arg == "-v" || arg == "--version") {
			options->showVersion = true;
		}

		else if(arg == "-h" || arg == "--help") {
			options->showHelp = true;
		}

		else if(arg == "-l" || arg == "--listmains") {
			options->listMains = true;
		}

		else if(arg == "-o" || arg == "--out") {
			options->outfilename = nextarg; i++;
		}

		else if(arg == "-c" || arg == "--mainclass") {
			options->mainclass = nextarg; i++;
		}

		else if(arg == "-m" || arg == "--mainmethod") {
			options->mainmethod = nextarg; i++;
		}

		else {
			options->hasErrors = true;
		}
	}

	return options;
}
