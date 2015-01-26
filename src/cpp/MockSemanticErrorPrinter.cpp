/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MockSemanticErrorPrinter.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <iostream>
#include "MockSemanticErrorPrinter.h"

void MockSemanticErrorPrinter::expect(SemanticErrorCode code) {
	expectations.push_back(code);
}

void MockSemanticErrorPrinter::print(SemanticError* e) {
	if(expectations.size()) {

		SemanticErrorCode code = expectations.back();

		if(e->code == code) {
			expectations.pop_back();
			return;
		}
	}

	_passed = false;
	cout << "this error SHOULD NOT have been raised:" << endl;
	SemanticErrorPrinter::print(e);
	return;
}

bool MockSemanticErrorPrinter::passed() {
	if(_passed && !expectations.size()) return true;

	if(expectations.size()) {
		cout << "this error SHOULD have been raised:" << endl;
		Node* n = 0;
		SemanticErrorPrinter::print(new SemanticError(expectations[0], "this error should have been raised", n));

	}

	return false;
}
