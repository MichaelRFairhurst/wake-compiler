#ifndef HEADER_COMPILATION_EXCEPTIONS
#define HEADER_COMPILATION_EXCEPTIONS

class CompilationException {
	public:
		string errormsg;
};

class SymbolNotFoundException : public CompilationException {
};

#endif
