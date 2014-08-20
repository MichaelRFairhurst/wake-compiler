/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MockSemanticErrorPrinter.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_MOCK_SEMANTIC_ERROR_PRINTER
#define HEADER_MOCK_SEMANTIC_ERROR_PRINTER

#include <vector>
#include "SemanticErrorPrinter.h"

class MockSemanticErrorPrinter : public SemanticErrorPrinter {

	public:
		MockSemanticErrorPrinter() { _passed = true; }
		void expect(SemanticErrorCode codes);
		virtual void print(SemanticError* e);
		bool passed();

	private:
		std::vector<SemanticErrorCode> expectations;
		bool _passed;

};
#endif
