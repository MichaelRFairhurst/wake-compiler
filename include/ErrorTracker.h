/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ErrorTracker.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_ERROR_TRACKER
#define HEADER_ERROR_TRACKER

#include <string>
#include <vector>
#include "SemanticError.h"
#include "SemanticErrorPrinter.h"

using namespace std;

class ErrorTracker {
	public:
		~ErrorTracker();
		void addError(SemanticError* e);
		void pushContext(string context);
		void popContext();
		void printErrors(SemanticErrorPrinter& p);
		bool passesForCompilation();

	private:
		vector<SemanticError*> errors;
		vector<string> contexts;

};

#endif
