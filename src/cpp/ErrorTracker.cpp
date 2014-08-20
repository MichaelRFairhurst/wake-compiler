/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ErrorTracker.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

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

bool ErrorTracker::passesForCompilation() {
	for(vector<SemanticError*>::iterator it = errors.begin(); it != errors.end(); ++it) {
		if((*it)->code != WARNING) return false;
	}
	return true;
}

ErrorTracker::~ErrorTracker() {
	for(vector<SemanticError*>::iterator it = errors.begin(); it != errors.end(); ++it) {
		delete *it;
	}
}
