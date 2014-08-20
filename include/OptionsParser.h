/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * OptionsParser.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_FLAG_PARSER
#define HEADER_FLAG_PARSER

#include "Options.h"

class OptionsParser {

	public:
		Options* parse(int argc, char** argv);

};

#endif
