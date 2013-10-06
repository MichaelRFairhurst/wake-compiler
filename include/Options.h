#ifndef HEADER_OPTIONS
#define HEADER_OPTIONS

#include <string>

struct Options {
	bool hasErrors;
	bool showHelp;
	bool listMains;
	bool showVersion;
	std::string infilename;
	std::string outfilename;
	std::string mainclass;
	std::string mainmethod;
};

#endif
