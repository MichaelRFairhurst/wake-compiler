#ifndef HEADER_SEMANTIC_ERROR
#define HEADER_SEMANTIC_ERROR

#include <string>
using namespace std;

extern "C" {
	#include "type.h"
	#include "node.h"
}

enum SemanticErrorCode {
	MULTIPLE_CLASS_DEFINITION,		// defining class A twice
	SELF_INHERITANCE,				// A implements or extends A
	CIRCULAR_INHERITANCE,			// A inherits from B inherits from A
	MORE_THAN_ONE_SUBCLASS,			// A extends B and C
	MULTIPLE_INHERITANCE,			// A inherits from B twice
	MULTIPLE_METHOD_DEFINITION,		// A.methodA() defined twice
	CLASSNAME_NOT_FOUND,			// accepting, extending, providing, or instantiating an unknown type
};

class SemanticError {

	public:
		SemanticError(SemanticErrorCode errorcode);
		SemanticError(SemanticErrorCode errorcode, string errormsg);
		SemanticError(SemanticErrorCode errorcode, string errormsg, Node* errortoken);
		SemanticError(SemanticErrorCode errorcode, string errormsg, Node* errortoken, string errorcontext);
		SemanticErrorCode code;
		string msg;
		string context;
		Node* token;

};

#endif
