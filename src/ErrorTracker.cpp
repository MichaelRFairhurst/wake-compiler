#include "ErrorTracker.h"

void ErrorTracker::addError(SemanticError* e) {
	e->context = contexts.back();
	errors.push_back(e);
}

void ErrorTracker::pushContext(string context) {
	contexts.push_back(context);
}

void ErrorTracker::popContext() {
	contexts.pop_back();
}

void ErrorTracker::printErrors(SemanticErrorPrinter& printer) {
	if(errors.size()) {
		for(std::vector<SemanticError*>::iterator vit = errors.begin(); vit != errors.end(); vit++)
			printer.print(*vit);
	}
}

ErrorTracker::~ErrorTracker() {
	for(vector<SemanticError*>::iterator it = errors.begin(); it != errors.end(); ++it) {
		delete *it;
	}
}
