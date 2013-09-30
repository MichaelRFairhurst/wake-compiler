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

	private:
		vector<SemanticError*> errors;
		vector<string> contexts;

};

#endif
