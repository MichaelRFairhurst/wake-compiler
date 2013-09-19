#include <stdio.h>
#include "SemanticError.h"

SemanticError::SemanticError(SemanticErrorCode errorcode) {
	code = errorcode;
}

SemanticError::SemanticError(SemanticErrorCode errorcode, string errormsg) {
	code = errorcode;
	msg = errormsg;
}

SemanticError::SemanticError(SemanticErrorCode errorcode, string errormsg, Node* errortoken) {
	code = errorcode;
	msg = errormsg;
	token = errortoken;
}

SemanticError::SemanticError(SemanticErrorCode errorcode, string errormsg, Node* errortoken, string errorcontext) {
	code = errorcode;
	msg = errormsg;
	token = errortoken;
	context = errorcontext;
}
