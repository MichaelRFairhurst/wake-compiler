#ifndef HEADER_SEMANTIC_ERROR_PRINTER
#define HEADER_SEMANTIC_ERROR_PRINTER
#include "SemanticError.h"

class SemanticErrorPrinter {
	public:
		virtual void print(SemanticError* e);
};

#endif
