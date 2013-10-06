#ifndef HEADER_FLAG_PARSER
#define HEADER_FLAG_PARSER

#include "Options.h"

class OptionsParser {

	public:
		Options* parse(int argc, char** argv);

};

#endif
