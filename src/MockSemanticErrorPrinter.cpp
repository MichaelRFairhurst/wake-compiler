#include "MockSemanticErrorPrinter.h"

void MockSemanticErrorPrinter::expect(SemanticErrorCode code) {
	expectations.push_back(code);
}

void MockSemanticErrorPrinter::print(SemanticError* e) {
	if(!expectations.size()) { _passed = false; return; }

	SemanticErrorCode code = expectations.back();
	expectations.pop_back();

	if(e->code != code) { _passed = false; return; }
}

bool MockSemanticErrorPrinter::passed() {
	return _passed;
}
