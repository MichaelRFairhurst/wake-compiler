/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * SemanticErrorPrinter.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_SEMANTIC_ERROR_PRINTER
#define HEADER_SEMANTIC_ERROR_PRINTER
#include "SemanticError.h"

class SemanticErrorPrinter {
	public:
		virtual void print(SemanticError* e);
};

#endif
