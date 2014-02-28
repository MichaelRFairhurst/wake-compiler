#ifndef HEADER_OPTIONS
#define HEADER_OPTIONS

#include <string>
#include <vector>

struct Options {
	bool hasErrors;
	bool showHelp;
	bool listMains;
	bool showVersion;

	bool link;
	bool table;

	std::vector<std::string> linkFilenames;
	std::string mainclass;
	std::string mainmethod;
	std::string tabledir;

	std::string compileFilename;
	std::string outFilename;
};

#endif
