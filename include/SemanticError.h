#ifndef HEADER_SEMANTIC_ERROR
#define HEADER_SEMANTIC_ERROR

#include <string>
using namespace std;

extern "C" {
	#include "type.h"
	#include "node.h"
}

class SemanticError {

	public:
		SemanticError(Node* errortoken, string errormsg);
		string msg;

	private:
		Node* token;

};

#endif
