/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * CompilationExceptions.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_COMPILATION_EXCEPTIONS
#define HEADER_COMPILATION_EXCEPTIONS

class CompilationException {
	public:
		string errormsg;
};

class SymbolNotFoundException : public CompilationException {
};

#endif
